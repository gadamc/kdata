#!/usr/bin/env python

from couchdbkit import *
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, glob, copy
import datetime, json, re, time
import socket
from KDataPy.exceptions import *
from KDataPy.uploadutilities import formatvalue
import KDataPy.samba_utilities as sut

_localRunDict = {}


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

    
def readrunheader(file):

  line = ''
  
  global _localRunDict
  
  while True:
  
    line = file.readline().rstrip()
    #need to check if we got to the end of the file, otherwise this will probably 
    #run forever.
    if line == '':
      print 'found end of file prematurely while readin the samba run header.'
      sys.exit(-1)
      
    if line == '# ===== Entete de run =====':
      print 'Found start of run. Creating Run Document'
      _localRunDict['_id'] = 'run_' + os.path.basename(file.name) + '_kdatascript'
      break
  
  #found the start of the run header, now read the lines
  #until the data is found
  
  while True:
    line = file.readline().rstrip()
    if line.startswith('#'):
      pass
    elif line == '* Donnees':
      print 'Finished Run Header', _localRunDict['_id']
      return _localRunDict
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
           
          _localRunDict[key] = value
    
 #if all goes well, will return _localRunDict.
 #otherwise, if an error occurs, should return False


def readsambafileheader(file):
  
  header = {}
    
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
      print 'Finished reading Samba File Header'
      return header
    elif line == '':
      print 'prematurely found the end of the file while reading the file header'
      sys.exit(-1)  #we shouldn't reach the end of the file already
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
  sys.exit(-1)
      
def readboloheader(file):

  header = {}

  firstline = file.readline()
  if firstline.strip().startswith('* Detecteur'):
    alist = firstline.split()
    detector = alist[2]

    header['detector'] = detector
    header['bolometer'] = getBolometerName(detector)
  else:
    return firstline  #this doesn't appear to be a Bolometer Configuration section
    
    
  while True:
  
    line = file.readline()
    
    if line.rstrip().endswith('* ----------'):
      print 'Finished Bolo Header' 
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
  sys.exit(-1)


def readchannelheader(file, voie):
  '''Due to the structure of the Samba Header, this function returns the a 2-tuple,
  with the first element being the header dictionary and the second element is the
  last line read, which should have the channel name of the next channel in the header
  Also, the channel must be provided by reading the lines in the file
  If the call to this function occurs after the readboloheader function, 
  then the line should be ready to be parse to determine the voie
  '''
  
  header = {}
  
  header['Voie'] = voie
  header['bolometer'] = getBolometerName(voie)
  
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
          print 'Finished reading Channel header (end of filter * Voie)'
          return (header, line, True)
        
        elif line.find('* Run') != -1:
          #print 'End of header. Found Run at end of filter', line[line.find('* Run'):].rstrip()
          filter = line[:line.find('* Run')]
          #header['Filtre'] = filter
          print 'Finished reading Channel header (end of filter * Run)'
          return (header, line, False)
        else:  #we must have found a filter that appears to have an end-of-line byte in the filter
          #print 'Found premature end of line in filter'
          line = file.readline()
        
    if line.startswith('* Voie'):
      #print 'End of header. Found Next Voie at start of line', line[line.find('* Voie'):].rstrip()
      print 'Finished reading Channel header (* Voie)'
      return (header, line, True)

    if line.startswith('* Run'):
      #print 'End of header. Found Run at start of line', line[line.find('* Run'):].rstrip()
      print 'Finished reading Channel header (* Run)'
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


