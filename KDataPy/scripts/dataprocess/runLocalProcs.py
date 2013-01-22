#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runProc0
import KDataPy.datadb
import json, datetime

def logtime():
  return str(datetime.datetime.now())

def run(**kwargs): 
  print logtime(), 'running proc0'
  (sucDocIds, failDocIds) = runProc0.main(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'])
  print logtime(), 'found', len(sucDocIds), 'successful docs and', len(failDocIds), 'failed docs'

  dbs = KDataPy.datadb.datadb(kwargs['server'], kwargs['database'])

  for docid in sucDocIds:
    resp = dbs.setkey(docid, 'status', 'proc1 queued')
    resp['time'] = logtime()
    print json.dumps(resp, indent=1)
    
  print logtime(), 'running proc1 locally wiht useProc0=false'
  runProc1.main(kwargs['server'], kwargs['database'], 'useProc0=false', 'username='+str(kwargs['sftp_username']), \
                 'password='+str(kwargs['sftp_password']),  ' '.join(sucDocIds))
