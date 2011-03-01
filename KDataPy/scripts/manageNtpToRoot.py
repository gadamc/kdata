#!/usr/bin/env python

import os, string, sys, lastNtpRootFileDate as last, buildSambaNtpToRoot as build
import findNtpFilesForRoot as find
import datetime


def main(dataDir = 'rawdata', overwrite = False, lastfile = 'lastNtpToRoot.txt',
        logFileName = 'ntpToRoot.log', startDir = ''):
        
  logfile = open(logFileName, 'a')
  sys.stdout = logfile
  sys.stderr = logfile
  print ''
  print 'Starting NtpToRoot', datetime.datetime.now()
  print 'Initial Arguments', dataDir, overwrite, lastfile, logFileName, startDir
  
  if startDir == '':
    lastDate = last.getLastNtpDate(lastfile)
    print 'Found Last Date:', lastDate
    lastDate += 'a000'
  else:
    lastDate = startDir
  filelist = find.main(dataDir,overwrite,lastDate)
  logfile.flush()
  currentFile = lastDate
  
  for i in filelist:
    build.main(i, os.path.dirname(i))
    currentFile = os.path.basename(i).strip('_ntp')
    last.writeLastNtpDate(lastfile, currentFile)
    logfile.flush()
    
  
  print 'done', datetime.datetime.now()
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  