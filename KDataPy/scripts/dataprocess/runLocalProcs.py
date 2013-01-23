#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runProc0
import KDataPy.datadb
import json, datetime

def logtime():
  return str(datetime.datetime.now())

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
  (sucDocIds, failDocIds) = runProc0.process(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'])
  print logtime(), 'found', len(sucDocIds), 'successful docs and', len(failDocIds), 'failed docs'

  dbs = KDataPy.datadb.datadb(kwargs['server'], kwargs['database'])

  for docid in sucDocIds:
    resp = dbs.setkey(docid, 'status', 'proc1 queued')
    resp['time'] = logtime()
    print docid
    print json.dumps(resp, indent=1)

  docstringlist = ' '.join(sucDocIds)
  print docstringlist
  print logtime(), 'running proc1 locally with useProc0=false'
  
  rp1kwargs = {'useProc0':False, 'username':kwargs['sftp_username'], 'password':kwargs['sftp_password'], 'ftp':kwargs['ftp']}

  runProc1.process(kwargs['server'], kwargs['database'], sucDocIds, **rp1kwargs)
