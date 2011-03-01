#!/usr/bin/env python

import sys, os, re


def getListOfNtpsToRootify(topDir, overwrite = False, startDir = 'aa00a000'):

  ntps = list()
  if os.path.isdir(topDir)==False:
    print topDir, 'is not a directory'
    return ntps

  sambaex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]'
  sambaex_ntp = sambaex + '_ntp'
  
  for dirpath, dirs, files in os.walk(topDir):
    pathlist = dirpath.split('/')
    currentdir = pathlist[len(pathlist)-1]
    
    if re.match(sambaex, currentdir):
      if currentdir >= startDir:
        for f in files:
          if re.match(sambaex_ntp, f) and re.search('.root',f)==None:
            if os.path.isfile(os.path.join(dirpath,f+'.root'))==False or overwrite:
              ntps.append(os.path.join(dirpath,f))

  return ntps
  

def main(*argv):
  
  files = getListOfNtpsToRootify(*argv[0:]) 
  return files
  

if __name__ == '__main__':
  print main(*sys.argv[1:])
  