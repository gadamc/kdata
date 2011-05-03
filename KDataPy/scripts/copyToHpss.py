#!/usr/bin/env python

'''
  This script checks the local Samba Raw Data event directory (currently the link 'rawdata')
  and copies any new files found in that directory to HPSS. First, this script must tar
  the files before sending the data to HPSS, and it must check that the files are large enough
  in size. 
  
'''
 
import sys, os, glob, tarfile, tempfile, string, time, subprocess, datetime, shlex, shutil, stat
import rootifySambaData as rootify
import scpToSps as scp

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
    
def removeFilesFromSmallList(filename, listoffiles):

  currentlist = list()
  
  '''first open the file to get the current list of small files in the file'''
  if os.path.isfile(filename):
    file = open(filename,'rU')
    for line in file:
      currentlist.append(line.rstrip('\n'))
    file.close()
    
  print 'current of small files list'
  print currentlist
  print 'list of files to remove'
  print listoffiles

  '''open the file to append new files to the list'''
  file = open(filename, 'w')
  for item in currentlist:
    if item not in listoffiles:  #only write out the files that are not in the listoffiles
    #effectively removing them from the small files list.
      print item
      file.write(str(item) + '\n')
      print 'keepting file', item
    else:
      print 'removing', item

      
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
  '''returns the folder size in bytes'''
  folder_size = 0
  for (path, dirs, files) in os.walk(folder):
    for file in files:
      filename = os.path.join(path, file)
      folder_size += os.path.getsize(filename)      
  
  return folder_size


def totalFolderListSize(folderList):
  '''returns the size in bytes for all folders in the list'''
  totalsize = 0
  for item in folderList: 
    totalsize += getfoldersize(item)
  return totalsize
  

def sortListBySize(fileList, min = None, max = None):

  if min == None:
    min = 0
  
  transfer = list()
  small = list()
  big = list()
  
  for item in fileList:
    size = getfoldersize(item)/(1024.0*1024.0)  #size of folder in MB    
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

def hasAlreadyBeenRootified(filename, sambaDirName):
  if os.path.isfile(filename):
    file = open(filename, 'rU')
    for line in file:
      if line.strip() == os.path.basename(sambaDirName):
        file.close()
        return True
  
  file.close()
  return False
  
def addToRootifiedList(filename, sambaDirName):
  if os.path.isfile(filename):
    file = open(filename, 'a')
    file.write(os.path.basename(sambaDirName) + '\n')
    file.close()
    
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
      if line.find('events') == -1:
        lastfiles[ str(os.path.basename(line)[4:5]) ] = os.path.basename(line).strip()
      else:
        lastfiles['events'] = os.path.basename(line).strip()    
    file.close()
  
  return lastfiles
  
  
def addItemToLastFiles(lastfilename, item):
    
    lastFiles = getDictOfLastFiles(lastfilename)
    if item.find('events') == -1:
      lastFiles[os.path.basename(item)[4:5]] = os.path.basename(item).strip()
    else:
      lastFiles['events'] = os.path.basename(item).strip()
      
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
      flogfiletime = os.path.getctime(logfile)
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

def tarTheList(dirlist, tempDir = None):

  if tempDir == None or tempDir == '.':
    filename = os.path.basename(dirlist[len(dirlist)-1])[0:2] + 'events.tar'
  else:
    tempDir.rstrip('/')
    filename = tempDir + '/' + os.path.basename(dirlist[len(dirlist)-1])[0:2] + 'events.tar'
  
  print 'Creating Tar File', filename
  file = tarfile.open(filename, 'w')
  for adir in dirlist:
    print 'Adding to tarfile:', adir
    logfile.flush()
    file.add(adir, arcname = os.path.basename(adir))
    
  file.close()
  
  return filename

