#!/usr/bin/env python

import os, string, sys
import datetime
import re
import json
import KDataPy.scripts.dataprocess.uploadSambaHeaderToCouch as upload
import KDataPy.scripts.dataprocess.findSambaPartitionFiles as find
from KDataPy.scripts.dataprocess.sambaFileDataDBTracker import SambaFileDataDBTracker

def main(uri = 'http://127.0.0.1:5984', db = 'datadb',
        dataDir = '/Users/adam/Scripts/rawdata'):
        
  
  print ''
  print 'Starting SambaToCouch', datetime.datetime.now()
  print 'Initial Arguments:', uri, db, dataDir
  

  tracker = SambaFileDataDBTracker( uri, db, trackerDoc_id)
  trackerDoc = tracker.getTrackerDoc()    
  
  
  print ''
  print 'Samba Database Doc Tracker file read: ', trackerDoc['_id']
  print trackerDoc
  
  
  filelist, metafilelist = find.getListOfNewSambaFiles(dataDir,trackerDoc)
  
  
  for afile in filelist:

    print 'Uploading ', afile , 'to Couch'
    raw_input()
    if upload.uploadFile(afile, uri, db):
      tracker.setLastSambaDataFile( os.path.basename(afile) )
    else:
      raise KDataDatabaseError( 'Something failed when uploading the file: %s' % afile )
      
  for ametafile in metafilelist:

    print 'Uploading ', ametafile , 'to Couch'
    raw_input()
    if upload.uploadMetaFile(ametafile, uri, db):
      tracker.setLastSambaMetaFile( os.path.basename(ametafile) ) 
    else:
      raise KDataDatabaseError( 'Something failed when uploading the file: %s' % ametafile )
      
  
  print 'done', datetime.datetime.now()
  
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  
