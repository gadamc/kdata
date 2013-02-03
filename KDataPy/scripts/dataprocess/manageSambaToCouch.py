#!/usr/bin/env python

import os, string, sys
import datetime
import re
import json
import KDataPy.scripts.dataprocess.uploadSambaHeaderToCouch as upload
import KDataPy.scripts.dataprocess.findSambaPartitionFiles as find
from KDataPy.scripts.dataprocess.sambaFileDataDBTracker import SambaFileDataDBTracker

def main(uri = 'http://127.0.0.1:5984', db = 'datadb',
        dataDir = '/Users/adam/Scripts/rawdata', 
        trackerDoc_id = None):
        
  
  print ''
  print 'Starting SambaToCouch', datetime.datetime.now()
  print 'Initial Arguments:', uri, db, dataDir, trackerDoc_id
  

  tracker = SambaFileDataDBTracker( uri, db, trackerDoc_id)
  trackerDoc = tracker.getTrackerDoc(trackerDoc_id)    
  
  
  print ''
  print 'Samba Database Doc Tracker file read: ', trackerDoc['_id']
  print trackerDoc
  
  
  filelist, metafilelist = find.getListOfNewSambaFiles(dataDir,trackerDoc)
  
  
  errors = False

  for afile in filelist:

    print 'Uploading ', afile , 'to Couch'

    if upload.uploadFile(afile, uri, db):
      tracker.setLastSambaDataFile( afile)
    else:
      print 'Something failed when uploading the file.', afile
      errors = True    
      break  #STOP if something has gone wrong.... don't want to make the problem worse
      
  for ametafile in metafilelist:

    print 'Uploading ', ametafile , 'to Couch'
 
    if upload.uploadMetaFile(ametafile, uri, db):
      tracker.setLastSambaMetaFile( ametafile ) 
    else:
      print 'Something failed when uploading the file.', ametafile
      errors = True
      break
      

  if errors:
    print 'there were errors!!!!  '
  print 'done', datetime.datetime.now()
  
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  
