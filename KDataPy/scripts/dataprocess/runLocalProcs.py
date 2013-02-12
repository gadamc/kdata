#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runProc0, runMetaProc0
import KDataPy.datadb
import json, datetime, os, time

def logtime():
  return str(datetime.datetime.utcnow())

def checkForLog(doc):
  try:
    dataPath = os.path.dirname(doc['file'])
    logFilePath = os.path.join(dataPath, doc['run_name']+'_log')

    if os.path.isfile(logFilePath):
      print str(datetime.datetime.utcnow()), 'FOUND LOG file for Samba run %s ... waiting five minutes' % doc['run_name']
      time.sleep(3) #sleep five minutes just to make sure all the meta data has copied over.... 
      return True
    else:
      print str(datetime.datetime.utcnow()), 'NO LOG file for Samba run %s ... this run must still be active' % doc['run_name']
      return False

  except Exception as e:
    print e
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


  if len(sucDocIds) > 0:
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


