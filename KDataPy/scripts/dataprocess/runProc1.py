#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
import os, sys, tempfile, shutil, datetime, copy, socket
import KDataPy.scripts.dataprocess.rootifySambaData as rt
from KDataPy.exceptions import *
import KDataPy.scripts.dataprocess.sftpToSps as ftp
from KDataPy.uploadutilities import splitargs
import KDataPy.datadb
import pickle
import signal
import urllib


#this function will give you a chance to clean up the database. Currently, its set to 
#handle, in the "main" function below, only the SIGXCPU signal, which is the signal sent 
#by the batch system when the batch job has reached the maximum allowed time to run
caught_sigxcpu = False

def sigxcpuHandler(sigNum, frame):
  global caught_sigxcpu
  caught_sigxcpu = True
  print 'Caught Signal: ', sigNum
  print 'Preparing to cleanUp'

def genericSignalHandler(sigNum, frame):
  print 'Caught Signal from System: ', sigNum

#in the main processing loop, the cleanUp is called when
#the sigxcpu signal has been caught
def cleanUp(doclist):

  dbs = KDataPy.datadb.datadb( sys.argv[1], sys.argv[2])

  for docid in doclist:
    
    reqOpts = {'update':'status', 'value':'good', 'remove':'proc1'}
    resp = dbs._update_inplace(docid,reqOpts)

    print 'resetting database information for', docid
    print json.dumps(resp['results'], indent=1)


def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes one argument...the input file name with the full path to the file.'
    sys.exit(-1)

  #
  newFileName = args[0] + '.root'
  outFile = ''
  print args[0], newFileName
    
  outFile = rt.convertfile(args[0], newFileName)
      
  processdoc = {}
  
  if outFile != '':
    processdoc['file'] = outFile
    
  return processdoc
  
def processOne(doc, **kwargs):
  global myProc
  print 'starting processOne', kwargs  
  try:
    print 'have doc', doc['_id']
    doc['status'] = 'proc1 in progress'
    myProc.upload(doc)

    #need to pass in the correct file name, depending upon where the process is happening!
    print 'processOne kwargs:', kwargs

    if kwargs.has_key('useProc0'):  #use this keyword arg when processing from Lyon!!! this is coded into batchRunProc1.py
      mustSftp = False
      filePath = doc['proc0']['file']
    else:
      filePath = doc['file']
      mustSftp = True


    print 'processing file', filePath
    procDict = myProc.doprocess(filePath) #this step calls runProcess
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
    else:
      raise KDataRootificationError('KDataRootificationError. runProc1.py line61 rootiftySambaData.convertfile returned an empty document.\n')

    print 'must sftp?', mustSftp

    if mustSftp:
      try:
        sftpRet = ftp.sendBoloData(kwargs['username'], kwargs['password'], procDict['file'])
        if doc.has_key('proc1') == False:  doc['proc1'] = {}
        doc['proc1']['sftp'] = sftpRet
        doc['proc1']['file'] = sftpRet['file'] #must do this to be consistent with batch processing records
      except Exception as e:
        raise KDataTransferError('KDataTransferError. runProc1.py line70  \n' + str(type(e)) + ' : ' + str(e))  

      #need to delete local .root file!
    return (doc, True)

    
  except Exception as e:
    theExc = KDataRootificationError('KDataRootificationError. runProc1.py line76  \n' + str(type(e)) + ' : ' + str(e))
    print theExc
    if doc.has_key('proc1') == False:  doc['proc1'] = {}
    doc['proc1']['pickled_exception'] = pickle.dumps(theExc)
    doc['proc1']['str_exception'] = str(theExc)
    doc['status'] = 'proc1 failed'
    return(doc, False) #don't throw here.... processOne is called by main and we want to save this to the database

      
  
def main(*argv):
  '''
  argv[0] is the couchdb server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2... n] are possible kwargs
  argv[n], argv[n+1], ... are the document ids (you can pass in an unlimited number of ids.)

  This supports passing in kwargs, in the form "keyword=value" without spaces! I don't use python.argparse here because CC in Lyon doesn't support it
  It uses the KData.util.splitargs function

  process 1 - converts a raw Samba data file into Kdata ROOT file.
  This script is meant to be run on ccage.in2p3.fr with access to the /sps/edelweis directory
  because that is where we expect the data files to be located.
  '''
  
  (myargs, mykwargs) = splitargs(argv)
  
  print 'my args', myargs
  print ' and kwargs', mykwargs

  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database... although, its barely useful... 
  global myProc
  myProc = DBProcess(myargs[0], myargs[1], runProcess)


  #create a list of remaining docs
  #these will be dealt with if the job
  #is killed by the SIGXCPU sent by 
  #the Lyon CC batch system
  myRemainingDocs = []
  for anId in myargs[2:]:
    myRemainingDocs.append(anId)

  signal.signal( getattr(signal, 'SIGXCPU') ,sigxcpuHandler)

  for i in [x for x in dir(signal) if x.startswith("SIG") and x != 'SIGXCPU']:
    try:
      signum = getattr(signal,i)
      signal.signal(signum,genericSignalHandler)
      print 'registering the signal handler for', signum, i
    except RuntimeError as m:
      print "RTE: Skipping %s"%i
    except ValueError as e:
      print "VE: Skipping %s"%i

  #start the data processing loop
  for anId in myargs[2:]:

    if caught_sigxcpu:
      cleanUp(myRemainingDocs)
      break  

    doc = myProc.get(anId)
    (doc, result) = processOne(doc, **mykwargs)
    myProc.upload(doc)

    myRemainingDocs.remove(anId)

    

if __name__ == '__main__':
  main(*sys.argv[1:])
