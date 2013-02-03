#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
from KDataPy.exceptions import *
import os, sys, tempfile, shutil, datetime, time
import KDataPy.scripts.dataprocess.sftpToSps as sftp

class ManagedSendToLyon:
  '''
    This class manages sending local data files to Lyon. This class is used both by runProc0 and runMetaProc0. 

    To use this class, when you instantiate it, you must provide the name of the database map-reduce "view" and the name of
    the key of the record that will be attached to the database document to record this action. 

    Here's a description of what happens when you use this class.

    1. Instantiate with object (see __init__ for details of needed values)
    2. Call the method sendAvailableDocs.
    3. In this method, the database viewName results are checked. 
    4. For each document (doc) returned by the view...
      5. It sets the doc['status'] to 'dbRecordName in progress' and saves the doc to the database
      6. Sends the local file found in doc['file'] to Lyon via sftp.
      7. Upon successful transfer of the data, sets doc['status'] to 'good' (or to 'dbRecordName failed', if unsuccessful)
      8. Information about the transfer and the file location in Lyon is stored in the doc[dbRecordName] key 
      9. The doc is saved back to the datbase.

    This class requires that the documents returned by the database View results has the key 'file', which points
    to a valid local file. 


  '''

  def __init__(self, server, datebase, username, password, viewName, dbRecordName, limit=False, lyonpath = '/sps/edelweis/kdata/data/raw/'):
    '''
      server - couchdb server, including credentials.
      database - name of the couchdb database
      username - username of the account on the Lyon CC.
      password - password for 'username'. this will use sftp.
      viewName - the database viewname in couchdbkit format. That is, for view nameed 'proc0' in the document '_design/proc',
          viewName would be 'proc/proc0'
      dbRecordName - this is the key name that is given 
      limit - if set to true, the method 'sendAvailableDocs' will only send one document, at maximum
      lyonpath - by default set to '/sps/edelweis/kdata/data/raw/'
    '''

    self.server = server
    self.database = database
    self.username = username
    self.password = password
    self.viewName = viewName  #something like 'proc/proc0'
    self.dbRecordName = dbRecordName  #something like 'proc0'
    self.limit = limit
    self.path = lyonpath

    #create a DBProcess instance, which will assist in uploading the
    #document to the database
    self.myProc = DBProcess( self.server, self.database, self._sendToLyon)
    

  def _sendToLyon(self, fileName, **kwargs):
    
    print 'calling sftpToSps.send'
    
    try:
      sftpRet = sftp.send(self.username, self.password, fileName, self.path)
    except Exception as e:
      raise KDataTransferError('KDataTransferError. ManagedSendToLyon._sendToLyon \n' + str(type(e)) + ' : ' + str(e))

    return sftpRet
    
    
  def sendAvailableDocs():
    
    
    print '\n', str(datetime.datetime.now()), ': starting ManagedSendToLyon.sendAvailableDocs \n'
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

