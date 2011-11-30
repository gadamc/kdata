#!/usr/bin/env python

from DBProcess import *
import os, sys, tempfile, shutil, datetime
import rootifySambaData as rt

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the database document.'
    sys.exit(-1)

  #
  newFileName = args[0]['proc0']['file'] + '.root'
  outFile = rt.convertfile(args[0]['proc0']['file'], newfileName)
  processdoc = {}
  
  if outFile != '':
    processdoc['file'] = outFile
  
  return processdoc
  
def processOne(doc):
  global myProc
  
  print 'have doc', doc['_id']
  doc['status'] = 'proc1 in progress'
  myProc.upload(doc)
  procDict = myProc.doprocess(doc) #this step calls runProcess
  print 'called process'

  #add a few more items to the document
  dd = datetime.datetime.utcnow()
  procDict['date'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
  procDict['processname'] = 'calculateNoise'
  
  #this step will add the procDict dictionary to the 
  #database document and then upload it to the DB
  doc['proc1'] = procDict
  
  if procDict.has_key('file'):
    return (doc, True)
  else:
    print 'the process returned an empty dictionary!'
    doc['status'] = 'proc1 failed'
    return (doc, False)
    
def setupProc(server, database, function):
  global myProc
  myProc = DBProcess(server, database, function)
  return myProc
  
def main(*argv):
  '''
  argv[0] is the couchdb server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2] is either 'all' or 'single'

  process 1 - converts the raw Samba data files into Kdata ROOT files.
  This script is meant to be run on ccage.in2p3.fr with access to the /sps/edelweis directory
  because that is where we expect the data files to be located.
  '''
  
  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database
  global myProc
  myProc = setupProc(argv[0], argv[1], runProcess)
  
  optAll = 'all'
  if(len(argv) > 2): 
    if(argv[2] == 'all' or argv[2] == 'single'):
      optAll = argv[2]
    else:
      print 'invalid argv[2] option'
      sys.exit(-1)

  if(optAll == 'all'):
    vr = myProc.view('proc/proc1', reduce=False)
  elif(optAll == 'single'):
    vr = myProc.view('proc/proc1', reduce=False, limit = 1)
  else:
    print 'how did i get here?'
    sys.exit(-1)

  for row in vr:
    doc = myProc.get(row['id'])
    (doc, result) = processOne(doc)
    myProc.upload(doc)


if __name__ == '__main__':
  main(*sys.argv[1:])
