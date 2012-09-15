import numpy as np
from ROOT import *
import KDataPy
import matplotlib.pyplot as plt
import copy

plt.ion()

try:
  gSystem.Load('libkds')
  gSystem.Load('libkpta')
  gSystem.Load('libkamping')
 
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
    


def looppulse(data, name=None, match=False, pta=None, analysisFunction = None, **kwargs):
  '''
  This function provides you with automatic looping code and gives you the chance for pulse processing. 
  This function will loop through a datafile, apply the KPulseProcessor that you provide (in pta), 
  and then call the analysisFunction, passing in the pulseRecord, the pta, and all **kwargs. 
  
  data = the .root data file, OR an instance of a KDataReader object
  name = plot pulses where 'name' is found in a pulse's channel name. You can use this to only analyze particular pulses
  match = if match is True, then only plot pulses that have a channel name that exactly matches 'name'
  pta = if you pass in a KPtaProcessor, the pulse is processed by the KPtaProcessor 
  analysisFunction = is a function that you provide to do extra analysis on each pulseRecord. The analysisFunction
  takes three arguments: the pulseRecord, the pta object and the **kwargs
    
  
  Example 1
  
  .. code-block:: python

     from KDataPy import util
     from ROOT import *
     gSystem.Load('libkpta')
  
     #here i define the analysis function
     def myFunction(pulseRecord, ptaObject=None, **kwargs):
       print pulseRecord.GetChannelName()
    
       if ptaObject != None
         pulsetrace = util.get_out(pta)
       else: pulsetrace = np.array(pulse.GetTrace())

       print 'the pulse maximum is', np.amax(pulsetrace), 'at bin', np.argmax(pulsetrace)
  
     #a KPtaProcessor object that will analyse each pulse
     bas = KBaselineRemoval()
  
     util.looppulse('path/to/kdatafile.root', name='ZM1', pta = bas, analysisFunction = myFunction)
  
  
  Example 2
  
  .. code-block:: python
    
    from KDataPy import util
    from ROOT import *
    gSystem.Load('libkpta')
  
    #here i define the analysis function
    def myFunction(pulseRecord, ptaObject=None, **kwargs):
      print pulseRecord.GetChannelName()
    
      if ptaObject != None
        pulsetrace = util.get_out(pta)
      else: pulsetrace = np.array(pulse.GetTrace())

      print 'the pulse maximum is', np.amax(pulsetrace), 'at bin', np.argmax(pulsetrace)
    
    bas = KBaselineRemoval()
    pat = KPatternRemoval()
    chain = KPulseAnalysisChain()
    chain.AddProcessor(bas)
    chain.AddProcessor(pat)
  
    util.looppulse('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'ZM1', pta = chain, analysisFunction = myFunction)
  
  
  Example 3 - using kwargs
  
  .. code-block:: python


    from KDataPy import util
    import numpy as np
    from ROOT import *
    gSystem.Load('libkpta')

    
    def myAnalysis(pulse, pta=None, **kwargs):
      if pta != None
        pulsetrace = util.get_out(pta)
      else: pulsetrace = np.array(pulse.GetTrace())
      
      if kwargs['myExtraOption'] == 'printMin':
        print 'the pulse minimum is', np.amin(pulsetrace), 'at bin', np.argmin(pulsetrace)
      else:
        print 'the pulse maximum is', np.amax(pulsetrace), 'at bin', np.argmax(pulsetrace)
        
    bas = KBaselineRemoval()
    pat = KPatternRemoval()
    filter = KIIRSecondOrder(a1, a2, b0, b1, b2)  #the "a" and "b" variables can be calculated by external tools (such as Scipy.signal)
    chain = KPulseAnalysisChain()
    chain.AddProcessor(bas)
    chain.AddProcessor(pat)
    chain.AddProcessor(filter)

    util.plotpulse('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'ZM1', pta = chain, analysisFunction = myAnalysis, myExtraOption="printMin")
      
  
  '''
  if 'KDataReader' in str(type(data)): kdfilereader = data
  else:
    kdfilereader = KDataPy.KDataReader(data)
    
  for event in kdfilereader:
    for pulse in event.boloPulseRecords():
      
      if pulse.GetPulseLength() == 0: continue
      if name:
        if name not in pulse.GetChannelName(): continue
        if match==True and name != pulse.GetChannelName(): continue
      
      if pta:
        pta.SetInputPulse( pulse.GetTrace() )
        pta.RunProcess()
        
      if analysisFunction:
        analysisFunction(pulse, pta, **kwargs)

   
def loopbolo(data, name=None, match=False, analysisFunction = None, **kwargs):
  '''
  Like looppulse, but just loops through each bolo record for you
  and you provide the analysis function. 
  "name" should be the name of the bolometer.
  if match == True, then only an exact match is passed to the analysisFunction
  The analysisFunction must take one argument, which is of type KBolometerRecord (or subclass), plus optional kwargs
  
  For example
  
  .. code-block:: python

    def myFunction(boloRecord, **kwargs):
      print boloRecord.GetDetectorName()
      
      for pulse in boloRecord.pulseRecords():
        print pulse.GetChannelName()
        .... do some analysis....
        
        
    util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, myExtraOptions="condition1")
    
  will loop through the file and return to you any bolometer with a GetDetectorName that
  contains "FID" and pass that bolo record to your myFunction(bolo).
  
  '''
  if analysisFunction==None:
    print 'You must provide an analysis function'
    return
    
  if 'KDataReader' in str(type(data)): kdfilereader = data
  else:
    kdfilereader = KDataPy.KDataReader(data)
    
  for event in kdfilereader:
    for bolo in event.boloRecords():
      
      if name:
        if name not in bolo.GetDetectorName(): continue
        if match==True and name != bolo.GetChannelName(): continue
        
      analysisFunction(bolo, **kwargs)

            
def plotpulse(data, name=None, match=False, pta = None, analysisFunction = None, **kwargs):
    '''
    Like looppulse, but will plot each pulse on screen and wait for you to hit the 'Enter'
    key in the shell before continuing. This is a nice way to visualize what is happening
    to the pulses. Additionally, if you don't provide any analysisFunction, it will be a
    nice event viewere just to look at the data.
    
    '''
        
    def plotingfunction(pulse, pta=None, **kwargs):
            
      
      print 'plotting', pulse.GetChannelName()
      if pta != None:
        pta.SetInputPulse( pulse.GetTrace() )
        pta.RunProcess()
        plt.plot( get_out(pta) )
      else:
        plt.plot( np.array(pulse.GetTrace()) )
        
      if kwargs['__callersAnalysisFunction']:
        newkwargs = copy.deepcopy(kwargs)
        del newkwargs['__callersAnalysisFunction']
        kwargs['__callersAnalysisFunction'](pulse,pta=pta, **newkwargs)
      
      try:
        if raw_input() == 'quit': 
          plt.cla()
          raise KeyboardInterrupt
      except KeyboardInterrupt: 
        plt.cla()
        raise KeyboardInterrupt
      
      plt.cla()
      
      
    looppulse(data, name=name, match=match, pta = pta, analysisFunction = plotingfunction, __callersAnalysisFunction = analysisFunction, **kwargs)  
    
        
        
            
        

  
  

