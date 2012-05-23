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
  '''
  return a numpy array from a pointer to data of length 'size'
  '''
  data = np.zeros(size)
  for i in range(size):
    data[i] = c_pointer[i]
  return data

def get_out(pta):
  '''
  return a numpy array from the output pulse of a KPtaProcessor (pta)
  '''
  return get_as_nparray(pta.GetOutputPulse(), pta.GetOutputPulseSize())

def get_in(pta):
  '''
  return a numpy array from the input pulse of a KPtaProcessor (pta)
  '''
  return get_as_nparray(pta.GetInputPulse(), pta.GetInputPulseSize())
    
def plotpulse(data, name=None, match=False, pta = None):
    '''
    This function provides you with a simple single-plot viewer and some automatic pulse processing. It will loop through a data
    file and can plot each pulse that you specify. Additionally, it can process each pulse with a KPtaProcessor, plotting
    the output of the processor. 
    
    data = the .root data file, OR an instance of a KDataReader object
    name = plot pulses where 'name' is found in a pulse's channel name. 
    match = if match is True, then only plot pulses that have a channel name that exactly matches 'name'
    pta = if you pass in a KPtaProcessor, the pulse is processed by the KPtaProcessor and output of that processor is plotted to screen.
    
    Example.
    
    from KDataPy import util
    
    bas = KBaselineRemoval()
    pat = KPatternRemoval()
    chain = KPulseAnalysisChain()
    chain.AddProcessor(bas)
    chain.AddProcessor(pat)
    
    util.plotpulse('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'ZM1', pta = chain)
    
    
    '''
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
        

  
  

