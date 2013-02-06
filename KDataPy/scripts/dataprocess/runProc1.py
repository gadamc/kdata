#!/usr/bin/env python

import os, sys, tempfile, shutil, datetime, copy, socket
import pickle
import signal
import urllib
import json
import argparse
import KDataPy.scripts.dataprocess.rootifySambaData as rt
from KDataPy.exceptions import *
import KDataPy.scripts.dataprocess.sftpToSps as ftp
import KDataPy.datadb
from KDataPy.scripts.dataprocess.DBProcess import *
from couchdbkit import Server

#this function will give you a chance to clean up the database. Currently, its set to 
#handle, in the "main" function below, only the SIGXCPU signal, which is the signal sent 
#by the batch system when the batch job has reached the maximum allowed time to run
caught_sigxcpu = False


def _sigxcpuHandler(sigNum, frame):
  global caught_sigxcpu
  caught_sigxcpu = True
  print 'Caught Signal: ', sigNum
  print 'Preparing to cleanUp'


#in the main processing loop, the cleanUp is called when
#the sigxcpu signal has been caught
def _cleanUp(doclist, servername, dbname):
  dbs = KDataPy.datadb.datadb( servername, dbname)

  for docid in doclist:
    
    reqOpts = {'update':'status', 'value':'good', 'remove':'proc1'}
    resp = dbs._update_inplace(docid,reqOpts)

    print 'resetting database information for', docid
    print json.dumps(resp['results'], indent=1)

def _rootify(*args):
  
  sambaFile = args[0]
  kdataFile = None

  if len(args) > 1:
    kdataFile = args[1]

  if kdataFile == None:
    kdataFile = sambaFile + '.root'
  
  outFile = ''
  print sambaFile, '->', kdataFile
    
  outFile = rt.convertfile(sambaFile, kdataFile)
      
  processdoc = {}
  
  if outFile != '':
    processdoc['file'] = outFile
    
  return processdoc
  
def _processOne(doc, **kwargs):
  global myProc
  print 'starting _processOne', kwargs  
  try:
    print 'have doc', doc['_id']
    doc['status'] = 'proc1 in progress'
    myProc.upload(doc)

    #need to pass in the correct file name, depending upon where the process is happening!

    print '_processOne kwargs:', kwargs

    kdataFile = None
    tempDir = None

    mustSftp = False
    if kwargs.get('ftp', False):
      mustSftp = True
      print 'will sent output ROOT file to SPS via sFTP'

    if kwargs.get('useProc0', False):  #use this keyword arg when processing from Lyon!!! this is coded into batchRunProc1.py
      filePath = doc['proc0']['file']

    else:
      print 'useProc0 set to false'
      filePath = doc['file']
      #if we're going to send by FTP, just create the file in a temporary space
      #so that we can delete it later after its been sent
      if mustSftp:
        tempDir = tempfile.mkdtemp()
        print tempDir
        kdataFile = os.path.join(tempDir, os.path.basename(filePath)+'.root')
      else:
        kdataFile = filePath + '.root'

      print 'will create', kdataFile


    print 'processing file', filePath
    procDict = myProc.doprocess(filePath, kdataFile) #this step calls _rootify
    print 'called rootification'

    #add a few more items to the document
    procDict['date'] = str(datetime.datetime.utcnow())
    procDict['processname'] = 'samba2kdata'
    procDict['hostname'] = socket.gethostname()
    procDict['localuname'] = os.uname()
    
    if doc.has_key('proc1') == False:  doc['proc1'] = {}
    doc['proc1'].update(procDict)    
    print 'process dictionary', procDict

    if procDict.has_key('file'):
      doc['status'] = 'good'
      procDict['file_size'] = os.path.getsize(procDict['file'])
    else:
      raise KDataRootificationError('KDataRootificationError. runProc1.py line102 rootiftySambaData.convertfile returned an empty document.\n')



    if mustSftp:
      try:
        print str(datetime.datetime.utcnow()), 'sending proc1 file', procDict['file']
        sftpRet = ftp.sendBoloData(kwargs['username'], kwargs['password'], procDict['file'])
        os.listdir(tempDir)
        print 'removing temp directory', tempDir
        shutil.rmtree(tempDir)
        if doc.has_key('proc1') == False:  doc['proc1'] = {}
        doc['proc1']['sftp'] = sftpRet
        doc['proc1']['sftp']['local_tempfile'] = kdataFile
        doc['proc1']['file'] = sftpRet['file'] #must do this to be consistent with batch processing records

        print os.path.basename(doc['proc1']['file']), ' successfully sent'
        #print json.dumps(doc['proc1'], indent=1)

      except Exception as e:
        theExc = KDataTransferError('KDataTransferError. runProc1.py line113  \n' + str(type(e)) + ' : ' + str(e))  
        print theExc
        if doc.has_key('proc1') == False:  doc['proc1'] = {}
        doc['proc1']['pickled_exception'] = pickle.dumps(theExc)
        doc['proc1']['str_exception'] = str(theExc)
        doc['status'] = 'proc1 failed'
        print str(datetime.datetime.utcnow()), doc['_id'], 'proc1 failed'
        return(doc, False) #don't throw here.... _processOne is called by main and we want to save this to the database
    
    print str(datetime.datetime.utcnow()), doc['_id'], 'complete'
    return (doc, True)

    
  except Exception as e:
    theExc = KDataRootificationError('KDataRootificationError. runProc1.py line120  \n' + str(type(e)) + ' : ' + str(e))
    print theExc
    if doc.has_key('proc1') == False:  doc['proc1'] = {}
    doc['proc1']['pickled_exception'] = pickle.dumps(theExc)
    doc['proc1']['str_exception'] = str(theExc)
    doc['status'] = 'proc1 failed'
    print str(datetime.datetime.utcnow()), doc['_id'], 'proc1 failed'
    return(doc, False) #don't throw here.... _processOne is called by main and we want to save this to the database

      
  
