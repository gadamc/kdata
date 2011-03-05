#!/usr/bin/env python

from couchdbkit import *
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, glob
import datetime, json, re

runDict = {}

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


def getBolometerName(astring):
  '''this will search for ID or FID detector names in astring and return 
  that string if it is found. otherwise if it doesn't find an ID/FID 
  name, it will attempt to strip off any 'centre', 'garde', or 'chaleur'
  parts of astring and return what it thinks is the detector name. if either
  of those attempts fail, it will simply return astring'''

  
  match = re.search('[F]{0,1}ID[0-9]{1,3}', astring)
  if match != None:
    #some strings appear to have a Ch at the end instead of the 
    #expected AB, CD or GH ending. Must first check for this possibility
    
    if re.search('AB|CD|GH', astring[match.end():]) != None:
      return re.findall('[F]{0,1}ID[0-9]{1,3}', astring)[0]
    else:
      return astring[match.start():]
      
  match = re.search('centre',astring)
  if match != None:
    return astring[match.end():].strip('_ ')
    
  match = re.search('garde',astring)
  if match != None:
    return astring[match.end():].strip('_ ')
    
  match = re.search('chaleur',astring)
  if match != None:
    return astring[match.end():].strip('_ ')
    
    
  return astring
def onIgnoreList(key):
  ignoreList = ['file_number',
                'date_uploaded',
                'addenda',
                '_id',
                '_rev'
                ]
                
  return key in ignoreList 
    
  
def appendSambaDocument(db, doc):

  if isinstance(doc, dict) == False:
    return [doc]

  if db.doc_exist(doc.get('_id')):
    olddoc = db.get(doc.get('_id'))
  else:
    return [doc]
    
  newdoc = dict()  #making a new document that just has the changed values
  
  docChanged = False
  for key, value in doc.items():
    if key in olddoc and (key == '_id' or key == '_rev')==False:
      if onIgnoreList(key) == False:
        if olddoc[key] != value:
          docChanged = True
          print 'Found change.', key, value, olddoc[key]
          newdoc[key] = value
                
            
    elif (key == '_id' or key == '_rev')==False:
      docChanged = True
      print 'New Key Found!!!', key
      newdoc[key] = value  #hmmmm do i want to do this? I can't imagine this will EVER get called. Samba format will not change in the middle of a Run!
      #unless there is a crash!
      
  if docChanged:
    newdoc['_id'] = doc['_id'] + '_addendum_' + str(doc['file_number'])
    newdoc['file_number'] = doc['file_number']
    newdoc['type'] ='addendum'
    newdoc['parent_id'] = doc['_id']
    
    newdocinfo = dict()
    newdocinfo['id'] = newdoc['_id']
    newdocinfo['file_number'] = newdoc['file_number']
    olddoc['addenda'].append(newdocinfo)
    print 'Adding addenda'
    print newdoc
    
    if db.doc_exist(newdoc.get('_id')):
      newdoc['_rev'] = db.get_rev(newdoc.get('_id'))
    
    return [olddoc,newdoc]
  else:
    return []
  
def readrunheader(file):

  line = ''
  
  global runDict
  
  while True:
  
    line = file.readline().rstrip()
    if line == '# ===== Entete de run =====':
      print 'Found start of run. Creating Run Document'
      runDict['_id'] = os.path.basename(file.name).split('_')[0] + '_samba_runheader'
      break
  
  #found the start of the run header, now read the lines
  #until the data is found
  runDict['author'] = 'Samba'
  runDict['content'] = 'Samba Run Header'
  runDict['type'] = 'samba run header'
  dd = datetime.datetime.utcnow()
  runDict['date_uploaded'] = {'year':dd.year,'month':dd.month,'day':dd.day,
                          'hour':dd.hour,'minute':dd.minute,'second':dd.second,
                          'microsecond':dd.microsecond,'timezone':0}  
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  runDict['run_name'] = formatvalue(runsplit[0])
  runDict['file_number'] = int(runsplit[1])
  runDict['addenda'] = list()
  
  while True:
    line = file.readline().rstrip()
    if line.startswith('#'):
      pass
    elif line == '* Donnees':
      print 'Finished Run Header', runDict['_id']
      return runDict
    elif line == '':
      print 'Prematurely found an empty line while reading the Run Header'
      sys.exit(-1)  #we shouldn't reach the end of the file already
    else:
      if line.count('=') > 0:
        alist = line.split('=') 
        if len(alist) >= 2:
          key = alist[0].strip()#.lower().replace('.','_').replace('-','_')
          vlist = alist[1].split('#')
          value = formatvalue(vlist[0].strip())
           
          runDict[key] = value
    
 #if all goes well, will return runDict.
 #otherwise, if an error occurs, should return False


