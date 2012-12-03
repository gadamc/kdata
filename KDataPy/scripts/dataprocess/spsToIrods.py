#!/usr/bin/env python

import os, sys
import KDataPy.database as kdb


def getListOfFiles(**dbKwargs):
  #returns a list of lists, one for each SAMBA machine

  #will need to check only SAMBA runs that are older than the current one... 
  #or, how do we tell this program when the Run is complete? Fucking hell, I hate Samba. what a piece of shit software. 
  pass

def packageFiles(fileList):
  pass

def getFileName(fileList):
  #figures out how to name the tarred file
  pass

def putOnIrods(tarredFile):
  pass


if __name__ == '__main__':
  
  #parse arguements. 
  #options are:
  # database server
  # username / password
