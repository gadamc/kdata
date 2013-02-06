#!/usr/bin/env python
import datetime
from KDataPy.scripts.dataprocess.ManagedSendToLyon import ManagedSendToLyon
  
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

  theManager = ManagedSendToLyon(argv[0], argv[1], argv[2], argv[3], 'proc/metaproc0', 'metaproc0', limit)

  print '\n', str(datetime.datetime.utcnow()), ': starting runMetaProc0.py \n'

  return theManager.sendAvailableDocs()

 