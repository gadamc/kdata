from .info import __version_info__, __version__
from ROOT import *

kdatalibs = ['libkds', 'libkpta', 'libksamba', 'libkamping', 'libEra', 'libkera', 'libkqpa']
for lib in kdatalibs:
  try:
    gSystem.Load(lib)
  except: 
    pass

import libPyROOT as _libpyroot

def _KEvent_iter__(self):
  i = 0
  while self.GetEntry(i):
    yield self.GetEvent()                   
    i += 1

_libpyroot.MakeRootClass( "KDataReader" ).__iter__    = _KEvent_iter__

__all__ = ['info', 'pulsetempy', 'util', 'database']