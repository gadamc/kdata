#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runpProc0
import KDataPy.datadb
import json

def run(**kwargs): 

  (sucDocIds, failDocIds) = runProc0.main(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'])

  dbs = KDataPy.datadb.datadb(kwargs['server'], kwargs['database'])

  for docid in sucDocIds:
    resp = dbs.setkey(docid, 'status', 'proc1 queued')
    print json.dumps(resp, indent=1)
    
  runProc1.main(kwargs['server'], kwargs['database'], 'useProc0=false', 'username='+str(kwargs['sftp_username']), \
                 'password='+str(kwargs['sftp_password']),  )
