#!/usr/bin/env python

import os, string, sys, lastNtpRootFileDate as last, buildSambaNtpToRoot as build
import findNtpFilesForRoot as find
import makeNtpRootAmplCorrelationPlots as plots
import printPrettyNtpRootGraphs as pretty
import datetime
import re

def main(dataDir = '/Users/adam/Scripts/makeNtpRoot/rawdata', outputDir = '/Users/adam/Scripts/makeNtpRoot/rawdata/ntp2root', 
        lastfile = '/Users/adam/Scripts/makeNtpRoot/lastNtpToRoot.txt',
        logFileName = '/Users/adam/Scripts/makeNtpRoot/ntpToRoot.log', startDir = ''):
        
  logfile = open(logFileName, 'a')
  sys.stdout = logfile
  sys.stderr = logfile
  print ''
  print 'Starting NtpToRoot', datetime.datetime.now()
  print 'Initial Arguments:', dataDir, outputDir, lastfile, logFileName, startDir
  
  if outputDir != '':
    if os.path.isdir(outputDir) == False:
      print outputDir, 'is not a directory. exiting -1'
      sys.exit(-1)
  
  lastFiles = last.getDictOfLastFiles(lastfile)    
              
  if startDir != '':
    startDir = startDir.split('/')[ len(startDir.split('/')) - 1 ]  #just get the last part of the directory
    if re.match('[d-n][a-m][0-9][0-9]', startDir[:4]) == False:
      print startDir, 'does\'t match the first four characters for the standard samba directory format. exiting'
      sys.exit(-1)
    
    for k, v in lastFiles.items():
      v['name'] = startDir[:4] + k + '000'
      v['size'] = 0

  print ''
  print 'Last NTP To Root File Read'
  for k, v in lastFiles.items():
    if v['name'] != '':
      print 'Mac', k, 'File', v['name'], 'Size', v['size']
  
  
  filelist = find.main(dataDir,lastFiles)
  
  print ''
  print 'List of NTP files to Rootify' 
  print filelist
  
  logfile.flush()
  
  
  for i in filelist:

    if outputDir == '':
      outputDir = os.path.dirname(i)
      
    rootFile = build.buildRootFile(i, outputDir)
    plotsoutput = plots.main(rootFile, outputDir)
    if plotsoutput != None:
      graphs = plotsoutput[0]
      plotFile = plotsoutput[1]
      #graphs is a dictionary if key = detectorName and value = list of TGraph
      pretty.main(plotFile, outputDir, graphs)
    
    last.writeToLastNtpFile(lastfile, i)
    logfile.flush()
    
  
  print 'done', datetime.datetime.now()
  
  
if __name__ == '__main__':
  
  main(*sys.argv[1:])
  
