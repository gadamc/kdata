#!/usr/bin/env python

'''
  This script checks the local Samba Raw Data event directory (currently the link 'rawdata')
  and copies any new files found in that directory to HPSS. First, this script must tar
  the files before sending the data to HPSS, and it must check that the files are large enough
  in size. 
  
'''
 
import sys, os, glob, tarfile, tempfile, string, time, subprocess, datetime, shlex, shutil

def timeout(func, args=(), kwargs={}, timeout_duration=10, default=None):
    """This function will spawn a thread and run the given function
    using the args, kwargs and return the given default value if the
    timeout_duration is exceeded.
    """ 
    import threading
    class InterruptableThread(threading.Thread):
        def __init__(self):
            threading.Thread.__init__(self)
            self.result = default
        def run(self):
            self.result = func(*args, **kwargs)
    it = InterruptableThread()
    it.start()
    it.join(timeout_duration)
    if it.isAlive():
        return it.result
    else:
        return it.result

def formatvalue(value):
  if (isinstance(value,str)):
    # #see if this string is really an int or a float
    if value.isdigit()==True: #int
      return int(value)
    else: #try a float
      try:
        if math.isnan(float(value))==False:
          return float(value)
      except:
        pass
    
    return value.strip('"') #strip off any quotations found in the value
  else:
    return value


def setToCurrentTransfer(thefile, item):
    file = open(thefile,'w')
    file.write(item + '\n')
    file.close()
    
    
def appendSmallRunListFile(filename, listoffiles):
  
  currentlist = list()
  
  '''first open the file to get the current list of small files in the file'''
  if os.path.isfile(filename):
    file = open(filename,'rU')
    for line in file:
      currentlist.append(line.rstrip('\n'))
    file.close()
  
  '''open the file to append new files to the list'''
  file = open(filename, 'a')
  for item in listoffiles:
    if item not in currentlist:
      file.write(str(item) + '\n')
      
  file.close()


def getfoldersize(folder):
  '''returns the folder size in MB'''
  folder_size = 0
  for (path, dirs, files) in os.walk(folder):
    for file in files:
      filename = os.path.join(path, file)
      folder_size += os.path.getsize(filename)      
  
  return folder_size/(1024.0*1024.0)


def sortListBySize(fileList, min = None, max = None):

  if min == None:
    min = 0
  
  transfer = list()
  small = list()
  big = list()
  
  for item in fileList:
    size = getfoldersize(item)
    
    if size > min:
      if max != None:
        if size > max:
          big.append(item)
      else:
        transfer.append(item)
    else:
      small.append(item)
  
  all = list()
  all.append(transfer)
  all.append(small)
  all.append(big)

  return all


def getDictOfLastFiles(filename):
  '''  read each line of the file and get the last file name, fill a dictionary and return it.
    each element in the dictionary corresponds to a particular mac, with the key being the mac letter (a, b, c, d, ...) '''

  letters = string.lowercase
  lastfiles = {}
  for i in range(len(letters)):
    lastfiles[letters[i]] = ''  #initialize the keys of the dictionary

  if os.path.isfile(filename):
    file = open(filename, 'rU')
    for line in file:
      lastfiles[ str(os.path.basename(line)[4:5]) ] = os.path.basename(line).strip()

    file.close()
  
  return lastfiles
  
  
def addItemToLastFiles(lastfilename, item):
    
    lastFiles = getDictOfLastFiles(lastfilename)
    lastFiles[os.path.basename(item)[4:5]] = os.path.basename(item).strip()
          
    file = open(lastfilename,'w') 
    for key, value in lastFiles.items():
      if value != '':
        file.write(value + '\n')
    
    file.close()
    
  

def getSambaDirPattern():
  return '[d-n][a-m][0-9][0-9][a-z][0-9][0-9][0-9]'


