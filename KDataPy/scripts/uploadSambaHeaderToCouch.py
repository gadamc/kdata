#!/usr/bin/env python

from couchdbkit import *
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, glob
import datetime, json


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

def readheader(file):
  
  header = {}
  header['author'] = 'Samba'
  header['content'] = 'Single Samba File Run Header'
  header['type'] = 'samba run header'
  dd = datetime.datetime.utcnow()
  header['date_filed'] = [dd.year,dd.month,dd.day,dd.hour,dd.minute,dd.second,dd.microsecond,0]
  header['_id'] = os.path.basename(file.name) + '_samba_runheader'
  header['run_name'] = os.path.basename(file.name)
  
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
      return header
    elif line == '':
      return False  #we shouldn't reach the end of the file already
    elif line.startswith('#'):  #skip lines that are comments
      pass #skip to the next line
    else:
      line = line.rstrip() #remove the trailing \n characters
      
      if line.count('=') > 0: #make sure its a key = value line
      
        list = line.split('=') 
        if len(list) >= 2:
          key = list[0].strip()#.lower().replace('.','_').replace('-','_')
          vlist = list[1].split('#')
          value = formatvalue(vlist[0].strip())
          #print repr(key) + ' : ' + repr(value)
          header[str(key)] = value
  
  return header
      
def readboloheader(file):

  header = {}
  header['author'] = 'Samba'
  header['content'] = 'Single Samba File Bolo Config Header'
  dd = datetime.datetime.utcnow()
  header['date_filed'] = [dd.year,dd.month,dd.day,dd.hour,dd.minute,dd.second,dd.microsecond,0]
  header['type'] = 'samba bolo config header'
  runname = os.path.basename(file.name)
  runsplit = runname.split('_')
  header['run_name'] = formatvalue(runsplit[0])
  header['file_number'] = runsplit[1]
  header['full_run_name_number'] = runname
  
  firstline = file.readline()
  if firstline.strip().startswith('* Detecteur'):
    list = firstline.split()
    detector = list[2]
    #print 'detector : ' + detector
    header['detector'] = detector
    header['_id'] = os.path.basename(file.name) + '_samba_boloconfiguration_' + detector 
  else:
    return firstline  #this doesn't appear to be a Bolometer Configuration section
    
    
  while True:
  
    line = file.readline()
    
    if line.rstrip().endswith('* ----------'):
      return header
    elif line == '':
      return False  #we shouldn't reach the end of the file already
    elif line.startswith('#'):  #skip lines that are comments
      pass #skip to the next line
    else:
      line = line.rstrip() #remove the trailing \n characters
      
      if line.count('=') > 0: #make sure its a key = value line
      
        list = line.split('=') 
        if len(list) >= 2:
        
            if list[0].strip() == 'Bolo.reglages': #handle the special case
              rootkey = list[0].strip()#.lower().replace('.','_').replace('-','_')
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
              key = list[0].strip()#lower().replace('.','_').replace('-','_')
              vlist = list[1].split('#')
              value = formatvalue(vlist[0].strip())
              header[str(key)] = value
  
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
  print db.info()
  
  file = open(filename)
  docs = list()
  
  sambaheader = readheader(file)
  
  try:
    if isinstance(sambaheader,dict):
    
      docexists = False
      if db.doc_exist(sambaheader.get('_id')):
        sambaheader['_rev'] = db.get_rev(sambaheader.get('_id'))
        docexists = True
      
      if override==True:
        docs.append(sambaheader)
      elif docexists == False:
        docs.append(sambaheader)
      
      docs = upload(db, docs)
  
  except KeyError:
      print 'Hey, something is wrong with the code. A KeyError was thrown looking for the _id in the samba run header'
      sys.exit(1)
      
  #now, loop through and read the bolometer header files
  while True:
    boloheader = readboloheader(file)
    if isinstance(boloheader,dict):
      #print boloheader._doc
      try:
        docexists = False
        if db.doc_exist(boloheader.get('_id')):
          boloheader['_rev'] = db.get_rev(boloheader.get('_id'))
          docexists = True
      
        if override==True:
          docs.append(boloheader)
        elif docexists == False:
          docs.append(boloheader)
      
      except KeyError:
        print 'Hey, something is wrong with the code. A KeyError was thrown looking for the _id in the bolometer configuration header'
        sys.exit(1)
    
    else:
      #print 'Not a Document, apparently. We are done.'
      break
    
      
  docs = upload(db,docs)

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

if __name__ == '__main__':
 
  uri = sys.argv[2]
  dbname = sys.argv[3]
  
  if len(sys.argv)>=5:
    if sys.argv[4]=='True':
      override = True   
    else:
      override = False
  else:
    override = False
  
  #check to see if argv[1] is a directory name or a file name
  if os.path.isfile(sys.argv[1]):
    uploadFile(sys.argv[1], uri, dbname, override)
  elif os.path.isdir(sys.argv[1]):
    if uploadFromRunDir(sys.argv[1], uri, dbname, override)==False:
      #try if this is a top-level samba directory rather than a single run directory
      uploadFromTopLevelDir(sys.argv[1], uri, dbname, override)
      
      
