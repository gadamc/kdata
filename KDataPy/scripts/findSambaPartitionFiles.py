#!/usr/bin/env python

import sys, os, re


def getListOfSambaFilesToUpload(topDir, minimumSambaFileDict):

  sambafiles = list()
  if os.path.isdir(topDir)==False:
    print topDir, 'is not a directory'
    return sambafiles

  sambaex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]'
  sambaex_partition = sambaex + '_[0-9][0-9][0-9]'
  
  for dirpath, dirs, files in os.walk(topDir):
    for f in files:
      parentdir = dirpath.split('/')[len(dirpath.split('/'))-1]
      if re.match(sambaex_partition, f) and re.match(sambaex,parentdir) and re.search('.root',f)==None and f.endswith('.gz')==False:
        
        fnumber = int(f.split('_')[1])
        runname = f.split('_')[0]
        mac = runname[4:5]
        
        if runname == minimumSambaFileDict[mac]['run']:  #check that the current file name is greater than or equal to the minimum 
          if fnumber > minimumSambaFileDict[mac]['file']: #if the file number has increased, then we put it in the list
            sambafiles.append(os.path.join(dirpath,f))
            print 'file', runname, fnumber, 'is newer than', minimumSambaFileDict[mac]['run'], minimumSambaFileDict[mac]['file']
            
        elif runname > minimumSambaFileDict[mac]['run']:
          sambafiles.append(os.path.join(dirpath,f))
          print 'file', runname, fnumber, 'is newer than', minimumSambaFileDict[mac]['run'], minimumSambaFileDict[mac]['file']
          
        else:
          pass
          #print 'file', runname, fnumber, 'is older than', minimumSambaFileDict[mac]['run'], minimumSambaFileDict[mac]['file']

  return sambafiles
  

def main(*argv):
  
  files = getListOfSambaFilesToUpload(*argv[0:]) 
  return files
  

if __name__ == '__main__':
  print main(*sys.argv[1:])
  
