from ROOT import gSystem
import libPyROOT as _root

def _KEvent_iter__(self):
  i = 0
  while self.GetEntry(i):
    yield self.GetEvent()                   
    i += 1
      
gSystem.Load("libkds")
_root.MakeRootClass( "KDataReader" ).__iter__    = _KEvent_iter__
