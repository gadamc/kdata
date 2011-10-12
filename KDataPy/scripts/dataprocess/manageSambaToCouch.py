#!/usr/bin/env python

import os, string, sys, lastSambaPartitionToCouch as last, uploadSambaHeaderToCouch as upload
import findSambaPartitionFiles as find
import datetime
import re

def main(uri = 'http://127.0.0.1:5984',
        dataDir = '/Users/adam/Scripts/uploadSambaToCouch/rawdata', 
        lastfile = '/Users/adam/Scripts/uploadSambaToCouch/lastSambaPartitionUploaded.txt',
        logFileName = '/Users/adam/Scripts/uploadSambaToCouch/sambatocouch.log', 
        errFileName = '/Users/adam/Scripts/uploadSambaToCouch/sambatocouch.err',
        db = 'datadb', startDir = '', override = None):
        
  logfile = open(logFileName, 'a')
  errfile = open(errFileName, 'a')
  sys.stdout = logfile
  sys.stderr = errfile
  print ''
  print 'Starting SambaToCouch', datetime.datetime.now()
  print 'Initial Arguments:', dataDir, lastfile, logFileName, uri, db, startDir
  
  lastFiles = last.getDictOfLastFiles(lastfile)    
  
  #print lastFiles
  
  if startDir != '':
    startDir = startDir.split('/')[ len(startDir.split('/')) - 1 ]  #just get the last part of the directory
    if re.match('[d-n][a-m][0-9][0-9]', startDir[:4]) == False:
      print startDir, 'does\'t match the first four characters for the standard samba directory format. exiting'
      sys.exit(-1)
    
    for k, v in lastFiles.items():
      v['run'] = startDir[:4] + k + '000'
      v['file'] = 0

  print ''
  print 'Last Samba Partition File Read'
  for k, v in lastFiles.items():
    if v['run'] != '':
      print 'Mac', k, 'SambaRun', v['run'], 'File', v['file']
  
  
  filelist = find.main(dataDir,lastFiles)
  
  print ''
  #print 'List of Samba Partition Files to Upload Header information to CouchDB' 
  #print filelist
  
  logfile.flush()
  
  
  for i in filelist:

    print 'Uploading ', i , 'to Couch'
    result = upload.uploadFile(i, uri, db, override)
    #graphs is a dictionary if key = detectorName and value = list of TGraph
    #result = True
    
    if(result == True):
      last.writeToLastSambaPartitionToCouchFile(lastfile, i)
    else:
      print 'Something failed when uploading the file.'
      
    logfile.flush()
    
  
  print 'done', datetime.datetime.now()
  
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  