def tarTheDir(adir, tempDir = None):
  if tempDir == None or tempDir == '.':
    filename = os.path.basename(adir) + '.tar'
  else:
    tempDir.rstrip('/')
    filename = tempDir + '/' + os.path.basename(adir) + '.tar'
  
  print 'Creating Tar File', filename
  file = tarfile.open(filename, 'w')
  print 'Adding to tarfile:', adir
  file.add(adir, arcname = os.path.basename(adir))
  file.close()
  logfile.flush()
  
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
  p['rootifyOption'] = 'true'
  p['rootifiedList'] = p['workingdir'] + '/rootifiedList.txt'
  
  for i in range(len(arglist)):
  
    if arglist[i] == '-t':
      i += 1
      val = formatvalue(arglist[i])
      if isinstance(val,int) or isinstance(val,float):
        p['timeout_hours'] = val
      else:
        print 'Invalid value for timeout.', val, 'Set to default', p['timeout_hours']
    
    elif arglist[i] == '-r':
      i += 1
      val = formatvalue(arglist[i])
      if val == 'true' or val == 'false':
        p['timeout_hours'] = val
      else:
        print 'Invalid value for rootifyOption.', val, 'Set to default', p['rootifyOption']
        print 'Valid options are "true" or "false" '
    
    else:
      pass
      #we don't know this option
      #print 'The option', arglist[i], 'is unknown. Exiting.'
      #sys.exit(-1)

  if checkArguments(p):
    return p
  else:
    return False
    
def getmonthnamedict():
  return {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}

def getmonthnumberdict():
  return {'a':1, 'b':2, 'c':3, 'd':4, 'e':5, 'f':6, 'g':7, 'h':8, 'i':9, 'j':10, 'k':11, 'l':12}

def getyeardict():
  return {'i':2008, 'j':2009, 'k':2010, 'l':2011, 'm':2012, 'n':2013, 'o':2014, 'p':2015, 'q':2016}

def getmonthdir(sambaDirName):
  return getmonthnamedict()[sambaDirName[1]]  #assume, of course, standard samba file structure...

def getyeardir(sambaDirName):
  return getyeardict()[sambaDirName[0]]  #assume, of course, standard samba file structure...


def formatSrbYearMonthDirStructure(sambaDirName):
  basename = os.path.basename(sambaDirName)
  return str(getyeardir(basename)) + '/' + getmonthdir(basename) + '{0:02d}'.format((getyeardir(basename) - 2000)) + '/events'

def getSmallFilesToTransfer(smallfile):

  if os.path.isfile(smallfile):
    file = open(smallfile)
    thedate = datetime.date.today()
    
    smalllist = list()
    months = getmonthnumberdict()
    years = getyeardict()
    
    for line in file:
      smfiledate = os.path.basename(line).strip()[:2]
      smfileyear = getyeardict()[smfiledate[0]]
      smfilemonth = getmonthnumberdict()[smfiledate[1]]
      
      if smfileyear < thedate.year or smfilemonth < thedate.month:
        smalllist.append(line.strip())
  
  return smalllist
  



def runSubProcess(command):
  args = shlex.split(command)
  print command
  logfile.flush()
  return subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def fileExistsOnSrb(spath, afile):
  sbproc = runSubProcess(spath + '/Sls ' + afile)
  return sbproc.stdout.readline() != ''
  
def printLinesInFile(afile):
    for line in afile:
      print line
      
