import numpy as np
from ROOT import *
import KDataPy
import matplotlib.pyplot as plt
import copy
import math

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
  :param match: if match is True, then the detector name needs to exactly match 'name'. Otherwise, if the pulse channel name contains 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
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

    util.looppulse('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'ZM1', pta = chain, analysisFunction = myAnalysis, myExtraOption="printMin")
      
  
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
  :param match: if match is True, then the bolometer name needs to exactly match 'name'. Otherwise, if the bolometer name contains 'name', then the data will be analyzed. For example, if you set 'name'='FID', then your analysis function will be called for each FID detector in the file
  :type match: bool
  :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function will be passed the following arguments: KBolometerRecord, **kwargs. 
  :type analysisFunction: function pointer
  :param maxEvents: if maxEvents != None, stops looping after maxEvents events. 
  :type match: int
  :param kwargs: all remaining kwargs are pass to the analysisFunction
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
    
 
  Besides strings, you can pass ANYTHING via the kwargs, in any order, even objects, and pointers to other functions!

  .. code-block:: python

    def callMe():
      print 'I can be called'


    def myFunction(boloRecord, **kwargs):
      print boloRecord.GetDetectorName()
      
      if kwargs['firstCall']:
        kwargs['firstCall']()

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

    util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, heatAna = chainHeat, ionAna = chainIon, firstCall = callMe)



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
    nice event viewer just to look at the data.
    
    :param data: The data file to use.
    :type data: string path to file or KDataReader object
    :param name: The channel name you want to analyze
    :type name: string
    :param match: if match is True, then the pulse channel name needs to exactly match 'name'. Otherwise, if the pulse channel name contains 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
    :type match: bool
    :param pta: a KPtaProcessor that will be automatically called for each pulse
    :type pta: KPtaProcessor object
    :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KRawBoloPulseRecord, KPtaProcessor, **kwargs). The KPtaProcessor will be the same object that you pass into pta
    :type analysisFunction: function pointer
    :param maxEvents: if maxEvents != None, stops looping after maxEvents events. 
    :type match: int
    :param kwargs: all kwargs are pass to the analysisFunction
    :type kwargs: keyword argument list


    Example Event Viewer. This will just print to screen every pulse shape for any channel with "ZM" in its name

    .. code-block:: python

      from KDataPy import util

      util.plotpulse('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'ZM')


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
    
        
        

def concatKdataFiles(fileList, outputFileName):
  '''
    This function performs a very simple operation. Given a list of KData file names (full path), fileList, it will merge the TTrees
    within these files into a single output file with name "outputFileName". This function should work for all types of KData files (Raw, Amp, HLA)
    
    :param fileList: The KData files to merge
    :type fileList: list of full paths to the files
    :param outputFileName: The name of the output KData file
    :type name: string

    [Developer's note: This assumes that the name of the TTree within the KData file is called 't']
  '''


  listOfTFiles = []
  listOfTrees = TList()

  print 'attempting to merge the following files'
  for fname in fileList:
    print fname
    f = TFile(fname)
    listOfTFiles.append(f)
    if f.Get('t') == 0:
      print 'doesnt appear to be a kdata file. no tree with name "t" found. quiting.'
      return
    listOfTrees.Add(f.Get('t'))
  
  print 'output file:', outputFileName
  fout = TFile(outputFileName, 'recreate')
  print 'starting the merge...'
  outTree = TTree.MergeTrees(listOfTrees, 'fast')
  outTree.Write()
  fout.Close()
  print 'done'


  
# format value
def formatvalue(value):
  if (isinstance(value,str)):
    # #see if this string is really an int or a float
    if value.isdigit()==True: #int
      return int(value)
    else: #try a float
      try:
        if math.isnan(float(value))==False:
          return float(value)
      except:
        pass

    return value.strip('" ') #strip off any quotations and extra spaces found in the value
  else:
    return value


#splitargs
def splitargs(*argv):
  '''
  A simple arg parser. This is useful because argparse is not installed on some Python systems with version < 2.7.
  This supports two kinds of arguments.
  1. a single value
  2. a key=value pair. 

  For example, this works
  "option1 option2  keyword1=value1 keyword2=value2 "

  But this little function doesn't support "switches" that have values like "-k 5" 
  
  It is highly recommended that you test this function before completely trusting what it does.

  It returns a tuple of ( [args], {kwargs} ).  In the example above, for example it would return

  ( ['option1', 'option2'], {'keyword1':'value1', 'keyword2':'value2'})
  '''
  
  myargs = []
  mykwargs = {}
  for arg in argv:
    if string.find(arg, '=') == -1:
      print arg
      myargs.append(arg)
    else:
      key, val = tuple(arg.split('='))
      
      if val  == 'True':
        val = True
      elif val == 'False':
        val = False
      else:
        val = formatvalue(val)
     
      print key,val
      mykwargs[key] = val

  return (myargs, mykwargs)
