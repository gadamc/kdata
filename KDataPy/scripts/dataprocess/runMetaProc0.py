#!/usr/bin/env python
import datetime
from KDataPy.scripts.dataprocess.ManagedSendToLyon import ManagedSendToLyon
  

def process(server, database, username, password, limit=False, callback=None):
  '''
  if 'limit' set to true, then this will only process one file
  the callback is the function set to ManagedSendToLyon.testDocFunction
  '''
  
  theManager = ManagedSendToLyon(server, database, username, password, 'proc/metaproc0', 'metaproc0', limit)

  theManager.testDocFunction = callback  #tell theManager to call this function before it sends any data to Lyon.

  print '\n', str(datetime.datetime.utcnow()), ': starting runMetaProc0.py \n'

  return theManager.sendAvailableDocs()

 