#!/usr/bin/env python

from KDataPy.scripts.dataprocess.DBProcess import *
from KDataPy.exceptions import *
import os, sys, tempfile, shutil, datetime, time
import KDataPy.scripts.dataprocess.sftpToSps as sftp

class ManagedSendToLyon:
  '''
    This class manages sending local data files to Lyon. This class is used both by runProc0 and runMetaProc0. The runProc1
    script does NOT use this class because it would be more effort to go back and recode this right now.

    To use this class, when you instantiate it, you must provide the name of the database map-reduce "view" that provides the list
    of database documents that correspond to local data files that will be transferred to Lyon. Also, you must provide the name of
    the key of the record that will be attached to the database document to record the details of this action. 

    Here's a description of what happens when you use this class.

    1. Instantiate with object (see __init__ for details of needed values)
      1b - set the testDocFunction attribute if desired (see Advanced use below)
    2. Call the method ManagedSendToLyon.sendAvailableDocs.
    3. In this method, the database mapReduce view results are checked. 
    4. For each document (doc) returned by the view...
      5. It sets the doc['status'] to 'dbRecordName in progress' and saves the doc to the database (where dbRecordName = 'proc0' or 'metaproc0' for example)
      6. Sends the local file found in doc['file'] to Lyon via sftp.
      7. Upon successful transfer of the data, sets doc['status'] to 'good' (or to 'dbRecordName failed', if unsuccessful)
      8. Information about the transfer and the file location in Lyon is stored in the doc[dbRecordName] key 
      9. The doc is saved back to the datbase.

    This class requires that the documents returned by the database View results has the key 'file', which points
    to a valid local file. That is, it must have doc['file'] for step #6 to work.

    Advanced use:
          There is an uninitiated attribute to this class called testDocFunction. The user of this class can set this
          attribute to point to an external function that takes a database document as the input and returns True/False.
          This lets the user decide if action will be taken for each particular database document. 

          Example: runProc0.py (and runMetaProc0.py) uses this to test for the existence of the _log file in the S7 data directory
          If the _log file is found, then it assumes that Jules has copied over all of the data for this run and it returns true.
          When it returns True, then this class proceeds to copy over the data (performing steps 4-9 above)



  '''

  def __init__(self, server, database, username, password, viewName, dbRecordName, limit=False, lyonpath = '/sps/edelweis/kdata/data/raw/'):
    '''
      server - couchdb server, including credentials.
      database - name of the couchdb database
      username - username of the account on the Lyon CC.
      password - password for 'username'. this will use sftp.
      viewName - the database viewname in couchdbkit format. That is, for view nameed 'proc0' in the document '_design/proc',
          viewName would be 'proc/proc0'
      dbRecordName - this is the key name that is given to the database docs, such as 'proc0' or 'metaproc0'
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
    self.testDocFunction = None

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
    
    
  def sendAvailableDocs(self):
    
    
    print '\n', str(datetime.datetime.utcnow()), ': starting ManagedSendToLyon.sendAvailableDocs \n'
    print self.viewName, self.dbRecordName

    
    if self.limit: 
      vr = self.myProc.view(self.viewName, reduce=False, limit=1)
    else:
      vr = self.myProc.view(self.viewName, reduce=False)

    successfulDocs = []
    failedDocs = []

    for row in vr:
      print str(datetime.datetime.utcnow()), 'ManagedSendToLyon.py has doc', row['id']
      
      try:
        doc = self.myProc.get(row['id'])

        if self.testDocFunction is not None:
          if self.testDocFunction(doc) is False: 
            print str(datetime.datetime.utcnow()), 'ManagedSendToLyon.py testDocFunction returned false'
            continue

        doc['status'] = self.dbRecordName + ' in progress'
        self.myProc.upload(doc)

        print '  sending %s to lyon' % doc['file']
        procDict = self.myProc.doprocess(doc['file']) #this step calls _sendToLyon


        if len(procDict) > 0:
          print 'appending database document'

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
          print 'send to lyon returned an empty dictionary!'
          failedDocs.append(doc['_id'])

      except Exception as e:
        print 'an exception has occurred'
        print e
        if doc.has_key(self.dbRecordName) == False:
            doc[self.dbRecordName] = {}
          
        doc[self.dbRecordName]['exception'] = str(type(e)) + ': ' + str(e)
        doc['status'] = self.dbRecordName + ' failed'
        #if doc.has_key(self.dbRecordName): del doc[self.dbRecordName]
        self.myProc.upload(doc)
        failedDocs.append(doc['_id'])

    return (successfulDocs, failedDocs)    