def uploadToHpss(theparams, tarfile, overwrite = False): #option to overwrite a file on SRB

  spath = theparams['srbpath'].rstrip('/')
  global logfile
  
  theReturn = True
  
  sinit = runSubProcess(spath +'/Sinit')
  sinit.wait()
  printLinesInFile(sinit.stderr)
  
  hpssdir = theparams['srbdestination'] + '/' + formatSrbYearMonthDirStructure(tarfile.rstrip('.tar'))
    
  smkdir = runSubProcess(spath + '/Smkdir -p ' + hpssdir)
  smkdir.wait()
  printLinesInFile(smkdir.stderr)
    
  scd = runSubProcess(spath +'/Scd ' + hpssdir)
  scd.wait()
  printLinesInFile(scd.stderr)
    
  putprocess = ''
  setToCurrentTransfer(theparams['currentTransferFile'], os.path.basename(tarfile).rstrip('.tar'))
 
  if overwrite==True:
    putprocess = spath + '/Sput -fMvK -r ' + tarfile
  else: 
    if fileExistsOnSrb(spath, hpssdir + '/' + os.path.basename(tarfile))==False:
      putprocess = spath + '/Sput -fMvK -r ' + tarfile
          
    elif overwrite == 'Update':
        targetfile = str.replace(os.path.basename(tarfile), '.tar','_2.tar')
        putprocess = spath + '/Sput -fMvK -r ' + tarfile + ' ' + targetfile
        print os.path.basename(tarfile), 'already exists on HPSS. Update option uploads file to', targetfile
    else:
      print os.path.basename(tarfile), 'already exists on HPSS. No update'
      putprocess = 'no transfer'
      theReturn = True  #return true to indicate a successful transfer
      
  if putprocess != '' and putprocess != 'no transfer':
    sput = runSubProcess(putprocess)
    sput.wait() 
    printLinesInFile(sput.stderr)
    addItemToLastFiles(theparams['lastcopyfile'], tarfile.rstrip('.tar'))
  
  elif putprocess == '':
    print 'No Put Process. You must have given a value to overwrite other than True, False or "Update" '
    print 'Quitting!!!'
    logfile.flush()
    sys.exit(-1)
  
  
  setToCurrentTransfer(theparams['currentTransferFile'], '')
  #the setToLastCopyFile should make sure to output a last-file for each 'mac' 
  
  sexit  = runSubProcess(spath + '/Sexit')
  printLinesInFile(sexit.stderr)
  logfile.flush()
  
  return theReturn

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
  #appendSmallRunListFile(params['smallrunlist'], smalllist)
  
  print 'Tarring files'
  tempDir = tempfile.mkdtemp()
  print 'in directory', tempDir
  logfile.flush()
  try:
    tarlist = list()
    for item in transferlist:
      tarfile = tarTheDir(item, tempDir)
      print ''
      print 'Uploading to Hpss', tarfile
      logfile.flush()
      if uploadToHpss(params, tarfile):
        tarlist.append(tarfile)
                
 
    print 'Searching for small files to transfer'
    transferlist = getSmallFilesToTransfer(params['smallrunlist'])
    print 'Current list of small files'
    print transferlist
    logfile.flush()
    #now check that all of these files combined are less than 200 MB
    if len(transferlist) > 0:
      totalSize = totalFolderListSize(transferlist)/(1024.0*1024.0)
      if totalSize > params['minfilesize']:
        tarfile = tarTheList(transferlist, tempDir)
        print ''
        print 'Uploading to Hpss', tarfile
        logfile.flush()
        if uploadToHpss(params, tarfile):
          removeFilesFromSmallList(params['smallrunlist'], transferlist)
          tarlist.append(tarfile)  
      else:
        print 'Small File Size is too small:', totalSize, ' < ', params['minfilesize']
      
    else:
      print 'No Small Files'
      logfile.flush()
      
      

    if params['rootifyOption'] == 'true':
        tempRootDir = tempfile.mkdtemp()
        print 'rootification directory', tempRootDir
        os.chmod(tempRootDir, stat.S_IRWXO + stat.S_IRWXG + stat.S_IRWXU)
        for item in newDirs:        
          
          if hasAlreadyBeenRootified(params['rootifiedList'],item)==False:
          
            print 'rootifying directory', item
            logfile.flush()
            allfiles = rootify.convertdir(item, tempRootDir)
            logfile.flush()
            
            for file in allfiles:
                print 'scp', file
                scp.sendBoloData(file)
                #notifyDbOfTransaction(file)
                logfile.flush()
            
            addToRootifiedList(params['rootifiedList'],item)

        print 'Deleting temporary Directory', tempRootDir
        shutil.rmtree(tempRootDir)

    
  except:
    print 'Deleting temporary Directory', tempDir
    shutil.rmtree(tempDir)
    print sys.exc_info()[0], sys.exc_info()[1]
    raise sys.exc_info()[0]
     
  print 'Deleting temporary Directory', tempDir
  shutil.rmtree(tempDir)
  return len(tarlist)

#______________________________
if __name__ == '__main__':

  start = time.time()
  params = readArguments(sys.argv)
  if params == False:
    print 'Something is wrong with the parameters'
    sys.exit(-99)
  
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
  
