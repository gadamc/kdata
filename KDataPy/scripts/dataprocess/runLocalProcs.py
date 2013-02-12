#!/usr/bin/env python
from KDataPy.scripts.dataprocess import runProc1, runProc0, runMetaProc0
import KDataPy.datadb
import KDataPy.database
import json, datetime, os, time

mycouchDb = None

def logtime():
  return str(datetime.datetime.utcnow())

def checkForLog(doc):
  '''
  this function is not used at the moment...
  
  '''
  try:
    
    if mycouchDb.doc_exists( doc['run_name']+'_log' ):
      print str(datetime.datetime.utcnow()), 'FOUND LOG file for Samba run %s found on database... ' % doc['run_name']
      return True
    else:
      print str(datetime.datetime.utcnow()), 'NO LOG file for Samba run %s found on database ... this run must still be active' % doc['run_name']
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

  global mycouchDb
  mycouchDb = KDataPy.database.datadb(kwargs['server'])

  print logtime(), 'running proc0'
  (sucDocIds, failDocIds) = runProc0.process(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'])
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
  (sucDocIds, failDocIds) = runMetaProc0.process(kwargs['server'], kwargs['database'], kwargs['sftp_username'], kwargs['sftp_password'])
  print logtime(), 'found', len(sucDocIds), 'successful docs and', len(failDocIds), 'failed docs'


