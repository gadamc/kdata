#!/usr/bin/env python
# -*- coding: utf-8 -*-


from couchdbkit import *
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, glob
import datetime, json

class Header(Document):
  author = StringProperty()
  content = StringProperty()
  datefiled = StringProperty()
  
class BolometerConfig(Document):
    author = StringProperty()
    content = StringProperty()
    datefiled = StringProperty()

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
    
    return value.strip('"\'') #strip off any quotations found in the value
  else:
    return value

def readheader(file):
  
  header = Header( #json document to be updloaded to database
    author = 'SAMBA',
    content = 'Single SAMBA File Run Header',
    datefiled = str(datetime.datetime.utcnow())
  )
    
  header['_id'] = os.path.basename(file.name) + '_samba_runheader'
  
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
          key = list[0]
          vlist = list[1].split('#')
          value = formatvalue(vlist[0].strip())
          #print repr(key) + ' : ' + repr(value)
          header[key] = value
  
  return header
      
def readboloheader(file):

  header = BolometerConfig( #json document to be updloaded to database
    author = 'SAMBA',
    content = 'Single SAMBA File Bolometer Configuration Header',
    datefiled = str(datetime.datetime.utcnow())
  )
  
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
              rootkey = list[0].strip()
              #print 'root key ' + rootkey
              
              rootval = dict()
              while True:
                nline = file.readline()
                
                if nline.strip().startswith(')') or nline.rstrip().endswith('* ----------'):
                  #print 'found end of bolo.reglages'
                  break
                
                if nline.lstrip().startswith('{'):
                
                  valuelist = nline.strip().split(':=')
                  key = valuelist[0].strip(' {')
                  value = valuelist[1].strip(' },')
                  rootval[key] = formatvalue(value)
              
              v = json.JSONEncoder().encode(rootval)
              header[rootkey] = v
              
            else:
              key = list[0]
              vlist = list[1].split('#')
              value = formatvalue(vlist[0].strip())
              header[key] = value
  
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
  sambaheader.set_db(db)
     
  if override==True:
    docs.append(sambaheader)
    if db.doc_exist(sambaheader.__getattr__('_id')):
      db.delete_doc(sambaheader.__getattr__('_id'))
      
  elif db.doc_exist(sambaheader.__getattr__('_id')) == False:
    docs.append(sambaheader)
      
  docs = upload(db, docs)
  
  #now, loop through and read the bolometer header files
  while True:
    boloheader = readboloheader(file)
    try:
      if boloheader._doc:
        #print boloheader._doc
        boloheader.set_db(db)

        if override==True:
          docs.append(boloheader)
          if db.doc_exist(boloheader.__getattr__('_id')):
            db.delete_doc(boloheader.__getattr__('_id'))
            
        elif db.doc_exist(boloheader.__getattr__("_id"))==False:
          docs.append(boloheader)
        
    except:
      #print 'Not a Document, apparently. We are done.'
      break
    
      
  docs = upload(db,docs)

def uploadFromRunDir(dirname, uri, dbname, override=None):
  if os.path.isdir(dirname)==False:
    return False
  
  searchstring = dirname.rstrip('/') + '/' + os.path.basename(dirname.rstrip('/')) + '_[0-9][0-9][0-9]'
  print 'Searching for ' + searchstring
  filelist = glob.glob(searchstring) #search for files of this formate

  for i in filelist:
    print 'Uploading ' + i
    uploadFile(i, uri, dbname, override)
    
  return True
  
def uploatFromTopLevelDir(dirname, uri, dbname, override=None):
  if os.path.isdir(dirname)==False:
    return False
  
  filelist = glob.glob(dirname.rstrip('/') + '/[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]') #search for directories of the SAMBA format

  for i in filelist:
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
      uploatFromTopLevelDir(dirname, uri, dbname, override)
      
      
