#!/usr/bin/env python

import os, string, sys, lastNtpRootFileDate as last, buildSambaNtpToRoot as build
import findNtpFilesForRoot as find
import makeNtpRootAmplCorrelationPlots as plots
import datetime


def main(dataDir = 'rawdata', outputDir = 'rawdata/ntp2root', overwrite = False, lastfile = 'lastNtpToRoot.txt',
        logFileName = 'ntpToRoot.log', startDir = ''):
        
  logfile = open(logFileName, 'a')
  sys.stdout = logfile
  sys.stderr = logfile
  print ''
  print 'Starting NtpToRoot', datetime.datetime.now()
  print 'Initial Arguments:', dataDir, outputDir, overwrite, lastfile, logFileName, startDir
  
  if outputDir != '':
    if os.path.isdir(outputDir) == False:
      print outputDir, 'is not a directory. exiting -1'
      sys.exit(-1)
      
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
    if outputDir == '':
      rootFile = build.buildRootFile(i, os.path.dirname(i))
    else:
      rootFile = build.buildRootFile(i, outputDir)
    currentFile = os.path.basename(i).strip('_ntp')
    last.writeLastNtpDate(lastfile, currentFile)
    if outputDir == '':
      plots.main(rootFile, os.path.dirname(i))
    else:
      plots.main(rootFile, outputDir)
    logfile.flush()
    
  
  print 'done', datetime.datetime.now()
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  