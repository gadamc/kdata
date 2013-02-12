#!/usr/bin/env python

import os, string, sys
import KDataPy.samba_utilities as sut
import couchdbkit
from KDataPy.exceptions import KDataDatabaseError

class SambaFileDataDBTracker:

  def __init__(self, couchDB_ServerName, couchDB_DatabaseName, db_doc_id = 'sambaToCouchDBTrackerDoc'):
    '''
      couchDB_DataBase is a couchdbkit.Database object
    '''

    self.trackerdoc_id = db_doc_id  
    self.db = couchdbkit.Server(couchDB_ServerName)[couchDB_DatabaseName]

  def _getEmptyTrackerDoc(self):
    
    doc = {}
    doc['samba'] = {}
    doc['_id'] = self.trackerdoc_id

    for aletter in string.lowercase:
      doc['samba'][ aletter ] = {'lastfile':'', 'last_BB': False, 'last_ntp' : False, 'last_log': False, 'last_setup.csv': False, 'last_seuils':False}
    return doc

  def _save_doc(self, doc):
    res = self.db.save_doc(doc)
    if not res['ok']:
      raise KDataDatabaseError('SambaFileDataDBTracker could not set data file')

  def getTrackerDoc(self):

    return self.db[self.trackerdoc_id ]

  def setLastSambaDataFile(self, aSambaDataFileName):

    if sut.isvalidsambadatafilename(aSambaDataFileName) is False:
      raise TypeError('%s : Invalid Samba Data File Name' % aSambaDataFileName)

    trackerDoc = self.getTrackerDoc()
    sambakey = os.path.basename(aSambaDataFileName)[4]

    trackerDoc['samba'][sambakey]['lastfile'] = os.path.basename(aSambaDataFileName) 
    
    self._save_doc(trackerDoc)
    

  def setLastSambaMetaFile(self, aSambaMetaFileName):

    if sut.isvalidsambametadatafilename(aSambaMetaFileName) is False:
      raise TypeError('%s : Invalid Samba Meta Data File Name' % aSambaMetaFileName)

    trackerDoc = self.getTrackerDoc()
    sambakey = os.path.basename(aSambaMetaFileName)[4]
    metakey = 'last_%s' % os.path.basename(aSambaMetaFileName).split('_')[1]

    trackerDoc['samba'][sambakey][metakey] = os.path.basename(aSambaMetaFileName) 
    
    self._save_doc(trackerDoc)


