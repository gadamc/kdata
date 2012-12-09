#!/usr/bin/env python 
#

import sys, os, time, datetime, string, re
import uploadRadonDataFile as up
from couchdbkit import Server, Database
import json

def main(*argv):
  uri = 'https://%s:%s@edelweiss.cloudant.com' % (argv[0], argv[1])
  dbname = 'radon'
  dataDir = '\\Radon\Data\\'  #this is where i will look for data files. we won't look recursively... just in this directory
  
  prog = re.compile('[0-9]*?_[0-9]*?.txt')  #this is the expected string pattern
  
  while True:
    
    #we get the data file on the database that has the highest
    #run number.
    try:
      s = Server(uri)
      db = s[dbname]
      vr = db.view('app/runNumber', reduce =False, limit = 1, descending=True)
      row = vr.first()
      try:
        majorRun = row['key'][0]
        minorRun = row['key'][1]
      except:
        majorRun = -1
        minorRun = -1
        pass
      
      print 'most recent data %d_%d' % (majorRun, minorRun)
      
      #now, see if any of the files in the data directory have 
      #a larger run number
      files = os.listdir(dataDir)
      for afile in files:
        
        if prog.match(afile):  
          majr, minr = afile.strip('.txt').split('_')
          majr = int(majr)
          minr = int(minr)
          #print majr, minr, majorRun, minorRun
          if (majr > majorRun) or (majr == majorRun and minr > minorRun):
            print 'uploading', os.path.join(dataDir, afile)
            up.upload(uri, dbname, os.path.join(dataDir, afile))

      print 'sleeping...', datetime.datetime.now()
      time.sleep(3600)  #wait one hour to look for more data

    except Exception as e: 
      ej = json.loads(e.msg)
      
      if ej['error'] == 'unauthorized'
        pass  #just wait and try again. 

      else raise e  #break - something else happened. 

if __name__ == '__main__':
  main(*sys.argv[1:])
