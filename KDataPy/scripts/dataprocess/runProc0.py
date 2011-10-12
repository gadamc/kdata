#!/usr/bin/env python

from DBProcess import *
import os, sys, tempfile, shutil, datetime
import rootifySambaData as rt
import scpToSps as scp

def rootifyAndScp(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the file name.'
    sys.exit(-1)
    
  tempDir = tempfile.mkdtemp()
  print 'creating temporary directory', tempDir
  outputFile = os.path.join(tempDir, os.path.basename(args[0]) + '.root') 
  
  #rootify the File into a KData File!
  print 'calling rootification and producing', outputFile
  theReturn = rt.convertfile(args[0], outputFile)

  if theReturn != outputFile:
    print 'something crazy happened with rootification'
    sys.exit(-1)
    
  #now send it via secure copy!
  print 'calling secure copy'
  scpRet = scp.sendBoloData(outputFile)
  
  #clean up
  print 'removing temporary directory', tempDir
  shutil.rmtree(tempDir)
  
  return scpRet
  
  
def main(*argv):
  '''
  argv[0] is the server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  '''
  
  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database
  myProc = DBProcess(argv[0], argv[1], rootifyAndScp)
  
  vr = myProc.view('proc/proc0', reduce=False)
  
  for row in vr:
    doc = myProc.get(row['id'])
    print 'have doc', row['id']
    doc['status'] = 'proc0 in progress'
    myProc.upload(doc)
    procDict = myProc.doprocess(doc['file']) #this step calls rootfiyAndScp
    print 'called process'

    if len(procDict) > 0:
      #add a few more items to the document
      dd = datetime.datetime.utcnow()
      procDict['date'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
      procDict['processname'] = 'rootifyAndCopyToSps'
      
      if len(procDict['scpErrs']) > 0:
        doc['status'] = 'bad'
      else:
        doc['status'] = 'good'
      #this step will add the procDict dictionary to the 
      #database document and then upload it to the DB
      doc['proc0'] = procDict
      myProc.upload(doc)
      
    else:
      print 'the process returned an empty dictionary!'
      sys.exit(-1)
    


if __name__ == '__main__':
  main(*sys.argv[1:])
