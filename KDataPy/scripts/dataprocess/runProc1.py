#!/usr/bin/env python

from DBProcess import *
import os, sys, tempfile, shutil, datetime
import rootifySambaData as rt
import scpToSps as scp
import calculateAveNoise as can

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the file name.'
    sys.exit(-1)

  return can.main(args[0]['proc0']['file'])
  
  
def processOne(doc):
  global myProc
  
  print 'have doc', row['id']
  doc['status'] = 'proc1 in progress'
  myProc.upload(doc)
  procDict = myProc.doprocess(doc) #this step calls runProcess
  print 'called process'

  if len(procDict) > 0:
    #add a few more items to the document
    dd = datetime.datetime.utcnow()
    procDict['date'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
    procDict['processname'] = 'calculateNoise'
    
    if len(procDict['scpErrs']) > 0:
      doc['status'] = 'bad'
    else:
      doc['status'] = 'good'
    #this step will add the procDict dictionary to the 
    #database document and then upload it to the DB
    doc['proc1'] = procDict
    
    return (doc, True)
    
  else:
    print 'the process returned an empty dictionary!'
    return ({}, False)
    
def setupProc(server, database, function):
  global myProc
  myProc = DBProcess(server, database, function)
  return myProc
  
def main(*argv):
  '''
  argv[0] is the server (http://127.0.0.1:5984)
  argv[1] is the database (edwdb)
  '''
  
  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database
  global myProc
  myProc = setupProc(argv[0], argv[1], runProcess)
  
  vr = myProc.view('proc/proc1', reduce=False)
  
  for row in vr:
    doc = myProc.get(row['id'])
    (doc, result) = processOne(doc)
    if result:
      myProc.upload(doc)


if __name__ == '__main__':
  main(*sys.argv[1:])
