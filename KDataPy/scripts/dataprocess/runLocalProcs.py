#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runProc0, runMetaProc0
import KDataPy.datadb
import json, datetime

def logtime():
  return str(datetime.datetime.utcnow())

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
    print str(datetime.datetime.utcnow()), 'runLocalProcs.checkForLog raised an Exception with doc ID %s.... returning false' % doc['_id']
    return False


def run(**kwargs): 
  '''
  kwargs are
  server:  couchDB server

  database: name of database

  ftp: True or False

  sftp_username: username for ccage.in2p3.fr
  sftp_password: password
  '''

  
  print logtime(), 'running proc0'
  (sucDocIds, failDocIds) = runProc0.process(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'], callback = checkForLog)
  print logtime(), 'found', len(sucDocIds), 'successful docs and', len(failDocIds), 'failed docs'


  print logtime(), 'running proc1'
  print logtime(), 'proc1 setting queue status'  
  dbs = KDataPy.datadb.datadb(kwargs['server'], kwargs['database'])

  for docid in sucDocIds:
    resp = dbs.setkey(docid, 'status', 'proc1 queued')
    resp['time'] = logtime()
    print docid
    print json.dumps(resp, indent=1)

  docstringlist = ' '.join(sucDocIds)
  print docstringlist

  print logtime(), 'runProc1.process called locally with useProc0=false'
  rp1kwargs = {'useProc0':False, 'username':kwargs['sftp_username'], 'password':kwargs['sftp_password'], 'ftp':kwargs['ftp']}

  runProc1.process(kwargs['server'], kwargs['database'], sucDocIds, **rp1kwargs)


  #run meta docs last...

  print logtime(), 'running metaproc0'
  (sucDocIds, failDocIds) = runMetaProc0.process(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'], callback = checkForLog)
  print logtime(), 'found', len(sucDocIds), 'successful docs and', len(failDocIds), 'failed docs'


