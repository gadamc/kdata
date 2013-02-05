from ROOT import gSystem

__all__ = ['util', 'database']

try:
  gSystem.Load('libkds')
  gSystem.Load('libkpta')
  gSystem.Load('libkamping')

except Exception as e: 
  print "failed to load KDataPy. Couldn't load one of the libraries: libkds, libkpta or libkamping"
  raise e


#add various iterators to KData objects

import libPyROOT as _libpyroot

def _KEvent_iter__(self):
  i = 0
  while self.GetEntry(i):
    yield self.GetEvent()                   
    i += 1
_libpyroot.MakeRootClass( "KDataReader" ).__iter__    = _KEvent_iter__


def _KRawBoloPulseRecord_getNumpArray__(self):
  return np.array(self.GetTrace())
_libpyroot.MakeRootClass( "KRawBoloPulseRecord" ).getnumpy    = _KRawBoloPulseRecord_getNumpArray__


def _KptaProc_input_iter__(self):
  i = 0
  while i < self.GetInputPulseSize():
    yield self.GetInputPulse()[i]                 
    i += 1
_libpyroot.MakeRootClass( "KPtaProcessor" ).input    = property(_KptaProc_input_iter__)



def _KptaProc_output_iter__(self):
  i = 0
  while i < self.GetOutputPulseSize():
    yield self.GetOutputPulse()[i]                  
    i += 1
_libpyroot.MakeRootClass( "KPtaProcessor" ).output    = property(_KptaProc_output_iter__)


def _KptaProc_input_iter_index__(self):
  i = 0
  while i < self.GetInputPulseSize():
    yield self.GetInputPulse()[i],i                 
    i += 1
_libpyroot.MakeRootClass( "KPtaProcessor" ).input_index    = property(_KptaProc_input_iter_index__)



def _KptaProc_output_iter_index__(self):
  i = 0
  while i < self.GetOutputPulseSize():
    yield self.GetOutputPulse()[i],i                
    i += 1
_libpyroot.MakeRootClass( "KPtaProcessor" ).output_index    = property(_KptaProc_output_iter_index__)

