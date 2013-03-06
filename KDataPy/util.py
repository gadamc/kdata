# -*- coding: utf-8 -*-
import numpy
import KDataPy
import matplotlib.pyplot
import copy
import math
import KDataPy.exceptions
import ROOT
matplotlib.pyplot.ion()

try:
  ROOT.gSystem.Load('libkds')
  ROOT.gSystem.Load('libkpta')
  ROOT.gSystem.Load('libkamping')

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
  return numpy.array(self.GetTrace())
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


def _KPulseAnalysisChain_iter__(self):
  i = 0
  while i < self.GetNumProcessors():
    yield self.GetProcessor()                   
_libpyroot.MakeRootClass( "KPulseAnalysisChain" ).__iter__    = _KPulseAnalysisChain_iter__


class KDataUtilQuitLoop(Exception):
  def __init__(self, value):
    self.value = value 


#small utility functions  
def get_as_nparray(c_pointer, size):
  '''
  
  return a numpy array from a pointer to data of length 'size'
  
  '''
  data = numpy.zeros(size)
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
  h = ROOT.TH1D(pta.GetName(), pta.GetName(), pta.GetOutputPulseSize(), 0, pta.GetOutputPulseSize())
  for val,i in pta.output_index: h.SetBinContent(i+1, val)
  return h


