#!/usr/bin/env python

from DBProcess import *
import os, sys, tempfile, shutil, datetime, copy, socket
import rootifySambaData as rt

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the database document.'
    sys.exit(-1)

  #
  newFileName = args[0]['proc0']['file'] + '.root'
  outFile = ''
  exc = {}
  try:
    outFile = rt.convertfile(args[0]['proc0']['file'], newFileName)
  except Exception as theExcep:
    outFile = ''
    print theExcep
    exc['print'] = str(theExcep)
    exc['type'] = str(type(theExcep))
    #exc['args'] = theExcep.args
    #exc['message'] = theExcep.message
    print 'done reading exception'
    
  processdoc = {}
  
  if outFile != '':
    processdoc['file'] = outFile
  elif exc.has_key['print']:
    processdoc['exception'] = copy.deepcopy(exc)
    
  
  return processdoc
  
def processOne(doc):
  global myProc
  
  print 'have doc', doc['_id']
  doc['status'] = 'proc1 in progress'
  myProc.upload(doc)
  procDict = myProc.doprocess(doc) #this step calls runProcess
  print 'called process'

  #add a few more items to the document
  procDict['date'] = str(datetime.datetime.utcnow())
  procDict['processname'] = 'samba2kdata'
  procDict['hostname'] = socket.gethostname()
  procDict['localuname'] = os.uname()
  
  #this step will add the procDict dictionary to the 
  #database document and then upload it to the DB
  if doc.has_key('proc1') == False:
    doc['proc1'] = {}
      
  doc['proc1'].update(procDict)
  
  if procDict.has_key('file'):
    doc['status'] = 'good'
    return (doc, True)
  else:
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
  argv[2] is the document id

  process 1 - converts a raw Samba data file into Kdata ROOT file.
  This script is meant to be run on ccage.in2p3.fr with access to the /sps/edelweis directory
  because that is where we expect the data files to be located.
  '''
  
  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database... although, its barely useful... 
  global myProc
  myProc = setupProc(argv[0], argv[1], runProcess)
  doc = myProc.get(argv[2])

  (doc, result) = processOne(doc)
  myProc.upload(doc)


if __name__ == '__main__':
  main(*sys.argv[1:])
