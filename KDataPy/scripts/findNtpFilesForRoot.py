#!/usr/bin/env python

import sys, os, re


def getListOfNtpsToRootify(topDir, minimumNtpDict):

  ntps = list()
  if os.path.isdir(topDir)==False:
    print topDir, 'is not a directory'
    return ntps

  sambaex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]'
  sambaex_ntp = sambaex + '_ntp'
  
  for dirpath, dirs, files in os.walk(topDir):
    for f in files:
      parentdir = dirpath.split('/')[len(dirpath.split('/'))-1]
      if re.match(sambaex_ntp, f) and re.match(sambaex,parentdir) and re.search('.root',f)==None:
        fsize = os.path.getsize(os.path.join(dirpath,f))
        if f == minimumNtpDict[f[4:5]]['name']:  #check that the current file name is greater than or equal to the minimum 
          if fsize > minimumNtpDict[f[4:5]]['size']: #if the file size has increased, then we put it in the list
            ntps.append(os.path.join(dirpath,f))
            print 'file', f, fsize, 'is newer than', minimumNtpDict[f[4:5]]['name'], minimumNtpDict[f[4:5]]['size']
        elif f > minimumNtpDict[f[4:5]]['name']:
          ntps.append(os.path.join(dirpath,f))
          print 'file', f, fsize, 'is newer than', minimumNtpDict[f[4:5]]['name'], minimumNtpDict[f[4:5]]['size']

  return ntps
  

def main(*argv):
  
  files = getListOfNtpsToRootify(*argv[0:]) 
  return files
  

if __name__ == '__main__':
  print main(*sys.argv[1:])
  