def isDirReady(dir):
  '''checks to see if this directory is ready, which is indicated by the existence of the log and seuils files
    also, should check to make sure that they are at least ~25 minute old so that we don't copy directories
    where the log and seuils files are still being copied.
  '''
  isokay = False
  tnow = time.time()
  
  dirlist = dir.strip('/').split('/')
  sambafile = dirlist[len(dirlist)-1]
  #test if dir is really a dir
  if os.path.isdir(dir):
    logfile = dir.rstrip('/') + '/' + sambafile + '_log'
    #seuilsfile = dir.rstrip('/') + '/' + sambafile + '_seuils'
    #test if the file _log and _seuils exist in the dir
    if os.path.isfile(logfile): # and os.path.isfile(seuilsfile):
      flogfiletime = os.path.getmtime(logfile)
      #fseuilstime = os.stat(seuilsfile).st_mtime
      
      #also test if the last time modified was more than 5 minutes
      if tnow > flogfiletime + 25.0*60.0: # and tnow > fseuilstime + 15.0*60.0:
        isokay = True
      
  
  return isokay
  
  
def getListOfNewSambaDirs(params):
  
  dictOfLastFiles = getDictOfLastFiles( params['lastcopyfile'] )
    
  if params['datadir'] == '.' or params['datadir'] == '':
    globsearch = getSambaDirPattern()
  else:
    globsearch = params['datadir'] + '/' + getSambaDirPattern()

  allsamba = glob.glob(globsearch)
  
  f = list()
  for item in allsamba:
    if os.path.basename(item) > dictOfLastFiles[ str(os.path.basename(item)[4:5]) ]:
      if isDirReady(item):
        f.append(item)
        
  return f


def tarTheDir(adir, tempDir = None):
  if tempDir == None or tempDir == '.':
    filename = os.path.basename(adir) + '.tar'
  else:
    tempDir.rstrip('/')
    filename = tempDir + '/' + os.path.basename(adir) + '.tar'
  
  file = tarfile.open(filename, 'w')
  file.add(adir, arcname = os.path.basename(adir))
  file.close()
  
  return filename


def checkArguments(p):
  '''checks to make sure that paths are directories and that files are files'''
  return True
  

def readArguments(arglist):
  
  p = {'minfilesize':200,'timeout_hours':23.5, 'workingdir':'/Users/adam/Scripts/copyToHpss', \
  'srbpath':'/usr/local/SRB3.5.0_MacOS_intel/bin','mdaspath':'/Users/adam/.srb', \
  'srbdestination':'/edw/edw2rawdata'}

  p['lastcopyfile'] = p['workingdir'] + '/lastCopyToHpss.txt'
  p['datadir'] = p['workingdir'] + '/rawdata'
  p['logfile'] = p['workingdir'] + '/copyToHpss.log'
  p['currentTransferFile'] = p['workingdir'] + '/currentTransfer.txt'
  p['smallrunlist'] = p['workingdir'] + '/smallrunlist.txt'
  
  for i in range(len(arglist)):
  
    if arglist[i] == '-t':
      i += 1
      val = formatvalue(arglist[i])
      if isinstance(val,int) or isinstance(val,float):
        p['timeout_hours'] = val
      else:
        print 'Invalid value for timeout.', val, 'Set to default', p['timeout_hours']
    
    else:
      pass
      #we don't know this option
      #print 'The option', arglist[i], 'is unknown. Exiting.'
      #sys.exit(-1)

  if checkArguments(p):
    return p
  else:
    return False
    

def getmonthdir(afile):
  months = {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}
  return months[afile[1]]  #assume, of course, standard samba file structure...

def getyeardir(afile):
  years = {'i':2008, 'j':2009, 'k':2010, 'l':2011, 'm':2012, 'n':2013, 'o':2014, 'p':2015, 'q':2016}
  return years[afile[0]]  #assume, of course, standard samba file structure...


def formatSrbYearMonthDirStructure(file):
  basename = os.path.basename(file)
  return str(getyeardir(basename)) + '/' + getmonthdir(basename) + str((getyeardir(basename) - 2000)) + '/events'

def runSubProcess(command):
  args = shlex.split(command)
  print command
  logfile.flush()
  return subprocess.Popen(args)
  
