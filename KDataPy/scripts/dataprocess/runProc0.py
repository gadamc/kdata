#!/usr/bin/env python
import datetime, os, time
from KDataPy.scripts.dataprocess.ManagedSendToLyon import ManagedSendToLyon
  

def checkForLog(doc):
  try:
    dataPath = os.path.dirname(doc['file'])
    logFilePath = os.path.join(dataPath, os.path.basename(doc['file']).split('_')[0]+'_log')

    if os.path.isfile(logFilePath):
      time.sleep(300) #sleep five minutes just to make sure all the meta data has copied over.... 
      return True
    else:
      return False

  except: 
    print str(datetime.datetime.utcnow()), 'runProc0.checkForLog raised an Exception with doc ID %s.... returning false' % doc['_id']
    return False

def process(*argv):
  '''
  argv[0] is the server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2] is the username to sftp to ccage
  argv[3] is the password for the username
  argv[4] - if set to 'limit', then this will only process one file
  '''
  
  limit = False
  try:
    if argv[4] == 'limit':
      limit = True
  except: pass

  theManager = ManagedSendToLyon(argv[0], argv[1], argv[2], argv[3], 'proc/proc0', 'proc0', limit)
  theManager.testDocFunction = checkForLog  #tell theManager to call this function before it sends any data to Lyon.

  print '\n', str(datetime.datetime.utcnow()), ': starting runProc0.py \n'

  return theManager.sendAvailableDocs()

 