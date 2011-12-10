#!/usr/bin/env python

from DBProcess import *
import os, sys, tempfile, shutil, datetime, copy

import ROOT

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the database document.'
    #sys.exit(-1)

  #
  newFileName = args[0]['proc1']['file'].strip('.root') + '.amp.root'
  
  
  k = ROOT.KAmpKounselor()
  kg = ROOT.KGrandCanyonKAmpSite()
  k.AddKAmpSite(kg)
  theRet = k.RunKamp(args[0]['proc1']['file'], newFileName)
  
  processdoc = {}
  processdoc['file'] = newFileName
  
  return (processdoc, theRet)
  
def processOne(doc):
  global myProc
  
  print 'have doc', doc['_id']
  doc['status'] = 'proc2 in progress'
  myProc.upload(doc)
  procDict, theRet = myProc.doprocess(doc) #this step calls runProcess
  print 'runprocess returned', theRet

  #add a few more items to the document
  dd = datetime.datetime.utcnow()
  procDict['date'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
  procDict['processname'] = 'kdataRaw2Amp'
  procDict['hostname'] = socket.gethostname()
  procDict['localuname'] = os.uname()
  
  #this step will add the procDict dictionary to the 
  #database document and then upload it to the DB
  doc['proc2'] = copy.deepcopy(procDict)
  
  if theRet:
    doc['status'] = 'good'
    return (doc, True)
  else:
    print 'the process returned an empty dictionary!'
    doc['status'] = 'proc2 failed'
    return (doc, False)
    
def setupProc(server, database, function):
  global myProc
  myProc = DBProcess(server, database, function)
  return myProc
  
def main(*argv):
  '''
  argv[0] is the couchdb server (http://127.0.0.1:5984)
  argv[1] is the database (datadb)
  argv[2] is the document id

  process 2 - runs the KAmpKounselor with the KGrandCanyonKampSite
  This script is meant to be run on ccage.in2p3.fr with access to the /sps/edelweis directory
  because that is where we expect the data files to be located.
  '''
  
  #create a DBProcess instance, which will assist in uploading the proc
  #document to the database... although, its barely useful... 
  global myProc
  myProc = setupProc(argv[0], argv[1], runProcess)
  doc = myProc.get(argv[2])

  (doc, result) = processOne(doc)
  myProc.upload(doc)


if __name__ == '__main__':
  ROOT.gROOT.SetBatch(True)
  main(*sys.argv[1:])
