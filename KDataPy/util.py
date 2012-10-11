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


#add various iterators to KData objects

import libPyROOT as _libpyroot

def _KEvent_iter__(self):
  i = 0
  while self.GetEntry(i):
    yield self.GetEvent()                   
    i += 1
_libpyroot.MakeRootClass( "KDataReader" ).__iter__    = _KEvent_iter__


def _KRawBoloPulseRecord_GetTrace_iter__(self):
  i = 0
  while i < self.GetPulseLength():
    yield self.GetTrace()[i]                 
    i += 1
_libpyroot.MakeRootClass( "KRawBoloPulseRecord" ).__iter__    = _KRawBoloPulseRecord_GetTrace_iter__


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
    
def getRootHistFromOutput(pta):
  '''
  
  return a ROOT TH1D histogram from the output pulse of a KPtaProcessor (pta)
  
  '''
  h = TH1D(pta.GetName(), pta.GetName(), pta.GetOutputPulseSize(), 0, pta.GetOutputPulseSize())
  for val,i in pta.output_index: h.SetBinContent(i+1, val)
  return h


def looppulse(data, name=None, match=False, pta=None, analysisFunction = None, maxEvents=None, **kwargs):
  '''
  
  This function provides you with automatic looping code and gives you the chance for pulse processing. 
  This function will loop through a datafile, apply the KPulseProcessor that you provide (in pta), 
  and then call the analysisFunction, passing in the pulseRecord, the pta, and all **kwargs. 

  :param data: The data file to use.
  :type data: string path to file or KDataReader object
  :param name: The channel name you want to analyze
  :type name: string
  :param match: if match is True, then the pulse channel name needs to exactly match 'name'. Otherwise, if the pulse channel name starts with 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
  :type match: bool
  :param pta: a KPtaProcessor that will be automatically called for each pulse
  :type pta: KPtaProcessor object
  :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KRawBoloPulseRecord, KPtaProcessor, **kwargs). The KPtaProcessor will be the same object that you pass into pta
  :type analysisFunction: function pointer
  :param maxEvents: if maxEvents != None, stops looping after maxEvents events. 
  :type match: int
  :param kwargs: all kwargs are pass to the analysisFunction
  :type kwargs: keyword argument list


    
  
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

    if maxEvents and kdfilereader.GetCurrentEntryNumber() >= maxEvents: return None
   
def loopbolo(data, name=None, match=False, analysisFunction = None, maxEvents=None, **kwargs):
  '''
  Like looppulse, but just loops through each bolo record for you
  and you provide the analysis function. 
  
  :param data: The data file to use.
  :type data: string path to file or KDataReader object
  :param name: The bolometer name you want to analyze
  :type name: string
  :param match: if match is True, then the bolometer name needs to exactly match 'name'. Otherwise, if the pulse channel name starts with 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
  :type match: bool
  :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KRawPulseRecord, **kwargs). 
  :type analysisFunction: function pointer
  :param maxEvents: if maxEvents != None, stops looping after maxEvents events. 
  :type match: int
  :param kwargs: all kwargs are pass to the analysisFunction
  :type kwargs: keyword argument list

  For example
  
  .. code-block:: python

    def myFunction(boloRecord, **kwargs):
      print boloRecord.GetDetectorName()
      
      for pulse in boloRecord.pulseRecords():
        print pulse.GetChannelName()
          if kwargs['myExtraOptions'] == 'condition1':
            .... do some analysis....
          else:
            .... do something different....
        
        
    util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, myExtraOptions="condition1")
    
 
  Besides strings, you can pass in ANYTHING via the kwargs, even objects

  .. code-block:: python

    def myFunction(boloRecord, **kwargs):
      print boloRecord.GetDetectorName()
      
      hAna = kwargs['heatAna']
      ionAna = kwargs['ionAna']

      for pulse in boloRecord.pulseRecords():
        print pulse.GetChannelName()

        #pick the correct analysis object
        if pulse.GetIsHeatPulse():
          theAna = hAna
        else:
          theAna = ionAna

        theAna.SetInputPulse(pulse.GetTrace())
        theAna.RunProcess()
        outTrace = KDataPy.util.get_out(theAna)
          ... do something ...


        
    chainHeat = KPulseAnalysisChain()
    chainHeat.AddProcessor( KBaselineRemoval() )

    chainIon = KPulseAnalysisChain()
    chainIon.AddProcessor( KBaselineRemoval() )
    chainIon.AddProcessor( KPatternRemoval() )

    util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, heatAna = chainHeat, ionAna = chainIon)



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
    if maxEvents and kdfilereader.GetCurrentEntryNumber() >= maxEvents: return None

            
def plotpulse(data, name=None, match=False, pta = None, analysisFunction = None, maxEvents=None, **kwargs):
    '''
    Exactly like looppulse, but will plot each pulse on screen and wait for you to hit the 'Enter'
    key in the shell before continuing. In fact, this function calls looppulse. This is a nice way to visualize what is happening
    to the pulses. Additionally, if you don't provide any analysisFunction, it will be a
    nice event viewere just to look at the data.
    
    :param data: The data file to use.
    :type data: string path to file or KDataReader object
    :param name: The channel name you want to analyze
    :type name: string
    :param match: if match is True, then the pulse channel name needs to exactly match 'name'. Otherwise, if the pulse channel name starts with 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
    :type match: bool
    :param pta: a KPtaProcessor that will be automatically called for each pulse
    :type pta: KPtaProcessor object
    :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KRawBoloPulseRecord, KPtaProcessor, **kwargs). The KPtaProcessor will be the same object that you pass into pta
    :type analysisFunction: function pointer
    :param maxEvents: if maxEvents != None, stops looping after maxEvents events. 
    :type match: int
    :param kwargs: all kwargs are pass to the analysisFunction
    :type kwargs: keyword argument list

    '''
        
    #hey - maybe i should use a decorator to "subclass" the looppulse function!.  how do i do that? 
    ##. this is much more robust than using __callersAnalysisFunction. perhaps....
    #

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
    
        
        
            
        

  
  