def readsambafileheader(file):
  
  header = {}
  header['author'] = 'Samba'
  header['content'] = 'Samba Single Partition File Header'
  header['type'] = 'samba file header'
  dd = datetime.datetime.utcnow()
  header['date_uploaded'] = {'year':dd.year,'month':dd.month,'day':dd.day,
                          'hour':dd.hour,'minute':dd.minute,'second':dd.second,
                          'microsecond':dd.microsecond,'timezone':0}  
  header['_id'] = os.path.basename(file.name).split('_')[0] + '_samba_fileheader'
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  header['run_name'] = formatvalue(runsplit[0])
  header['file_number'] = int(runsplit[1])
  header['Run.Date.secondes'] = runDict['Date.secondes']
  header['Run.Date.microsecs'] = runDict['Date.microsecs']
  header['Run.Header.id'] = runDict['_id']
  header['addenda'] = list()
  
  firstline = file.readline()
  if firstline.rstrip() == '* Archive SAMBA':
    firstline = file.readline() #skip the next line
    if firstline.rstrip().startswith('* Version ') == False:
      return False  #force the file to hold this formatting. if not, then we'll deal with it later
  else:
    return False  #this doesn't appear to be a SAMBA data file
    
    
  while True:
  
    line = file.readline()
    
    if line.rstrip().endswith('* ----------'):
      print 'Finished reading Samba File Header', header['_id']
      return header
    elif line == '':
      return False  #we shouldn't reach the end of the file already
    elif line.startswith('#'):  #skip lines that are comments
      pass #skip to the next line
    else:
      line = line.rstrip() #remove the trailing \n characters
      
      if line.count('=') > 0: #make sure its a key = value line
      
        alist = line.split('=') 
        if len(alist) >= 2:
          key = alist[0].strip()#.lower().replace('.','_').replace('-','_')
          vlist = alist[1].split('#')
          value = formatvalue(vlist[0].strip())
          #print repr(key) + ' : ' + repr(value)
          if key == 'Date':
            date = value.split('/')
            value = {'year':int(date[0]) + 2000, 'month':int(date[1]), 'day':int(date[2])}  
           
          header[key] = value
          
  print 'Reading Samba Partition Header. We ended in a weird state.'
  return header
      
def readboloheader(file):

  header = {}
  header['author'] = 'Samba'
  header['content'] = 'Single Samba File Detector Header'
  dd = datetime.datetime.utcnow()
  header['date_uploaded'] = {'year':dd.year,'month':dd.month,'day':dd.day,
                          'hour':dd.hour,'minute':dd.minute,'second':dd.second,
                          'microsecond':dd.microsecond,'timezone':0}
  header['type'] = 'samba detector header'
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  header['run_name'] = formatvalue(runsplit[0])
  header['file_number'] = int(runsplit[1])
  header['Run.Date.secondes'] = runDict['Date.secondes']
  header['Run.Date.microsecs'] = runDict['Date.microsecs']
  header['Run.Header.id'] = runDict['_id']
  header['addenda'] = list()


  firstline = file.readline()
  if firstline.strip().startswith('* Detecteur'):
    alist = firstline.split()
    detector = alist[2]

    header['detector'] = detector
    header['bolometer'] = getBolometerName(detector)
    header['_id'] = os.path.basename(file.name).split('_')[0] + '_samba_detectorconfiguration_' + detector 
  else:
    return firstline  #this doesn't appear to be a Bolometer Configuration section
    
    
  while True:
  
    line = file.readline()
    
    if line.rstrip().endswith('* ----------'):
      print 'Finished Bolo Header', header['_id'] 
      return header
    elif line == '':
      print 'Hey! Reached end of file when reading the samba detector header!'
      return False  #we shouldn't reach the end of the file already
    elif line.startswith('#'):  #skip lines that are comments
      pass #skip to the next line
    else:
      line = line.rstrip() #remove the trailing \n characters
      
      if line.count('=') > 0: #make sure its a key = value line
      
        alist = line.split('=') 
        if len(alist) >= 2:
        
            if alist[0].strip() == 'Bolo.reglages' and alist[1].strip() != '()': #handle the special case
              rootkey = alist[0].strip()#.lower().replace('.','_').replace('-','_')
              #print 'root key ' + rootkey
              
              rootval = dict()
              while True:
                nline = file.readline()
                
                if nline.strip().startswith(')') or nline.rstrip().endswith('* ----------'):
                  #print 'found end of bolo.reglages'
                  break
                
                if nline.lstrip().startswith('{'):
                
                  valuelist = nline.strip().split(':=')
                  key = valuelist[0].strip(' {').strip()#.lower().replace('.','_').replace('-','_')
                  value = valuelist[1].strip(' },').strip()
                  rootval[str(key)] = formatvalue(value)
              
              header[str(rootkey)] = rootval
              
            else:
              key = alist[0].strip()#lower().replace('.','_').replace('-','_')
              vlist = alist[1].split('#')
              value = formatvalue(vlist[0].strip())
              header[str(key)] = value
  print 'Reading Bolometer Header. We ended in a weird state.'
  return header


