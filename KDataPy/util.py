import numpy as np
from ROOT import *

try:
  gSystem.Load('libkpta')
  #small utility functions
  def get_as_nparray(c_pointer, size):
    data = np.zeros(size)
    for i in range(size):
      data[i] = c_pointer[i]
    return data

  def get_out(pta):
    return get_as_nparray(pta.GetOutputPulse(), pta.GetOutputPulseSize())

  def get_in(pta):
    return get_as_nparray(pta.GetInputPulse(), pta.GetInputPulseSize())
    
except: pass

