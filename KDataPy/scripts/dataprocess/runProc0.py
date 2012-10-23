#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
from KDataPy.exceptions import *
import os, sys, tempfile, shutil, datetime
import KDataPy.scripts.dataprocess.sftpToSps as scp

def scpToLyon(*args, **kwargs):
  
  if len(args) > 3:
    print 'takes three args: username, password and filename.'
    sys.exit(-1)
    
  #now send it via secure copy!
  print 'calling secure copy', args[0], args[1], args[2]
  
  try:
    scpRet = scp.sendBoloData(args[0], args[1], args[2])
  except Exception as e:
    raise KDataTransfer('KDataTransfer. runProc0.py line 20  ' + str(type(e)) + ' : ' + str(e) + '\n')

  return scpRet
  
  
def main(*argv):
  '''
  argv[0] is the server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2] is the username to sftp to ccage
  argv[3] is the password for the username
  argv[4] - if set to 'limit', then this will only process one file
  '''
  
  print '\n', str(datetime.datetime.now()), ': starting runProc0.py \n'

  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database
  myProc = DBProcess(argv[0], argv[1], scpToLyon)
  
  try:
    if argv[4] == 'limit': 
      vr = myProc.view('proc/proc0', reduce=False, limit=1)
  except:
    vr = myProc.view('proc/proc0', reduce=False)
  
  for row in vr:
    print row['id']
    
    try:
      doc = myProc.get(row['id'])
      print 'have doc', row['id']
      doc['status'] = 'proc0 in progress'
      myProc.upload(doc)

      procDict = myProc.doprocess(argv[2], argv[3], doc['file']) #this step calls scpToLyon
      print 'called process'

      if len(procDict) > 0:
        #add a few more items to the document
        procDict['date'] = str(datetime.datetime.utcnow())
        procDict['processname'] = 'copySambaFileToSps'
      
        #if len(procDict['scpErrs']) > 0:
        #  doc['status'] = 'proc0 failed'
        #else:
        
        doc['status'] = 'good'
        #this step will add the procDict dictionary to the 
        #database document and then upload it to the DB
        if doc.has_key('proc0') == False:
          doc['proc0'] = {}
        
        doc['proc0'].update(procDict)
        myProc.upload(doc)
      
      else:
        doc['status'] = 'proc0 failed'
        myProc.upload(doc)
        print 'the process returned an empty dictionary!'
        sys.exit(-1)

    except Exception as e:
      print e
      if doc.has_key('proc0') == False:
          doc['proc0'] = {}
        
      doc['proc0']['exception'] = str(type(e)) + ': ' + str(e)
      doc['status'] = 'proc0 failed'
      if doc.has_key('proc0'): del doc['proc0']
      myProc.upload(doc)
      sys.exit(-1)


if __name__ == '__main__':
  main(*sys.argv[1:])