def uploadFile(filename, uri, dbname, overWrite = False):

  if sut.isvalidsambadatafilename(os.path.basename(filename)) is False:
    return False
  
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  print db.info()
  
  #read the run header
  file = open(filename)
  
  global _localRunDict
  _localRunDict = {}
  
  runheader = readrunheader(file)
  

  _localRunDict['author'] = 'Samba'
  _localRunDict['content'] = 'Samba DAQ document for a particular run. Use this database entry to track the progress of the processing of this data'
  _localRunDict['type'] = 'daqdocument'

  _localRunDict['date_uploaded'] = time.time()
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  _localRunDict['file'] = file.name
  _localRunDict['run_name'] = formatvalue(runsplit[0])
  _localRunDict['file_number'] = int(runsplit[1])
 

  file.close()  #close and then reopen the file, just to make it easy to get to the start
  # of the run.
  
  #read the samba file header
  file = open(filename)
  sambaheader = readsambafileheader(file)
  
  #now add the key/values to the _localRunDict document
  for k, v in sambaheader.items():
    if _localRunDict.has_key(k) is False:
      _localRunDict[k] = v
      
  #now, loop through and read the bolometer header files
  
  boloArray = list()
  lastline = ''
  
  while True:
    boloheader = readboloheader(file)
    
    if isinstance(boloheader,dict):
      boloArray.append(boloheader)
       
    elif isinstance(boloheader, str):
      lastline = copy.copy(boloheader)
      print 'Not a Dictionary. We are done reading the bolometer headers.'
      break 
      
    else:
      print 'Not a Dictionary. We are done reading the bolometer headers.'
      break    
  
  _localRunDict['Detecteurs'] = boloArray
  
  
  #now read through the channel configuration values  
  # the while loop above quits when the the readboloheader doesn't return
  # a dictionary. Instead, it returns the next line in the header, which
  # should be the start of the channel configurations. (assuming that Samba
  # doesn't change its format
  #
  
  channelArray = list()
  
  voiepart = lastline[lastline.find('* Voie'):]
  channelName = voiepart[voiepart.find('"'):].strip('":\n')
  while True:
    chanheaderoutput = readchannelheader(file, channelName)
    channelheader = chanheaderoutput[0]
    voiepart = chanheaderoutput[1][chanheaderoutput[1].find('* Voie'):]
    channelName = voiepart[voiepart.find('"'):].strip('":\n')
    
    if isinstance(channelheader,dict):
      channelArray.append(channelheader)
 
    else:
      print 'Read Channel Header didn\'t return a dictionary.'
      
    if chanheaderoutput[2] == False:
      print 'Channel Header output False - Done reading Channel Headers.' # okay, this tells us that we're done
      break
  
  _localRunDict['Voies'] = channelArray
  
  _localRunDict['status'] = 'closed'
  _localRunDict['hostipaddress'] = socket.gethostbyname( socket.gethostname() )
  _localRunDict['hostname'] = socket.gethostname()
  #_localRunDict['size_in_bytes'] = os.path.getsize(_localRunDict['file'])

  #don't allow this script to rewrite a doc to the database!
  #if you want to do that, then delete the doc you want to recreate
  if db.doc_exist(_localRunDict['_id']) and overWrite is False:
    file.close()
    return False
  else:
    _localRunDict = ['_rev'] = db.get_rev(_localRunDict['_id'])

  db.save_doc(_localRunDict)
  
  file.close()
  return True
  

def uploadMetaFile(filename, uri, dbname):

  if sut.isvalidsambametadatafilename(os.path.basename(filename)) is False:
    return False
  
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  print db.info()
  
  #with the meta file docs, I don't have to read anytihng inside of the doc!
  doc = {}
  doc['_id'] = os.path.basename(filename)
  doc['author'] = 'Samba'
  doc['type'] = 'daqmetadatadocuments'
  doc['date_uploaded'] = time.time()
  doc['content'] = 'Meta Data DB Document for a particular Samba Run'
  doc['file'] = filename
  doc['run_name'] = filename.split('_')[0]
  
  doc['status'] = 'closed'
  doc['hostipaddress'] = socket.gethostbyname( socket.gethostname() )
  doc['hostname'] = socket.gethostname()
  doc['file_size'] = os.path.getsize(doc['original_file'])

  db.save_doc(doc)
  
  return True


def uploadFromRunDir(dirname, uri, dbname):
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
    uploadFile(i, uri, dbname)
    
  return True
  
def uploadFromTopLevelDir(dirname, uri, dbname):
  if os.path.isdir(dirname)==False:
    return False
  
  dirlist = glob.glob(dirname.rstrip('/') + '/[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]') #search for directories of the SAMBA format

  if len(dirlist) == 0:
    return False
    
  for i in dirlist:
    if os.path.isdir(i):
      uploadFromRunDir(i, uri, dbname)
    
  return True
  

  
