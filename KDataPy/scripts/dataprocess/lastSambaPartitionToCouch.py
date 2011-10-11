#!/usr/bin/env python

import os, string, sys

def getEmptyFileDict():
  letters = string.lowercase
  files = dict()
  for i in range(len(letters)):
    files[letters[i]] = {'run':'', 'file':0}
  return files
  

def getDictOfLastFiles(theLastFile):
  
  files = getEmptyFileDict()
  
  if os.path.isfile(theLastFile):
    file = open(theLastFile, 'rU')
    for line in file:
      splitline = line.split(' ')
      if splitline[0].strip() != '':  #skip blank lines!
        files[line.strip()[4:5]]['run'] = splitline[0].strip()
        files[line.strip()[4:5]]['file'] = int(splitline[1].strip())
      
    file.close()
  return files

def writeToLastSambaPartitionToCouchFile(theLastFile, aSambaFile):
  lastfiles = getDictOfLastFiles(theLastFile)
  
  lastfiles[os.path.basename(aSambaFile)[4:5]]['run'] = os.path.basename(aSambaFile).split('_')[0]
  lastfiles[os.path.basename(aSambaFile)[4:5]]['file'] = int(  os.path.basename(aSambaFile).split('_')[1] )
  
  file = open(theLastFile,'w')
  
  for k, v in lastfiles.items():
    if v['run'] != '':
      file.write( lastfiles[k]['run']+ ' ' + str(lastfiles[k]['file']) + '\n')
      
  file.close()  