def looppulse(data, name=None, match=False, pta=None, analysisFunction = None,  **kwargs):
  '''
  This function provides you with automatic looping code and gives you the chance for pulse processing. 
  This function will loop through a datafile, apply the KPulseProcessor that you provide (in pta), 
  and then call the analysisFunction, passing in the pulseRecord, the pta, and all kwargs. 
  This function allows for all KData types (Raw, Amp, HLA). Only events in Raw data files are processed with the
  KPtaProcessor object, of course.

  :param data: The data file to use.
  :type data: string path to file or KDataReader object
  :param name: The channel name you want to analyze
  :type name: string
  :param match: if match is True, then the detector name needs to exactly match 'name'. Otherwise, if the pulse channel name contains 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
  :type match: bool
  :param pta: a KPtaProcessor that will be automatically called for each pulse
  :type pta: KPtaProcessor object
  :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KEvent, KBoloPulseRecord, KPtaProcessor, kwargs). The KPtaProcessor will be the same object that you pass into pta
  :type analysisFunction: function pointer
  :param kwargs: all kwargs are pass to the analysisFunction
  :type kwargs: keyword argument list


    
  
  Example 1
  
  .. code-block:: python

     from KDataPy import util
     from ROOT import *
     gSystem.Load('libkpta')
  
     #here i define the analysis function
     def myFunction(theEvent, pulseRecord, ptaObject=None, **kwargs):
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
    def myFunction(theEvent, pulseRecord, ptaObject=None, **kwargs):
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

    
    def myAnalysis(theEvent, pulse, pta, **kwargs):
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
    kdfilereader = ROOT.KDataReader(data)
    
  for event in kdfilereader:
    for pulse in event.boloPulseRecords():
      
      if pulse.GetPulseLength() == 0: continue
      if name:
        if name not in pulse.GetChannelName(): continue
        if match==True and name != pulse.GetChannelName(): continue
      
      try:
        if pta:
          pta.SetInputPulse( pulse.GetTrace() )
          pta.RunProcess()
      except AttributeError:
        pass #ignore. probably because the pulse.GetTrace method doesn't exist because this file is not a raw level kdata file

      if analysisFunction:
        try:
          analysisFunction(event, pulse, pta, **kwargs)
        except KDataUtilQuitLoop:
          return
   
def loopbolo(data, name=None, match=False, ptaDictionary = None, analysisFunction = None,  **kwargs):
  '''
  Like looppulse, but just loops through each bolo record for you
  and you provide the analysis function. However, you can also supply a dictionary object that has the pulse
  channel names for keys and KPtaProcessor objects as values. Before your analysisFunction is called,
  these KPtaProcessors will be applied to the appropriate pulses. See the examples below.
  
  :param data: The data file to use.
  :type data:  string, or KDataReader object
  :param name: The bolometer name you want to analyze
  :type name: string
  :param match: if match is True, then the bolometer name needs to exactly match 'name'. Otherwise, if the bolometer name contains 'name', then the data will be analyzed. For example, if you set 'name'='FID', then your analysis function will be called for each FID detector in the file
  :type match: bool
  :param ptaDictionary: A dictionary of references to pta Objects. Each 'key' is the channel name and the value is the KPtaProcessor that will be applied to that channel's pulse before the analysisFunction is called.
  :type ptaDictionary: dict 
  :param analysisFunction: the function that you define to analyze the bolometer record. You must supply this callback function.  The function will be passed the following arguments: KEvent, KBolometerRecord, ptaDictionary, kwargs. 
  :type analysisFunction: function pointer
  :param kwargs: all remaining kwargs are pass to the analysisFunction
  :type kwargs: keyword argument list

  For example
  
  .. code-block:: python

    import KDataPy.util

    def myFunction(theEvent, boloRecord, ptaDict=None,  **kwargs):
      print boloRecord.GetDetectorName()
      
      for pulse in boloRecord.pulseRecords():
        print pulse.GetChannelName()
          if kwargs['myExtraOptions'] == 'condition1':
            .... do some analysis....
          else:
            .... do something different....
        
        
    KDataPy.util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, myExtraOptions="condition1")
    
 
  Besides strings, you can pass ANYTHING via the kwargs, in any order, even objects, and pointers to other functions.

  .. code-block:: python

    import KDataPy.util
    from ROOT import *

    def callMe():
      print 'I can be called'


    def myFunction(theEvent, boloRecord, ptaDict=None, **kwargs):
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
        outTrace = util.get_out(theAna)
          ... do something ...


        
    chainHeat = KPulseAnalysisChain()
    chainHeat.AddProcessor( KBaselineRemoval() )

    chainIon = KPulseAnalysisChain()
    chainIon.AddProcessor( KBaselineRemoval() )
    chainIon.AddProcessor( KPatternRemoval() )

    util.loopbolo('/path/to/file.root', name="FID", analysisFunction=myFunction, heatAna = chainHeat, ionAna = chainIon, firstCall = callMe)

  This final example shows how to use the ptaDictionary, which should save you some lines of code in your anaysisFunction.

  .. code-block:: python

    from KDataPy import util
    import ROOT
    ROOT.gSystem.Load('libkds')
    ROOT.gSystem.Load('libkpta')

    def myFunction(theEvent, boloRecord, ptaDict=None, **kwargs):
      print boloRecord.GetDetectorName()

      for pulse in boloRecord.pulseRecords():
        
        outTrace = KDataPy.util.get_out( ptaDict[pulse.GetChannelName()])
          # do something 


    chainHeat =ROOT.KPulseAnalysisChain()
    chainHeat.AddProcessor(ROOT.KBaselineRemoval() )

    chainIon1 =ROOT.KPulseAnalysisChain()
    chainIon1.AddProcessor(ROOT.KBaselineRemoval() )
    chainIon1.AddProcessor(ROOT.KPatternRemoval() )

    chainIon2 =ROOT.KPulseAnalysisChain()
    chainIon2.AddProcessor(ROOT.KLinearRemoval() )
    chainIon.AddProcessor(ROOT.KPatternRemoval() )

    myPtaDict = {}
    myPtaDict['chalA FID807'] = chainHeat
    myPtaDict['chalB FID807'] = chainHeat

    myPtaDict['ionisA FID807'] = chainIon1
    myPtaDict['ionisB FID807'] = chainIon1
    myPtaDict['ionisC FID807'] = chainIon2
    myPtaDict['ionisD FID807'] = chainIon2


    util.loopbolo('/path/to/file.root', name="FID", ptaDictionary = myPtaDict, analysisFunction=myFunction)


  '''
  if analysisFunction==None:
    print 'You must provide an analysis function'
    return
    
  if 'KDataReader' in str(type(data)): kdfilereader = data
  else:
    kdfilereader = ROOT.KDataReader(data)
    
  for event in kdfilereader:
    for bolo in event.boloRecords():
      
      if name:
        if name not in bolo.GetDetectorName(): continue
        if match==True and name != bolo.GetChannelName(): continue
        
        if ptaDictionary:
          for pulse in bolo.pulseRecords():
            if pulse.GetPulseLength() == 0: continue  #skip empty pulses
            try:
              ptaDictionary[pulse.GetChannelName()].SetInputPulse(pulse.GetTrace())
              if ptaDictionary[pulse.GetChannelName()].RunProcess() is False:
                raise KDataPy.exceptions.KDataError('RunProcess failed in plotbolo')

            except KeyError as e:
              if e.message != pulse.GetChannelName(): 
                raise e

      try:
        analysisFunction(event, bolo, ptaDictionary, **kwargs)
      except KDataUtilQuitLoop:
          return
            
def plotpulse(data, name=None, match=False, pta = None, analysisFunction = None, **kwargs):
    '''
    Exactly like looppulse, but will plot each raw pulse on screen and wait for you to hit the 'Enter'
    key in the shell before continuing. In fact, this function calls looppulse. This is a nice way to visualize what is happening
    to the pulses. Additionally, if you don't provide any analysisFunction, it will be a
    nice event viewer just to look at the data.  This function will only work with KData Raw-level files. That is, its only
    meant for plotting the pulse traces, which only exist in the raw data files. Otherwise, this will raise an exception at run-time.
    This function plot to matplotlib figure(0)! So, don't use figure(0) or it will be cleared and a pulse will be drawn.

    You can quit the loop by typing 'quit', 'q', 'bye', or '-q' on the command-line

    :param data: The data file to use.
    :type data: string path to file or KDataReader object
    :param name: The channel name you want to analyze
    :type name: string
    :param match: if match is True, then the pulse channel name needs to exactly match 'name'. Otherwise, if the pulse channel name contains 'name', then the data will be analyzed. For example, if you set 'name'='chal', then you will probably analyze all heat channels
    :type match: bool
    :param pta: a KPtaProcessor that will be automatically called for each pulse
    :type pta: KPtaProcessor object
    :param analysisFunction: the function that you will define to analyze your pulse. This is a sort of 'callback' function. Your function must have three arguments(KEvent, KBoloPulseRecord, KPtaProcessor, kwargs). The KPtaProcessor will be the same object that you pass into pta
    :type analysisFunction: function pointer
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

    def __plotingfunction(event, pulse, pta=None, **kwargs):
            
      
      print 'plotting', pulse.GetChannelName()
      matplotlib.pyplot.figure(0)
      matplotlib.pyplot.cla()
      if pta != None:
        pta.SetInputPulse( pulse.GetTrace() )
        pta.RunProcess()
        dataPulse = get_out(pta)
        
      else:
        dataPulse = numpy.array(pulse.GetTrace())
        
        
      yMax = numpy.max(dataPulse)
      yMin = numpy.min(dataPulse)
      yMaxNew = abs(yMax - yMin)*0.05 + yMax
      yMinNew = -abs(yMax - yMin)*0.05 +yMin
      matplotlib.pyplot.plot(dataPulse)
      matplotlib.pyplot.ylim(yMinNew, yMaxNew)
      matplotlib.pyplot.title(pulse.GetChannelName())
        #matplotlib.pyplot.show()
        
      
        
      if kwargs['__callersAnalysisFunction']:
        newkwargs = copy.deepcopy(kwargs)
        del newkwargs['__callersAnalysisFunction']
        kwargs['__callersAnalysisFunction'](event, pulse, pta, **newkwargs)
      
      try:
        quitmessage = raw_input()
        if quitmessage in ['quit', 'q', 'bye', '-q']: 
          matplotlib.pyplot.cla()
          raise KDataUtilQuitLoop(quitmessage)
      except KeyboardInterrupt: 
        matplotlib.pyplot.cla()
        raise KeyboardInterrupt
      
      matplotlib.pyplot.cla()
      
      
    looppulse(data, name=name, match=match, pta = pta, analysisFunction = __plotingfunction, __callersAnalysisFunction = analysisFunction, **kwargs)  
    