def readchannelheader(file, voie):
  '''Due to the structure of the Samba Header, this function returns the a 2-tuple,
  with the first element being the header dictionary and the second element is the
  last line read, which should have the channel name of the next channel in the header
  Also, the channel must be provided by reading the lines in the file
  If the call to this function occurs after the readboloheader function, 
  then the line should be ready to be parse to determine the voie
  '''
  
  header = {}
  header['author'] = 'Samba'
  header['content'] = 'Single Samba File Voie Config Header'
  dd = datetime.datetime.utcnow()
  header['date_uploaded'] = {'year':dd.year,'month':dd.month,'day':dd.day,
                          'hour':dd.hour,'minute':dd.minute,'second':dd.second,
                          'microsecond':dd.microsecond,'timezone':0}
  header['type'] = 'samba voie header'
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  header['run_name'] = formatvalue(runsplit[0])
  header['file_number'] = int(runsplit[1])
  header['Run.Date.secondes'] = runDict['Date.secondes']
  header['Run.Date.microsecs'] = runDict['Date.microsecs']
  header['Run.Header.id'] = runDict['_id']
  header['addenda'] = list()

  header['Voie'] = voie
  header['bolometer'] = getBolometerName(voie)
  header['_id'] = os.path.basename(file.name).split('_')[0] + '_samba_voieconfiguration_' + voie.split(' ')[0] + '_' + voie.split(' ')[1]
        
  while True:
  
    line = file.readline().rstrip()
    
    if line == '* Filtre:':
      line = file.readline()
      #print 'Found the Filter. This should be the end of', header['Voie'], '\'s header'
      while True:
        if line.find('* Voie') != -1:
          #print 'Next channel header found', line[line.find('* Voie'):].rstrip()
          filter = line[:line.find('* Voie')]
          #header['Filtre'] = filter
          print 'Finished reading Channel header (end of filter * Voie)', header['_id']
          return (header, line, True)
        
        elif line.find('* Run') != -1:
          #print 'End of header. Found Run at end of filter', line[line.find('* Run'):].rstrip()
          filter = line[:line.find('* Run')]
          #header['Filtre'] = filter
          print 'Finished reading Channel header (end of filter * Run)', header['_id']
          return (header, line, False)
        else:  #we must have found a filter that appears to have an end-of-line byte in the filter
          #print 'Found premature end of line in filter'
          line = file.readline()
        
    if line.startswith('* Voie'):
      #print 'End of header. Found Next Voie at start of line', line[line.find('* Voie'):].rstrip()
      print 'Finished reading Channel header (* Voie)', header['_id']
      return (header, line, True)

    if line.startswith('* Run'):
      #print 'End of header. Found Run at start of line', line[line.find('* Run'):].rstrip()
      print 'Finished reading Channel header (* Run)', header['_id']
      return (header, line, False)
      
    elif line == '':
      print 'Hey! Reached end of file when reading the channel header!?!'
      return ('notDict', line, False)  #we shouldn't reach the end of the file already
    elif line.startswith('#'):
      pass #skip lines that are comments

    else:
            
      if line.count('=') > 0: #make sure its a key = value line
      
        alist = line.split('=') 
        if len(alist) >= 2:
          key = alist[0].strip()#lower().replace('.','_').replace('-','_')
          vlist = alist[1].split('#')
          value = formatvalue(vlist[0].strip())
          header[str(key)] = value
  
  print 'I hope I never get here... a weird place to be in the Samba readChanneHeader'
  return header


#_____________
# upload
def upload(db, docs):
    #print 'upload:\t%i' % n
    try:
      db.bulk_save(docs)
      del docs
      return list()
    except BulkSaveError as e:
      print e.errors
      print 'couchdbkit.BulkSaveError.'
      print 'Will not delete the docs list and return to continue adding to docs'
      return docs


