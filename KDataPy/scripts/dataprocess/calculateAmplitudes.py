#!/usr/bin/env python
import numpy as np
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
  fin = KDataReader(argv[0])
  fout = KDataWriter(argv[1], 'KAmpEvent')
  ein = fin.GetEvent()
  eout = fin.GetEvent()
   
  configArg = None
  if len(argv) >= 3:
    configArg = argv[2]
  
  config = getConfiguration(configArg)
  
  pulseIonA = np.zeros(8196)
  pulseIonB = np.zeros(8196)

  pulseHeatA = np.zeros(512)
  pulseHeatB = np.zeros(512)


  

if __name__ == '__main__':
  main(*sys.argv[1:])