#!/usr/bin/env python 
#

import sys, os, time, datetime, string, re
import uploadRadonDataFile as up

def uploadDir(*argv):
  server = argv[0]
  dbname = argv[1]
  dirContents = os.listdir(argv[2])
  
  for item in dirContents:
    if os.path.isfile(os.path.join(argv[2],item)):
      if re.match('[0-9]*_[0-9]*.txt', os.path.basename(item)) != None:
        print 'uploading', item
        up.upload(server, dbname, os.path.join(argv[2],item))

def uploadFileInRange(*argv):
  server = argv[0]
  dbname = argv[1]
  theDir = argv[2]
  startFileName = argv[3]
  endFileName = argv[4]
  
  dirContents = os.listdir(argv[2])
  
  for item in dirContents:
    if os.path.isfile(os.path.join(argv[2],item)) and os.path.basename(item) >= startFileName and os.path.basename(item) <= endFileName:
      if re.match('[0-9]*_[0-9]*.txt', os.path.basename(item)) != None:
        print 'uploading', item
        up.upload(server, dbname, os.path.join(argv[2],item))
        

if __name__ == '__main__':
  if len(sys.argv) == 4:
    uploadDir(*sys.argv[1:])
  elif len(sys.argv) == 6:
    uploadFileInRange(*sys.argv[1:])