def uploadFile(filename, uri, dbname, override=None):
  if override == None:
    override = False
    
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  #print db.info()
  
  runDict.clear()
  
  
  #read the run header
  file = open(filename)
  docs = list()
  runheader = readrunheader(file)
  
  if isinstance(runheader,dict):
    runheaders = appendSambaDocument(db, runheader)
    
    for header in runheaders:
        if isinstance(header,dict):
          docs.append(header)
          #docs = upload(db, docs)
  
  else:
    print 'Hey - We didn\'t find the Run Header. Returned header', runheader
    
  file.close()  #close and then reopen the file, just to make it easy to get to the start
  # of the run.
  
  #read the samba file header
  file = open(filename)
  sambaheader = readsambafileheader(file)
  if isinstance(sambaheader,dict):
    sambaheaders = appendSambaDocument(db, sambaheader)
    for header in sambaheaders:
        if isinstance(header,dict):
          docs.append(header)
          #docs = upload(db, docs)
          
  else:
    print 'Hey - We didn\'t find the Samba Header. Returned header', sambaheader
    
        
  #now, loop through and read the bolometer header files
  
  while True:
    boloheader = readboloheader(file)
    
    if isinstance(boloheader,dict):
      boloheaders = appendSambaDocument(db, boloheader)
      for header in boloheaders:
        if isinstance(header,dict):
          docs.append(header)
          #docs = upload(db, docs)
          
    else:
      print 'Not a Dictionary. We are done reading the bolometer headers.'
      break    
  
  #docs = upload(db,docs)
  
  #now read through the channel configuration values  
  # the while loop above quits when the the readboloheader doesn't return
  # a dictionary. Instead, it returns the next line in the header, which
  # should be the start of the channel configurations. (assuming that Samba
  # doesn't change its format
  #
  
  voiepart = boloheader[boloheader.find('* Voie'):]
  channelName = voiepart[voiepart.find('"'):].strip('":\n')
  while True:
    chanheaderoutput = readchannelheader(file, channelName)
    channelheader = chanheaderoutput[0]
    voiepart = chanheaderoutput[1][chanheaderoutput[1].find('* Voie'):]
    channelName = voiepart[voiepart.find('"'):].strip('":\n')
    
    if isinstance(channelheader,dict):
      channelheaders = appendSambaDocument(db, channelheader)
      for header in channelheaders:
        if isinstance(header,dict):
          docs.append(header)
          #docs = upload(db, docs)
    else:
      print 'Read Channel Header didn\'t return a dictionary.'
      
    if chanheaderoutput[2] == False:
      print 'Channel Header output False - Done reading Channel Headers.' # okay, this tells us that we're done
      break
      
  docs = upload(db,docs)
  
  return True
  

def uploadFromRunDir(dirname, uri, dbname, override=None):
  if os.path.isdir(dirname)==False:
    return False

  if dirname == '.':
    searchstring = os.path.basename(os.getcwd()) + '_[0-9][0-9][0-9]'
  else:
    searchstring = dirname.rstrip('/') + '/' + os.path.basename(dirname.rstrip('/')) + '_[0-9][0-9][0-9]'
  
  print 'Searching for ' + searchstring
  filelist = glob.glob(searchstring) #search for files of this formate

  if len(filelist) == 0:
    return False
    
  for i in filelist:
    print 'Uploading ' + i
    uploadFile(i, uri, dbname, override)
    
  return True
  
def uploadFromTopLevelDir(dirname, uri, dbname, override=None):
  if os.path.isdir(dirname)==False:
    return False
  
  dirlist = glob.glob(dirname.rstrip('/') + '/[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]') #search for directories of the SAMBA format

  if len(dirlist) == 0:
    return False
    
  for i in dirlist:
    if os.path.isdir(i):
      uploadFromRunDir(i, uri, dbname, override)
    
  return True
  
  
def main(*args):
  uri = args[1]
  dbname = args[2]
  
  if len(args)>=4:
    if args[3]=='True':
      override = True   
    else:
      override = False
  else:
    override = False
  
  #check to see if argv[1] is a directory name or a file name
  if os.path.isfile(args[0]):
    uploadFile(args[0], uri, dbname, override)
  elif os.path.isdir(args[0]):
    if uploadFromRunDir(args[0], uri, dbname, override)==False:
      #try if this is a top-level samba directory rather than a single run directory
      uploadFromTopLevelDir(args[0], uri, dbname, override)
      
      

if __name__ == '__main__':
  main(*sys.argv[1:])

