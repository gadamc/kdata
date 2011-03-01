#!/usr/bin/env python

import os, string, sys

def getLastNtpDate(afile):
  if os.path.isfile(afile):
    file = open(afile)
    return file.readline().strip()
  else:
    return 'aa01'

def writeLastNtpDate(afile, lastNtp, force = False):
  if os.path.isfile(afile):
    file = open(afile,'rU')
    current = file.readline()
    file.close()
  
  else:
    current = 'aa01a000'
  
  if len(lastNtp) < 4:
    print lastNtp, 'doesn\'t have enough characters. You\'re doing it wrong.'
  
  if force or current[:4] < lastNtp[:4]:
    file = open(afile,'w')
    file.write( lastNtp[0:2] + '{0:02d}'.format(int(lastNtp[2:4])-1) + '\n')
    file.close()
    
