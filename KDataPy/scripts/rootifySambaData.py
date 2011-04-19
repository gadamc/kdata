#!/usr/bin/env python

'''
  This script converts the raw Samba files into raw KData files. 
  
'''
from ROOT import KSamba2KData

import os, sys, glob

def getSambaFilePattern():
  return '[d-n][a-m][0-9][0-9][a-z][0-9][0-9][0-9]_[0-9][0-9][0-9]'

def convertfile(input, output):

  if os.path.isfile(input) and os.path.isdir(output)==False:
    c = KSamba2KData(input, output)
    c.ConvertFile()
  else:
    print 'rootifySambaData.convertfile. Input must be a file path and Output cannot be a directory'
    return ''
  
  return output
    
def convertdir(input, output):
  
  rootFiles = list()
  
  if os.path.isdir(input) and os.path.isdir(output):
    allfiles = glob.glob(os.path.join(input, getSambaFilePattern()))

    for file in allfiles:
      rootFiles.append(convertfile(file, os.path.join(output, file + '.root')))
    
  else:
    print 'rootifySambaData.convertdir. Input and Output must be directory path'
    
  return rootFiles