def uploadToHpss(theparams, tarfile):

  spath = theparams['srbpath'].rstrip('/')

  sinit = runSubProcess(spath +'/Sinit')
  sinit.wait()
  
  hpssdir = theparams['srbdestination'] + '/' + formatSrbYearMonthDirStructure(tarfile.rstrip('.tar'))
    
  smkdir = runSubProcess(spath + '/Smkdir -p ' + hpssdir)
  smkdir.wait()
  scd = runSubProcess(spath +'/Scd ' + hpssdir)
  scd.wait()
    
  setToCurrentTransfer(theparams['currentTransferFile'], os.path.basename(tarfile).rstrip('.tar'))
  sput = runSubProcess(spath + '/Sput -fMvK -r ' + tarfile)
  sput.wait()
    
  addItemToLastFiles(theparams['lastcopyfile'], tarfile.rstrip('.tar'))
  setToCurrentTransfer(theparams['currentTransferFile'], '')
  #need to catch errors here
  #the setToLastCopyFile should make sure to output a last-file for each 'mac' 
  
  sexit  = runSubProcess(spath + '/Sexit')
  
   
  #end by writing out last files to the output
  

#______________________________

'''
main script
'''

def runCopy(params):
  
  print 'Initial parameters:'
  for k, v in params.items():
      print k, ' = ', v

  newDirs = getListOfNewSambaDirs(params)
  print ''
  print 'List of new Samba Directories:'
  print newDirs
  newDirsbysize = sortListBySize(newDirs, params['minfilesize'])
  print ''
  print 'Sorting By Size:'
  
  
  transferlist = newDirsbysize[0]   #we should ensure the order of the files in the list so that if transfer is interrupted
  smalllist = newDirsbysize[1]
  biglist = newDirsbysize[2]  #i haven't specified a max size in sortListBySize
                              #and currently we ship all big files
                              #however, we could consider ignoring big files
                              #or shipping them later, or just deleting them

  print 'transferlist', transferlist
  print 'smalllist', smalllist
  print 'biglist', biglist
  
  print 'Appending small run directories to', params['smallrunlist']
  appendSmallRunListFile(params['smallrunlist'], smalllist)
  
  print 'Tarring files'
  tempDir = tempfile.mkdtemp()
  print 'in directory', tempDir
  logfile.flush()
  try:
    tarlist = list()
    for item in transferlist:
      tarfile = tarTheDir(item, tempDir)
      tarlist.append(tarfile)
      print ''
      print 'Uploading to Hpss', tarfile
      logfile.flush()
      uploadToHpss(params, tarfile)
 
    # maybe I don't want to use a random directory after all
    # if i used a local, specific directory, i could recover from 
    # errors more easily. 
    #
    
  
  except:
    shutil.rmtree(tempDir)
    print sys.exc_info()[0], sys.exe_info()[1]
    raise sys.exc_info()[0]
  
  #now, check the small file list.  First need to check to see
  #if there are any files not part of the current month in
  #in the small file list, and if there are, pack them into a 
  #tar file IF they are going to be larger than the MinimumFileSize (200 MB)
  #If they are not larger than 200MB, then we leave them there until the next month
   
  #transferlist = getSmallFilesToTransfer(params['smallrunlist'])
  #smalltarlist = list()
  
  shutil.rmtree(tempDir)
  return len(tarlist)

#______________________________
if __name__ == '__main__':

  start = time.time()
  params = readArguments(sys.argv)
  if params == False:
    print 'Something is wrong with the parameters'
    sys.exit(-1)
  
  logfile = open(params['logfile'], 'a')
  sys.stdout = logfile
  sys.stderr = logfile
  print ''
  print 'Starting transfer script', str(os.uname()), ':', datetime.datetime.now()
  print ''
  logfile.flush()
  
  returncode = timeout(runCopy, (params,), timeout_duration = params['timeout_hours']*60.0*60.0, default = 'TimedOut')
  
  if returncode == 'TimedOut':
    print 'Either an error occured, or the process timed-out before', params['timeout_hours'], 'hours.', datetime.datetime.now()
    
  else:
    ndocs = returncode
    delta = time.time() - start
    rate = float(ndocs)/float(delta)
    ndocs = ndocs
    print 'uploaded: %i docs in: %i seconds for a rate: %f docs/sec' % (ndocs, delta,rate)
    print 'done at', datetime.datetime.now()
  
  logfile.close()
  sys.stdout = sys.__stdout__
  
