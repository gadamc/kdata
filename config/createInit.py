#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os,sys

if __name__ == '__main__':

  modulelist = list()

  if len(sys.argv) < 3:
    sys.exit(1)
    
  for i in range(len(sys.argv)-1):
    if i != 0:
      modulelist.append(sys.argv[i])
    
  
  file = open(sys.argv[len(sys.argv)-1].rstrip('/') + '/__init__.py','w')
  
  myline = '__all__ = ' + str(modulelist)
  
  file.write(myline)