#!/usr/bin/env python
from couchdbkit import Server, Database

def getConfiguration(configDoc):
  config = {}
  
  
  
  if configDoc:
    s = Server('https://edwdbik.fzk.de:6984')
    db = s['analysis']
    config = db[configDoc]
    
  return config

def main(*argv):
  '''
  argv[0] == the input file
  argv[1] == the output file
  argv[2] == (optional) the couchdb database document that provides configuration options
  to this program. otherwise, a default configuration is used. 
  '''
  
  #Now, this routine will then call a precompiled executable passing in the configuration values.
  #This is because memory management can be done better (and more easily for the details that we
  #require for as fast as possible calculation) in C/C++
    

if __name__ == '__main__':
  main(*sys.argv[1:])