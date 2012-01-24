#!/usr/bin/env python

'''
  This script converts the raw Samba files into raw KData files. 
  
'''

import ROOT

import os, sys, glob

def getSambaFilePattern():
  return '[d-n][a-m][0-9][0-9][a-z][0-9][0-9][0-9]_[0-9][0-9][0-9]'

def convertfile(input, output):
  '''
    converts input samba file into output kdata file. If successful, this returns the 'output',
    otherwise it returns an empty string ( '' )
  '''
  ROOT.gSystem.Load('libkds')
  ROOT.gSystem.Load('libksamba')
  ROOT.gROOT.SetBatch(True)
  
  if os.path.isfile(input) and os.path.isdir(output)==False:
    c = ROOT.KSamba2KData(input, output)
    if c.ConvertFile():
      print 'converted file to root file'
    else:
      print 'failed to convert file'
      return ''
  else:
    print 'rootifySambaData.convertfile. Input must be a file path and Output cannot be a directory'
    return ''
  
  return output
    
def convertdir(input, output):
  
  rootFiles = list()
  
  if os.path.isdir(input) and os.path.isdir(output):
    allfiles = glob.glob(os.path.join(input, getSambaFilePattern()))

    for file in allfiles:
      rootFiles.append( convertfile(file, os.path.join(output, os.path.basename(file)+'.root')) )
    
  else:
    print 'rootifySambaData.convertdir. Input and Output must be directory path'
    
  return rootFiles
