#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
import os, sys, tempfile, shutil, datetime, copy, socket
from couchdbkit import Server, Database

import ROOT

def fillGrandCanyonParameters(dataFile, kg):
  global myProc
  db = myProc.sv['pulsetemplates']
  
  #have to scan through the file to find
  #the names of the channels that have data in this file
  #and also need to get a date string for the first event
  #in this file.
  
  ftemp = ROOT.KDataReader(dataFile)
  e = ftemp.GetEvent()
  chanList = []
  filedate = ''
  for i in range(ftemp.GetEntries()):
    ftemp.GetEntry(i)
    if filedate == '':
      if e.GetNumSambas()>0:
        s = e.GetSamba(0)
        dd = datetime.datetime.utcfromtimestamp(s.GetNtpDateSec())
        filedate = str(dd)
    for j in range(e.GetNumBoloPulses()):
      p = e.GetBoloPulse(j)
      if p.GetChannelName() not in chanList:
        chanList.append(p.GetChannelName())
  ftemp.Close()
  
  trapKamper = kg.GetTrapKamperProto()
  for chan in chanList:
    vr = db.view('analytical/bychandate', descending=True, reduce=False,startkey=[chan,filedate], endkey=[chan,''], limit=1, include_docs=True)
    try:
      doc = vr.first()['doc']
      trapKamper.SetTrapAmplitudeDecayConstant(doc['channel'],doc['kampsites']['KGrandCanyonKAmpSite']['trapAmpDecayConstant'])
    except: #this will throw if vr.first() doesn't return a document. just ignore it and move on to the next channel
      pass

def fillBlackForestParameters(dataFile, kbf):
  global myProc
  db = myProc.sv['pulsetemplates']
  f = ROOT.KDataReader(dataFile)
  e = f.GetEvent()
  chanList = []
  fileDate = ''
  for entry in range(f.GetEntries()):
    f.GetEntry(entry)
    if (fileDate == '') and (e.GetNumSambas()>0):
      timestamp = e.GetSamba(0).GetNtpDateSec()
      fileDate = str(datetime.datetime.utcfromtimestamp(timestamp))
    for pulse in range(e.GetNumBoloPulses()):
      prec = e.GetBoloPulse(pulse)
      if prec.GetChannelName() not in chanList:
        chanList.append(prec.GetChannelName())
  f.Close()
  settings = {}
  Kamper = kbf.GetBFKamper()
  for chan in chanList:
    vr = db.view('analytical/bychandate', descending=True, reduce=False,startkey=[chan,'2013'], limit=1, include_docs=True)
    try:
      doc = vr.first()['doc']
      params = ROOT.std.vector("double")()
      paramlist = doc['kampsites']['KBlackForestKAmpSite']['params']
      for i in range(len(paramlist)):
        params.push_back(paramlist[i])
      Kamper.SetParams(doc['channel'],params)
      settings[doc['channel']] = paramlist
    except: #this will throw if vr.first() doesn't return a document. just ignore it and move on to the next channel
      pass

def runProcess(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the database document.'
    #sys.exit(-1)

  #
  newFileName = args[0]['proc1']['file'].strip('.root') + '.amp.root'
  theRet = False
  exc = {}

  ROOT.gSystem.Load('libkds')
  ROOT.gSystem.Load('libkpta')
  ROOT.gSystem.Load('libkamping')
  
  
  k = ROOT.KAmpKounselor()
  kg = ROOT.KGrandCanyonKAmpSite()
  fillGrandCanyonParameters(args[0]['proc1']['file'], kg)
  k.AddKAmpSite(kg)

  klb = ROOT.KLibertyBellKAmpSite()
  fillGrandCanyonParameters(args[0]['proc1']['file'], klb)
  k.AddKAmpSite(klb)
  
  #kbf = ROOT.KBlackForestKAmpSite()
  #fillBlackForestParameters(args[0]['proc1']['file'], kbf)
  #k.AddKAmpSite(kbf)
  
  theRet = k.RunKamp(args[0]['proc1']['file'], newFileName)
  
  processdoc = {}
  
  if theRet == True:
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
  procDict['date'] = str(datetime.datetime.utcnow())
  procDict['processname'] = 'kdataRaw2Amp'
  procDict['hostname'] = socket.gethostname()
  procDict['localuname'] = os.uname()
  
  #this step will add the procDict dictionary to the 
  #database document and then upload it to the DB
  if doc.has_key('proc2') == False:
    doc['proc2'] = {}
      
  doc['proc2'].update(procDict)
  
  if theRet == True:
    doc['status'] = 'good'
    return (doc, True)
  else:
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

  try:
    (doc, result) = processOne(doc)
  except Exception as e:
    doc['exception'] = str(type(e)) + ': ' + str(e)
    doc['status']  = 'proc2 failed'

  myProc.upload(doc)


if __name__ == '__main__':
  ROOT.gROOT.SetBatch(True)
  main(*sys.argv[1:])