def process(couchServer, couchDbName, runList, **processKwargs):
  '''
  runList is a list of document _id's for each run that will be processed.

  the following kwargs affect the behavior
  'useProc0' -- if set to True, then this rootifies the data using the samba file found in the location doc['proc0']['file'], where the 'doc' is the CouchDB database document for that run. If set to False, or not set at all, then the file located in doc['file'] is rootified. This is used when the rootification process (proc1) is done "locally" (in production, this means on a computer in Modane -- S7)

  'ftp' -- if set to True, then this process will attempt to send the rootified data file to the ccage.in2p3.fr system. 
  
  If 'ftp' is set to True, you must supply a 'username' and 'password' in the kwargs, which will be used to gain access to ccage.in2p3.fr and transfer the data to /sps/edelweis/kdata/data/raw

  '''
  
  global myProc
  myProc = DBProcess(couchServer, couchDbName, _rootify)

  #create a list of remaining docs that will be dealt with if the job
  #is killed by the SIGXCPU sent by the Lyon CC batch system

  myRemainingDocs = []
  for anId in runList:
    myRemainingDocs.append(anId)

  #the SIGXCPU signal is sent by the Lyon Batch system - set up handler function
  signal.signal( getattr(signal, 'SIGXCPU') ,_sigxcpuHandler)

 
  #start the data processing loop
  for anId in runList:

    if caught_sigxcpu:
      _cleanUp(myRemainingDocs, couchServer, couchDbName)
      break  

    doc = myProc.get(anId)
    (doc, result) = _processOne(doc, **processKwargs)
    myProc.upload(doc)

    myRemainingDocs.remove(anId)

    

if __name__ == '__main__':
  '''
  Uses argparse to split up the command line and call the 'process' function above. See above for details.
  '''

  parser = argparse.ArgumentParser()
  parser.add_argument('serverUrl', help='[required] the name of the couch server including credentials')
  parser.add_argument('dbName', help='[required] the name of the database (datadb)')
  parser.add_argument('-f', '--files', nargs = "+")
  parser.add_argument('-p0', '--useProc0', action='store_true')
  parser.add_argument('-ftp','--ftp', action='store_true')
  parser.add_argument('-u', '--username')
  parser.add_argument('-pw', '--password')
  args = parser.parse_args()

  if args.ftp and (args.username is None or args.password is None):
    print 'Must set username and password if using ftp option'
    sys.exit(0)
  else:
    print args

  kw = {'useProc0':args.useProc0, 'username':args.username, 'password':args.password, 'ftp':args.ftp}

  process(args.serverUrl, args.dbName, args.files, **kw)
