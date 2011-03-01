#!/usr/bin/env python

import os, string, sys

def getEmptyFileDict():
  letters = string.lowercase
  files = dict()
  for i in range(len(letters)):
    files[letters[i]] = {'name':'', 'size':0}
  return files
  

def getDictOfLastFiles(afile):
  
  files = getEmptyFileDict()
  
  if os.path.isfile(afile):
    file = open(afile, 'rU')
    for line in file:
      splitline = line.split(' ')
      if splitline[0].strip() != '':  #skip blank lines!
        files[line.strip()[4:5]]['name'] = splitline[0].strip()
        files[line.strip()[4:5]]['size'] = int(splitline[1].strip())
      
    file.close()
  return files

def writeToLastNtpFile(afile, anNtp):
  lastfiles = getDictOfLastFiles(afile)
  
  lastfiles[os.path.basename(anNtp)[4:5]]['name'] = os.path.basename(anNtp)
  lastfiles[os.path.basename(anNtp)[4:5]]['size'] = int(os.path.getsize(anNtp))
  
  file = open(afile,'w')
  
  for k, v in lastfiles.items():
    if v['name'] != '':
      file.write( lastfiles[k]['name']+ ' ' + str(lastfiles[k]['size']) + '\n')
      
  file.close()  
