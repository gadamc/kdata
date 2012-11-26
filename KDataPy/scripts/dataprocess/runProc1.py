#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
import os, sys, tempfile, shutil, datetime, copy, socket
import KDataPy.scripts.dataprocess.rootifySambaData as rt
from KDataPy.exceptions import *
import KDataPy.scripts.dataprocess.sftpToSps as ftp
from KDataPy.uploadutilities import splitargs

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes one argument...the input file name with the full path to the file.'
    sys.exit(-1)

  #
  newFileName = args[0] + '.root'
  outFile = ''
    
  outFile = rt.convertfile(args[0], newFileName)
      
  processdoc = {}
  
  if outFile != '':
    processdoc['file'] = outFile
    
  return processdoc
  
def processOne(doc, **kwargs):
  global myProc
  
  try:
    print 'have doc', doc['_id']
    doc['status'] = 'proc1 in progress'
    myProc.upload(doc)

    #need to pass in the correct file name, depending upon where the process is happening!
    mustSftp = False

    try:
      if kwargs['useProc0'] == True:  #use the location of the file from proc0 -- useful when run from Lyon
        procDict = myProc.doprocess(doc['proc0']['file']) #this step calls runProcess
    except:
      procDict = myProc.doprocess(doc['file']) #this step calls runProcess
      mustSftp = True

    print 'called process'

    #add a few more items to the document
    procDict['date'] = str(datetime.datetime.utcnow())
    procDict['processname'] = 'samba2kdata'
    procDict['hostname'] = socket.gethostname()
    procDict['localuname'] = os.uname()
    
    if doc.has_key('proc1') == False:  doc['proc1'] = {}
    doc['proc1'].update(procDict)    
    
    if procDict.has_key('file'):
      doc['status'] = 'good'
    else:
      raise KDataRootificationError('KDataRootificationError. runProc1.py line51. rootiftySambaData.convertfile returned an empty document.\n')

    if mustSftp:
      try:
        sftpRet = scp.sendBoloData(kwargs['username'], kwargs['password'], procDict['file'])
        doc['proc1']['sftp'] = sftpRet
        doc['proc1']['file'] = sftpRet['file'] #must do this to be consistent with batch processing records
      except Exception as e:
        raise KDataTransferError('KDataTransferError. runProc1.py line 55  \n' + str(type(e)) + ' : ' + str(e))  

    return (doc, True)

    
  except Exception as e:
    theExc = KDataRootificationError('KDataRootificationError. runProc1.py line54  \n' + str(type(e)) + ' : ' + str(e))
    doc['proc1']['exception'] = theExc
    doc['status'] = 'proc1 failed'
    return(doc, False) #don't throw here.... processOne is called by main and we want to save this to the database

      
  
def main(*argv):
  '''
  argv[0] is the couchdb server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2], argv[3], ... are the document ids (you can pass in an unlimited number of ids.)

  This supports passing in kwargs, in the form "keyword=value" without spaces! I don't use python.argparse here because CC in Lyon doesn't support it
  It uses the KData.util.splitargs function

  process 1 - converts a raw Samba data file into Kdata ROOT file.
  This script is meant to be run on ccage.in2p3.fr with access to the /sps/edelweis directory
  because that is where we expect the data files to be located.
  '''
  
  (myargs, mykwargs) = splitargs(argv)
  

  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database... although, its barely useful... 
  global myProc
  myProc = DBProcess(myargs[0], myargs[1], runProcess)
  
  for anId in myargs[2:]:
    doc = myProc.get(anId)
    
    (doc, result) = processOne(doc, **mykwargs)
  
    myProc.upload(doc)


if __name__ == '__main__':
  main(*sys.argv[1:])
