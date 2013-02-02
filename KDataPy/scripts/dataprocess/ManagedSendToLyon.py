#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
from KDataPy.exceptions import *
import os, sys, tempfile, shutil, datetime, time
import KDataPy.scripts.dataprocess.sftpToSps as sftp

class ManagedSendToLyon:

  def __init__(self, server, datebase, username, password, viewName, dbRecordName, limit=False):
    self.server = server
    self.database = database
    self.username = username
    self.password = password
    self.viewName = viewName  #something like 'proc/proc0'
    self.dbRecordName = dbRecordName  #something like 'proc0'
    self.limit = limit

    #create a DBProcess instance, which will assist in uploading the
    #document to the database
    self.myProc = DBProcess( self.server, self.database, self._sendToLyon)
    

  def _sendToLyon(self, fileName, **kwargs):
    
    print 'calling sendBoloData'
    
    try:
      sftpRet = sftp.sendBoloData(self.username, self.password, fileName)
    except Exception as e:
      raise KDataTransferError('KDataTransferError. ManagedSendToLyon.py line 31  \n' + str(type(e)) + ' : ' + str(e))

    return sftpRet
    
    
  def sendAvailableDocs():
    
    
    print '\n', str(datetime.datetime.now()), ': starting ManagedSendToLyon. \n'
    print self.viewName, self.dbRecordName

    
    if self.limit 
      vr = self.myProc.view(self.viewName, reduce=False, limit=1)
    
    successfulDocs = []
    failedDocs = []

    for row in vr:
      print row['id']
      
      try:
        doc = self.myProc.get(row['id'])
        print 'have doc', doc['_id']
        doc['status'] = self.dbRecordName + ' in progress'
        self.myProc.upload(doc)

        procDict = self.myProc.doprocess(doc['file']) #this step calls _sendToLyon
        print 'called process'

        if len(procDict) > 0:
          #add a few more items to the document
          procDict['date'] = str(datetime.datetime.utcnow())
          procDict['date_unixtime'] = time.time()
          procDict['processname'] = 'ManagedSendToLyon'
        
          #if len(procDict['sftpErrs']) > 0:
          #  doc['status'] = self.dbRecordName + ' failed'
          #else:
          
          doc['status'] = 'good'
          #this step will add the procDict dictionary to the 
          #database document and then upload it to the DB
          if doc.has_key(self.dbRecordName) == False:
            doc[self.dbRecordName] = {}
          
          doc[self.dbRecordName].update(procDict)
          self.myProc.upload(doc)
          successfulDocs.append(doc['_id'])

        else:
          doc['status'] = self.dbRecordName + ' failed'
          self.myProc.upload(doc)
          print 'the process returned an empty dictionary!'
          failedDocs.append(doc['_id'])

      except Exception as e:
        print e
        if doc.has_key(self.dbRecordName) == False:
            doc[self.dbRecordName] = {}
          
        doc[self.dbRecordName]['exception'] = str(type(e)) + ': ' + str(e)
        doc['status'] = self.dbRecordName + ' failed'
        #if doc.has_key(self.dbRecordName): del doc[self.dbRecordName]
        self.myProc.upload(doc)
        failedDocs.append(doc['_id'])

    return (successfulDocs, failedDocs)    

