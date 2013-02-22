KDataPy Utility functions
=========================

This module extends the KDataReader class, the KRawBoloPulseRecords class and the family of KPtaProcessor classes.  It add iteration, provides easy functions to create numpy arrays from KPtaProcessors, and provides event looping functions, allowing you to hook in an analysis function to the loop. This way you don't have to write those chunks of code ever again.

The KDataReader class is now iterable and returns a pointer to the KEvent class (KRawEvent, KAmpEvent, KHLAEvent).  The KRawBoloPulseRecord class has been extended in Python. The method getnumpy() was added, which returns a numpy array. An example is shown below.

.. code-block:: python

     import KDataPy.util
     import ROOT

     f = ROOT.KDataReader("path/to/file/ma22a000_000.root")
     
     for event in f:  #KDataReader is iterable, so you can write for loops like this

     	#do something with the event

Since TClonesArrays are also iterable objects, you can then write simple Pythonic loops

.. code-block:: python

     import KDataPy.util
     import ROOT

     f = ROOT.KDataReader("path/to/file/ma22a000_000.root")

     for event in f:
     	for bolo in event.boloRecords():
     	  for pulse in bolo.pulseRecords():
     
     		#do something with the data

     		#such, as get a numpy array of the pulse trace
     		np = pulse.getnumpy()

However, why write an event loop ever again? KDataPy.util already does this for you and gives you an easy
way to hook in an analysis function for each pulse or bolometer record.  See the looppulse and loopbolo functions below!

Another useful iteration that has been added is for the KPtaProcessor-based classes (KBaselineRemoval, KIIRFilter, KConvolution, KPulseAnalysisChain, etc....). One can iterate over the input and output pulses for each object.

Here's how you would use the iteratble KPtaProcessor.output and KPtaProcessor.input functions

.. code-block:: python

     import KDataPy.util
     import ROOT

     #create some dummy pulse for this example
     bas = ROOT.KBaselineRemoval()
     vp = ROOT.std.vector("double")()
     for i in range(1000): vp.push_back(i)
     bas.SetInputPulse(vp)
     bas.RunProcess()
     #


     #The OLD way to loop through the output pulse
     for ii in range(bas.GetOutputPulseSize()):
     	print bas.GetOutputPulse()[i]

     #The Python way
     for val in bas.output:
       print val

     for val in bas.input:
       print val

     #Also, you can get the index as well
     for val, i in bas.output_index:
       print i, val

     for val, i in bas.input_index:
       print i, val


When loading this module, the KPulseAnalysisChain also becomes iterable and returns a pointer to each KPtaProcessor in its ordered list. 

.. code-block:: python

     import KDataPy.util
     import ROOT

     #add processors to the chain
     chain = ROOT.KPulseAnalysisChain()
     chain.AddProcessor( ROOT.KBaselineRemoval() )
     chain.AddProcessor( ROOT.KLinearRemoval() )
     chain.AddProcessor( ROOT.KPatternRemoval() )

     #now pythonic iterable.
     for processor in chain:
       print processor.GetName()



.. automodule:: KDataPy.util
   :members:
