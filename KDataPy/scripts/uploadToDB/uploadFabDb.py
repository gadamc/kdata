#!/usr/bin/env python
# -*- coding: utf-8 -*-

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
import time, sys, subprocess, math, os, datetime, string, copy

#----------
# taken from the Python website in order to handle unicode, which is need in order to properly
# deal with different kinds of characters
import csv, codecs, cStringIO
      
# #connect to the db
global theServer
global db
 
def UnicodeDictReader(str_data, encoding, **kwargs):
    csv_reader = csv.DictReader(str_data, **kwargs)
    # Decode the keys once
    keymap = dict((k, k.decode(encoding)) for k in csv_reader.fieldnames)
    
    #for row in csv_reader:
    #  for k, v in row.iteritems():
    #    print str_data.name,'key',k,'value',v
    
    for row in csv_reader:
        yield dict((keymap[k], v.decode(encoding)) for k, v in row.iteritems())
  
#_______________
def formatvalue(value):
  if (isinstance(value, basestring)):  #SUPPORT UNICODE
    # #see if this string is really an int or a float
    if value.isdigit()==True: #int
      return int(value)
    else: #try a float
      try:
        if math.isnan(float(value))==False:
          return float(value)
      except:
        return value

    return value.strip('" ') #strip off any quotations and extra spaces found in the value
  else:
    return value
    
    
#______________
def parseDoc(doc):
    for k,v in doc.items():
        doc[k] = formatvalue(v)
                
    return doc

#______________
def parseLineToArray(line, token):
  ml = line.split(token)
  for i in range(len(ml)):
    ml[i] =  ml[i].strip() #strip off any extra spaces
     
  return ml

#______________
def parseKeyIntoArray(doc, key, token):
  if doc.has_key(key):
    doc[key] = parseLineToArray(doc[key], token)

        
#______________
def readDetectorFile( thefile ):
  docs = dict()
  #reader = UnicodeReader(thefile)
  reader = UnicodeDictReader(thefile, 'utf-8')
  
  for rawdoc in reader:
    #print rawdoc
    doc = parseDoc(rawdoc)
    doc['_id'] = 'detector_%s' % string.replace(doc['Detector reference'], ' ', '_')
    doc['detector'] = doc['Detector reference']
    del doc['Detector reference']
    print doc['_id']
    doc['type'] = u'detector_config'
    doc['author'] = u'Xavier Navick'
    doc['location'] = doc['Where is it']
    del doc['Where is it']
    
    parseKeyIntoArray(doc, 'Who assembled it', '/')
    parseKeyIntoArray(doc, 'Problems', '/')
        
    docs[doc['detector']] = doc

  return docs
#_____________
def addCrystalFile(crysfile, docs):
  
  #reader = UnicodeReader(crysfile)
  reader = UnicodeDictReader(crysfile, 'utf-8')
  
  for rawdoc in reader:
    doc = parseDoc(rawdoc)
    detector = doc['Crystal ref']
    del doc['Crystal ref']
  
    if doc.has_key(detector): 
      docs[detector]['crystal'] = doc
    
#______________
def addGluedNtdFile(gnfile, docs):
  
  #reader = UnicodeReader(gnfile)
  reader = UnicodeDictReader(gnfile, 'utf-8', dialect = 'excel')
  
  for rawdoc in reader:
    doc = parseDoc(rawdoc)
    detector = doc['Detector reference']
    del doc['Detector reference']
    
    ntds = list() #make it a list now so that it can be expanded in the future
    
    #reformat some of the fields
    ntddoc = copy.deepcopy(doc)
    
    ntddoc['ntd'] = doc['NTD glued']
    ntddoc['ntd_id'] = 'ntd_%s' % string.replace(ntddoc['ntd'], ' ', '_')  #i can't use a validation document to ensure that ntd_id actually exists. 
    #but i could put it in here… 
    
    del ntddoc['NTD glued']
    del ntddoc['NTD unglued']
    
    ntddoc['date_on'] = ntddoc['NTD glueing day']
    del ntddoc['NTD glueing day']
    
    if ntddoc['NTD unglue day'] != '':
      ntddoc['date_off'] = ntddoc['NTD unglue day']
    del ntddoc['NTD unglue day']
    
    parseKeyIntoArray(ntddoc, 'Who glued it', '/')
    parseKeyIntoArray(ntddoc, 'Who unglued it', '/')
    parseKeyIntoArray(ntddoc, 'Problems', '/')
    
    ntds.append(ntddoc)
    docs[detector]['ntd'] = ntds
  
  
#______________
def addRunFile(runFile, docs):
  #reader = UnicodeReader(runFile)
  reader = UnicodeDictReader(runFile, 'utf-8', dialect = 'excel')
  
  for rawdoc in reader:
    rundoc = parseDoc(rawdoc)
    detector = rundoc['Name Detector']
    
    runs = list()
    
    listOfRuns = ['Run 14 Position','Run 13 Position','Run 12 Position','Run 11 Position','Run 10 Position','Run 9 Position','Run 8 Position']
    
    for runPos in listOfRuns:
      if rundoc[runPos] != '':
        #from the T-RUN.txt file it looks like these are always formated like T10/e
        runinfo = dict()
        runinfo['tower'] = formatvalue(rundoc[runPos].split('/')[0][1:])
        runinfo['level'] = formatvalue(rundoc[runPos].split('/')[1])
        runinfo['Run'] = formatvalue(runPos.split(' ')[1])
        runs.append(runinfo)
        
    docs[detector]['CryoPosition'] = runs
    
  
#______________
def readNtdFile(ntdfile):
  docs = dict()
  #reader = UnicodeReader(ntdfile)
  reader = UnicodeDictReader(ntdfile, 'utf-8', dialect = 'excel')
  
  for rawdoc in reader:
    doc = parseDoc(rawdoc)
    doc['_id'] = 'ntd_%s' % string.replace(doc['Nom NTD'], ' ' , '_')
    doc['ntd'] = doc['Nom NTD']
    del doc['Nom NTD']
    doc['type'] = u'ntd_config'
    doc['author'] = u'Xavier Navick'
    
    #change the comments to an array
    comments = list()
    comments.append(doc['Comments'])
    doc['Comments'] = comments
    
    del doc['Unglueing date']
    del doc['2nd glueing on']
    del doc['2nd unglueing date']
    del doc['3rd glueing on']
    
    parseKeyIntoArray(doc, 'Who assembled it', '/')
        
    docs[doc['ntd']] = doc

  return docs

#______________
def uploadData(fname, uri, dbname):
     
  print 'Upload contents of %s to %s/%s' % (fname, uri, dbname)
  
  
  crystalfile = open(os.path.join(dir, 'O-Crystals.txt'), 'r')   
  assemblyfile = open(os.path.join(dir, 'O-Detectors assembling.txt'), 'r')
  holderfile = open(os.path.join(dir, 'O-Holders.txt'), 'r')   
  ntdfile = open(os.path.join(dir, 'O-NTD.txt'), 'r') 
  capsfile = open(os.path.join(dir, 'O-Caps.txt'), 'r')
  runconditionsfile = open(os.path.join(dir, 'R-RUN conditions.txt'), 'r')                                                             
  runinfofile = open(os.path.join(dir, 'R-Run info.txt'), 'r') 
  glueactions = open(os.path.join(dir, 'A-NTD Gluing choice and characteristics.txt'), 'r')
  lsmactions = open(os.path.join(dir, 'A-Actions LSM.txt'), 'r')
  capaction = open(os.path.join(dir, 'A-Caps fixing.txt'), 'r')
  copperfile = open(os.path.join(dir, 'M-Copper.txt'), 'r')
  kaptonfile = open(os.path.join(dir, 'M-Kapton.txt'), 'r')
  teflonfile = open(os.path.join(dir, 'M-Teflon.txt'), 'r')
                                      
  uploadCrystalFile(crystalfile)
  uploadAssemblyFile(assemplyfile)
  uploadholderfile(holderfile)
  uploadNtdFile(ntdfile)
  uploadCapsFile(capsfile)
  uploadRunConditions(runconditionsfile)
  uploadRunInfo(runinfofile)
  uploadGlueActions(glueactions)
  uploadLsmActions(lsmactions)
  uploadCapsActions(capaction)
  uploadCopper(copperfile)
  uploadKapton(kaptonfile)
  uploadTeflon(teflonfile)
  
  detectordocs = createDetectorDocs()
  
  addCrystalFile(crystalfile, detectordocs)
  addGluedNtdFile(gluedntdfile, detectordocs)
  addRunFile(runfile, detectordocs)
  
  ntddocs = readNtdFile(ntdfile)
  #used for bulk uploading
  
  #convert detectordocs dictionary into a list
  docs = list()
  for key, val in detectordocs.iteritems():
    #print key, val['detector']
    docs.append(val)
    
  for key, val in ntddocs.iteritems():
    #print key, val['ntd']
    docs.append(val)

  start = time.time()  
  #print len(docs)
  #shit… do i need to check for doc revisions? probably. 
  for doc in docs:
    print doc['_id']
    if db.doc_exist(doc['_id']):
      doc['_rev'] = db.get_rev(doc['_id'])
    
  print 'bulk save'
  
  db.bulk_save(docs)
  
  #print summary statistics  
  delta = time.time() - start
  rate = float(len(docs))/float(delta)
  print 'uploaded: %i docs in: %i seconds for a rate: %f docs/sec' % (len(docs), delta, rate)
  
  
#______________
if __name__=='__main__':
  dir = sys.argv[1]
  uri = sys.argv[2]
<<<<<<< HEAD
  dbname = 'fabdb' 
  # #connect to the db
  global theServer
  global db 
  # #connect to the db
  theServer = Server(uri)
  db = theServer[dbname]
  print uri, dbname
   
=======
  
  dbname = 'fabdb'
>>>>>>> 361126b0de0cfb74ce86854aa84e76c9f11af625
  if len(sys.argv) >= 4:
    dbname = sys.argv[3]
  
  uploadData(dir, uri, dbname)
  