def plotbolo(data, name=None, match=False, ptaDictionary = None, analysisFunction = None, **kwargs):
    '''
    Exactly like loopbolo, but will plot each raw pulse on screen and wait for you to hit the 'Enter'
    key in the shell before continuing. In fact, this function calls loopbolo. This is a nice way to visualize what is happening
    to the pulses. Additionally, if you don't provide any analysisFunction, it will be a
    nice event viewer just to look at the data.  This function will only work with KData Raw-level files. That is, its only
    meant for plotting the pulse traces, which only exist in the raw data files. Otherwise, this will raise an exception at run-time.
    This function plot to matplotlib figure(0)! So, don't use figure(0) or it will be cleared and a pulse will be drawn.

    You can quit the loop by typing 'quit', 'q', 'bye', or '-q' on the command-line

    :param data: The data file to use.
    :type data:  string, or KDataReader object
    :param name: The bolometer name you want to analyze
    :type name: string
    :param match: if match is True, then the bolometer name needs to exactly match 'name'. Otherwise, if the bolometer name contains 'name', then the data will be analyzed. For example, if you set 'name'='FID', then your analysis function will be called for each FID detector in the file
    :type match: bool
    :param ptaDictionary: A dictionary of references to pta Objects. Each 'key' is the channel name and the value is the KPtaProcessor that will be applied to that channel's pulse before the analysisFunction is called.
    :type ptaDictionary: dict 
    :param analysisFunction: the function that you define to analyze the bolometer record. You must supply this callback function.  The function will be passed the following arguments: KEvent, KBolometerRecord, ptaDictionary, axisDictionary, kwargs. The axisDictionary is a dictionary of matplotlib.pyplot.Axis objects that are created for each subplot in the figure. You can use these to alter how your plot looks
    :type analysisFunction: function pointer
    :param kwargs: all remaining kwargs are pass to the analysisFunction
    :type kwargs: keyword argument list

    Example Event Viewer. This will just print to screen all pulses for any detector with "FID" in its name

    .. code-block:: python

      from KDataPy import util

      util.plotbolo('/sps/edelweis/kdata/data/raw/me20a010_010.root', name = 'FID')

    '''
        
    #hey - maybe i should use a decorator to "subclass" the looppulse function!.  how do i do that? 
    ##. this is much more robust than using __callersAnalysisFunction. perhaps....
    #

    def __plotingfunction__(event, bolo, ptaDictionary=None, **kwargs):
            
      axisDict = {}
      
      print 'plotting', bolo.GetDetectorName()
      fig = matplotlib.pyplot.figure(0)
      fig.clf()

      counter = 1
      for pulse in bolo.pulseRecords():


        ax = matplotlib.pyplot.subplot(bolo.GetNumPulseRecords(), 1, counter)
        counter += 1
        matplotlib.pyplot.cla()
        if pulse.GetPulseLength() == 0: 
          continue

        axisDict[pulse.GetChannelName()] = ax
        ax.set_title(pulse.GetChannelName())
        

        if ptaDictionary:
          try:
            tobeplotted = get_out(ptaDictionary[pulse.GetChannelName()])

          except KeyError as e:
            if e.message != pulse.GetChannelName(): raise e
            tobeplotted = pulse.GetTrace()
        else:
          tobeplotted = pulse.GetTrace()

        yMax = numpy.max(tobeplotted)
        yMin = numpy.min(tobeplotted)
        yMaxNew = abs(yMax - yMin)*0.05 + yMax
        yMinNew = -abs(yMax - yMin)*0.05 +yMin
        matplotlib.pyplot.plot(tobeplotted )
        matplotlib.pyplot.ylim(yMinNew, yMaxNew)
        matplotlib.pyplot.title(pulse.GetChannelName())

      
        
      if kwargs['__callersAnalysisFunction__']:
        newkwargs = copy.deepcopy(kwargs)
        del newkwargs['__callersAnalysisFunction__']
        kwargs['__callersAnalysisFunction__'](event, bolo, ptaDictionary, axisDict, **newkwargs)
      
      try:
        quitmessage = raw_input()
        if quitmessage in ['quit', 'q', 'bye', '-q']: 
          matplotlib.pyplot.cla()
          raise KDataUtilQuitLoop(quitmessage)
      except KeyboardInterrupt: 
        matplotlib.pyplot.cla()
        raise KeyboardInterrupt
      
      matplotlib.pyplot.cla()
      
      
    loopbolo(data, name=name, match=match, ptaDictionary = ptaDictionary, analysisFunction = __plotingfunction__, __callersAnalysisFunction__ = analysisFunction, **kwargs)  
        
        

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
  listOfTrees = ROOT.TList()

  print 'attempting to merge the following files'
  for fname in fileList:
    print fname
    f = ROOT.TFile(fname)
    listOfTFiles.append(f)
    if f.Get('t') == 0:
      print 'doesnt appear to be a kdata file. no tree with name "t" found. quiting.'
      return
    listOfTrees.Add(f.Get('t'))
  
  print 'output file:', outputFileName
  fout = ROOT.TFile(outputFileName, 'recreate')
  print 'starting the merge...'
  outTree = ROOT.TTree.MergeTrees(listOfTrees, 'fast')
  outTree.Write()
  fout.Close()
  print 'done'


  


