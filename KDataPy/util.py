import numpy as np
from ROOT import *
import KDataPy
import matplotlib.pyplot as plt
plt.ion()

try:
  gSystem.Load('libkpta')
  gSystem.Load('libkds')
 
except Exception as e: 
  print 'failed to load KDataPy.util'
  raise e

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
    
def plotpulse(data, name=None, match=False, pta = None):
    
    if 'KDataReader' in str(type(data)): kdfilereader = data
    else:
      kdfilereader = KDataPy.KDataReader(data)
      
    print 'type "quit" at any time to exit'
    for event in kdfilereader:
      for pulse in event.boloPulseRecords():
        
        if pulse.GetPulseLength() == 0: continue
        if name not in pulse.GetChannelName(): continue
        if match==True and name != pulse.GetChannelName(): continue
        
        print 'plotting', pulse.GetChannelName()
        if pta != None:
          pta.SetInputPulse( pulse.GetTrace() )
          pta.RunProcess()
          plt.plot( get_out(pta) )
        else:
          plt.plot( np.array(pulse.GetTrace()) )
        
        try:
          if raw_input() == 'quit': return
        except KeyboardInterrupt: return
        except: pass
        
        plt.cla()    
        

  
  

