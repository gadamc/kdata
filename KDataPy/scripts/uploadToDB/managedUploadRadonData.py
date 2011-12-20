#!/usr/bin/env python 
#

import sys, os, time, datetime, string, re
import uploadRadonDataFile as up
from couchdbkit import Server, Database

def main(*argv):
  uri = 'https://edwdbuser:3000kgd@134.158.176.27:6984' #security by obscurity 
  dbname = 'radon'
  dataDir = '\\Radon\Data\\'  #this is where i will look for data files. we won't look recursively... just in this directory
  
  s = Server(uri)
  db = s[dbname]
  
  prog = re.compile('[0-9]?_[0-9]?.txt')  #this is the expected string pattern
  
  while True:
    
    #we get the data file on the database that has the highest
    #run number. 
    vr = db.view('app/runNumber', reduce =False, limit = 1, descending=True)
    row = vr.first()
    majorRun = row.key[0]
    minorRun = row.key[1]
    
    #now, see if any of the files in the data directory have 
    #a larger run number
    files = os.listdir(dataDir)
    for afile in files:
      if prog.match(afile):  
        majr, minr = afile.strip('.txt').split('_')
        if majr > majorRun or (majr == majorRun and minr > minorRun):
          
          up.upload(uri, dbname, os.path.join(dataDir, afile))
        
    
    time.sleep(600)  #wait 10 minutes

if __name__ == '__main__':
  main(*sys.argv[1:])