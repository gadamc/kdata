from .info import __version_info__, __version__
from ROOT import *

kdatalibs = ['libkds', 'libkpta', 'libksamba', 'libkamping', 'libEra', 'libkera', 'libkqpa']
for lib in kdatalibs:
  try:
    #print 'load', lib
    gSystem.Load(lib)
  except: 
    #print 'load failed'
    pass

import libPyROOT as _libpyroot

def _KEvent_iter__(self):
  i = 0
  while self.GetEntry(i):
    yield self.GetEvent()                   
    i += 1

#print 'adding iterator'
_libpyroot.MakeRootClass( "KDataReader" ).__iter__    = _KEvent_iter__
