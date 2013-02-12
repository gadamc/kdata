#!/usr/bin/env python

import sys, os, re, time
import KDataPy.samba_utilities as sut

def isFileOldEnough(aFileName):
  '''
  returns True if aFileName is older than 15 minutes from now. 
  '''
  if os.path.getmtime(aFileName) < time.time() - 900:
    return True
  else:
    return False

def _appendToList(aList, aPath, message):
  aList.append(aPath)
  print message, 'adding', aPath, 'to the list'

def getListOfNewSambaFiles(topDir, lastSambaFileDict):

  sambafiles = list()
  metafiles = list()

  
  if os.path.isdir(topDir)==False:
    print topDir, 'is not a directory'
    return sambafiles
  
  try:
    for dirpath, dirs, files in os.walk(topDir):
      for aFileName in files:

        parentdir = os.path.basename(dirpath) 

        #check for data file
        if sut.isvalidsambadatafilename(aFileName) and sut.isvalidsambarunname(parentdir) and isFileOldEnough(aFileName):
        
          sambaLetter = parentdir[4] 
          add = False

          if lastSambaFileDict['samba'].has_key(sambaLetter):
            if aFileName > lastSambaFileDict['samba'][sambaLetter]['lastfile']:    
              add=True
          else:
            add=True

          if add:
            _appendToList(sambafiles, os.path.join(dirpath, aFileName), 'samba data file found:')


        #check for meta data file
        if sut.isvalidsambametadatafilename(aFileName) and sut.isvalidsambarunname(parentdir) and isFileOldEnough(aFileName):
        
          sambaLetter = parentdir[4] 
          add = False

          if lastSambaFileDict['samba'].has_key(sambaLetter):
            if aFileName > lastSambaFileDict['samba'][sambaLetter]['last_%s' % aFileName.split('_')[1] ]:    
              add=True
          else:
            add=True

          if add:
            _appendToList(metafiles, os.path.join(dirpath, aFileName), 'samba meta data file found:')
  except Exception as e:
    print aFileName, parentdir, sambaLetter, len(sambafiles), len(metafiles)
    raise e


  return sambafiles, metafiles
  


  
