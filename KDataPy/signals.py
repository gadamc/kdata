'''
  This module defines classes to generate Heat and BBv2 Ionization signals 
  (BBv1 pulses could be easily added if needed) They can return the value 
  at a particular time, or they may be used to generate a list or numpy array
  of numerical values. Additionally, this module defines two classes to produce
  noise. The GaussianNoise class produces white gaussian noise about zero and 
  of a particular width. While, the ArbitraryNoise class will randomly generare 
  a noise pulse based on an input noise power spectrum.  (For technical 
  reasons ArbitraryNoise doesn't inherit directly from Signal, but it does 
  behave similarly.)   These classes are iterable and can be used directly 
  with Numpy and  MatPlotLib. That is, you can called

  .. code-block:: python

    import KDataPy.signals
    heat = KDataPy.signals.HeatSignal()
    #by default, parameter[1] is -1, which is fine for
    #templates, but should be larger to see 
    #above typical noise levels
    heat.setpar(1,-1500) 

    #use iteration to make a list
    pulse = []
    for val in heat:
      pulse.append(val)

    #and get a numpy array

    pulse = heat()  

    #and plot with MatPlotLib

    matplotlib.pyplot.plot(heat)


  These classes have been optimized for speed (in case you repeatedly want
  to generate the same pulse), by using a standard Python 'memoization' 
  decorator cache. The values are cached the first time they are calculated 
  so that they are not calculated again. 

  This means that the second time you call 

  .. code-block:: python

    pulse = heat()

  the values are just retrieved from the cache. (However, they are repacked 
  into a numpy array, which takes time.)  

  If you change the value of the parameters, however, the pulse will be 
  recalculated.

  Additionally, you can use the object call to return just a single 
  value of the signal. That is, 

  .. code-block:: python

    heat[10]  #or heat(10)

  returns the heat at index = 10. 

  If you want the heat at time = X

  .. code-block:: python

    heat[ X/heat.time_per_index ]


  Signals can also be added, subtraced and multipled with the '+' , '-' 
  and '*' operators. These operations return a numpy array. 

  .. code-block:: python

    #instantiate a GaussianNoise 
    #object of width = 5
    noise = KDataPy.signals.GaussianNoise(5)  

    signal = noise + heat
    plt.plot(signal)

  Finally, the ArbitraryNoise class will randomly generate a noise pulse
  given an input power spectrum. 

  Given 'noise_power' is a list-like object that holds the average power 
  spectrum for a particular channel. Then you would use it to 
  instantiate an ArbitraryNoise object.

  .. code-block:: python

    noise = KDataPy.signals.ArbitraryNoise( noise_power )

    plt.plot(heat + noise)

  You can change the noise_power simply by calling

  .. code-block:: python

    noise.noise_power = another_noise_power
    noise.generate()


  Developer's note: This whole module should probably be rewritten such that everything inherits from numpy.ndarray. Then 
  you get all of the numpy stuff for free.... :P  Don't ask why I didn't do this in the first place. 

'''

import functools
import collections
import math
import numpy
import random

class memoized(object):
  '''Decorator. Caches a function's return value each time it is called.
  If called later with the same arguments, the cached value is returned
  (not reevaluated).
  '''
  def __init__(self, func):
    self.func = func
    self.cache = {}
  def __call__(self, *args):
    if not isinstance(args, collections.Hashable):
      # uncacheable. a list, for instance.
      # better to not cache than blow up.
      return self.func(*args)
    if args in self.cache:
      return self.cache[args]
    else:
      value = self.func(*args)
      self.cache[args] = value
      return value
  def __repr__(self):
    '''Return the function's docstring.'''
    return self.func.__doc__
  def __get__(self, obj, objtype):
    """Support instance methods."""
    fn = functools.partial(self.__call__, obj)
    fn.reset = self._reset
    return fn
  def _reset(self):
    self.cache = {}



class Signal(object):

  '''
    Base class for Signals (HeatSignal and BBv2IonSignal)
    You shouldn't instantiate this class directly.

    Use setpar and setparameters instead of setting the values 
    in the _parameters directly

  '''

  def __init__(self, length = 8192, time_per_index = 1.0, parameters = None):

    self.length = length
    self.time_per_index = time_per_index
    self._parameters = parameters


  def _val(self, time):
    if isinstance(time, int) is False and isinstance(time, float) is False and isinstance(time, long) is False:
      raise TypeError('input must be a number')
    if time < 0 or time/self.time_per_index > self.length:
      raise IndexError('out of range')


  def __iter__(self):
    i = 0
    while i < self.length:
      yield self._val(i * self.time_per_index)                   
      i += 1


  def __array__(self):
    return numpy.array([v for v in self])


  def setpar(self, index, value):
    '''
      I couldn't figure out how to use the __setattr__ when trying 
      to set a single element of the _parameters list. (self._parameters 
      calls __getattr__ ) Instead, I've written this simple method 
      (and setparameters) that you should use to set a parameters list values. 

      When you use this method and setparameters, the internal cache
      will be reset and the pulse will be calculated.

      calling 
      self._parameters[index] = value
      is discouraged because you'll have to remember to call self._reset()

    '''
    if self._parameters[index] != value:
      self._parameters[index] =  value
      self._reset()

  def setparameters(self, paramlist):

    if paramlist == self._parameters is False: #check to see if something is different.
      self._parameters = paramlist
      self._reset()

  # def __setattr__(self, name, value):
  #   print 'signal set', name, value
  #   if name == 'parameters':
  #     self._reset()
  #   object.__setattr__(self, name, value)

  # def __getattribute__(self, name):
  #   print 'signal get', name
  #   if name == 'parameters':
  #     self._reset()
  #   return object.__getattribute__(self, name)

  def __add__(self, rhs):
    try:
      return self.__array__() + rhs.__array__()
    except:
      return NotImplemented

  def __sub__(self, rhs):
    try:
      return self.__array__() - rhs.__array__()
    except:
      return NotImplemented

  def __mul__(self, rhs):
    try:
      return self.__array__() * rhs.__array__()
    except:
      return NotImplemented

  def __getitem__(self, index = None):
    if index is None:
      return self.__array__()
    
    return self._val(index * self.time_per_index)

  def __call__(self, index = None):
    if index is None:
      return self.__array__()
    
    return self._val(index * self.time_per_index)

  def __len__(self):
    return self.length

  def _reset(self):
    self._val.reset()




class HeatSignal(Signal):
  '''
    This class defines a Heat signal using the standard definition with
    a finite rise-time, followed by a double exponential tail. 

    Read this module's docstring. 

    Note that by default, the time_per_index for Heat pulses is 
    2.016 ms. That means that the parameters (0, 2, 3, 5)
    used in the '_val' method should also be in units of 'ms'. The 
    length of the pulse, by default, is 512 points.


    Here is the code that generates the signal

    .. code-block:: python

      if time < par[0]: return 0  
      else: return par[1]*(1 - math.exp(-(time-par[0])/par[2]))*(math.exp(-(time-par[0])/par[3]) + par[4]*math.exp(-(time-par[0])/par[5]))

    Use setpar and setparameters instead of setting the values 
    in the _parameters directly

  '''

  def __init__(self, length = 512, time_per_index = 2.016, parameters = None):

    if parameters is None:
      Signal.__init__(self, length, time_per_index, [514.08,-1,21.04,16.34,0.1331,129.54]) #these are actually the parameters for chalA FID802 for 2011-11-23
    else:
      Signal.__init__(self, length, time_per_index)


    

  @memoized
  def _val(self, time):
    '''

    .. code-block:: python

      if time < par[0]: 
          return 0  

      return par[1]*(1 - math.exp(-(time-par[0])/par[2]))*(math.exp(-(time-par[0])/par[3]) + par[4]*math.exp(-(time-par[0])/par[5]))
    '''

    Signal._val(self, time)

    if time < self._parameters[0]: 
      return 0  

    pt = time-self._parameters[0]  #pt = post trigger

    return self._parameters[1]*(1 - math.exp(-( pt )/self._parameters[2]))*(math.exp(-( pt )/self._parameters[3]) + self._parameters[4]*math.exp(-( pt )/self._parameters[5]))




class BBv2IonSignal(Signal):
  '''
    Defines a BBv2 ionization signal (a step function)

    .. code-block:: python

      if time < par[0]: 
        return 0  
      
      return par[1]

    Use setpar and setparameters instead of setting the values 
    in the _parameters directly

  '''

  def __init__(self, length = 8192, time_per_index = 1.0, parameters = None):
    

    if parameters is None:
      Signal.__init__(self, length, time_per_index, [4095.0, 1.0])
    else:
      Signal.__init__(self, length, time_per_index)


    

  @memoized
  def _val(self, time):
    '''
    if time < par[0]: 
      return 0  
    
    return par[1]
    '''
    Signal._val(self, time)

    if time < self._parameters[0]: 
      return 0  
    
    return self._parameters[1]




class GaussianNoise(Signal):
  '''
    This inherits from Signal, but doesn't use 'memoize'. It will always return a different noise signal

    This is just gauassian noise about zero with a particular width = std_deviation.  (width**2 = variance)

  '''
  def __init__(self, width, length = 8192):
    Signal.__init__(self, length, 1.0)
    self.width = width

  def _val(self, index):
    '''
    '''
    Signal._val(self, index)
    return random.gauss(0,self.width)





class ArbitraryNoise(object):
  '''
    This does not inherit from Signal, but is iterable and numpy array compatible. It acts like
    it inherits from Signal, so you can use it similarly. 
    
    For example:

    .. code-block:: python

      noise = KDataPy.signals.ArbitraryNoise(noise_power)
      heat = KDataPy.signals.HeatSignal()
      heat.setpar(1, -500)

      matplotlib.pyplot.plot(heat + noise)

    You must call .generate() to create a new noise pulse based upon the input noise_power.  

    .. code-block:: python

      noise.generate()

      matplotlib.pyplot.plot(heat + noise)

    You can change the noise_power simply by calling

    .. code-block:: python

      noise.noise_power = another_noise_power 
      noise.generate()

      matplotlib.pyplot.plot(noise)

    you can get access to the numpy array by calling
    
    .. code-block:: python
    
      noise_nparr = noise[]  or 
      noise_nparr = noise.noise_instance or
      noise_nparr = noise.__array__()

  '''

  def __init__(self, noise_power):
    '''
      noise_power should be 'list-like'. That is, it can be a numpy array, list or tuple.
      It should also be of length N/2 + 1, where N is the length of the pulse and should be even. 
    '''
    
    self.noise_power = noise_power  
    self.generate() #have to manually call this the first time...

  def generate(self):
    '''
      You should call this method each time you want to generate a new instance of noise signal.
    '''
    no2p1 = len(self.noise_power) #n over 2 plus 1
    n = 2*(no2p1-1)

    noise_fft = numpy.empty(no2p1, dtype=complex)

    noise_fft[0] = numpy.complex( random.gauss(0, math.sqrt(self.noise_power[0]/2.0)) , 0)
    noise_fft[n/2] = numpy.complex( random.gauss(0, math.sqrt(self.noise_power[n/2]/2.0)) , 0)

    for i in range(1,n/2):
      width = math.sqrt(self.noise_power[i]/2.0)
      noise_fft[i] = numpy.complex(random.gauss(0, width), random.gauss(0, width))

    self.noise_instance = math.sqrt(n) * numpy.fft.irfft(noise_fft, n) 



    #here is the version using KData's KHalfComplexToRealDFT()
    #this is about ~4 times as slow as using the code above. however, this is due to the sub-optimal interfaceing 
    #of python to C/kdata here and the extra Python for-loop. If this was all run in C/C++, I'm believe KData would just as fast.
    #I think numpy is written in FORTRAN, so it could actually be faster than C
    # import ROOT
    # import KDataPy.util 
    # ROOT.gSystem.Load('libkpta')
    # self.hc2r = ROOT.KHalfComplexToRealDFT()
  
    # real = numpy.empty(no2p1)

    # real[0] = random.gauss(0, math.sqrt(self.noise_power[0]/2.0))
    # real[ n/2 ]  = random.gauss(0, math.sqrt(self.noise_power[ n/2 ]/2.0))

    # imag = numpy.empty(no2p1-2)

    # for i in range(1,  n/2 ):
    #   width = math.sqrt(self.noise_power[i]/2.0)
    #   real[i], imag[i-1] = random.gauss(0, width), random.gauss(0, width)
  
    # hcarray = numpy.concatenate( (real,imag[::-1]) )  #puts the real/imag in half-complex array format
    # self.hc2r.SetInputPulse(hcarray, len(hcarray))
    # self.hc2r.RunProcess()
    # self.noise_instance = numpy.empty( self.hc2r.GetOutputPulseSize())
    # for i in range(self.hc2r.GetOutputPulseSize()):
    #   self.noise_instance[i] = math.sqrt(n) * self.hc2r.GetOutputPulse()[i]



    #here is the code where I directly compared the difference between the two 
    #methods (numpy v. kdata). The results are exactly the same to within machine round-off error (10^-14)


    # noise_fft = numpy.empty( n , dtype=complex)
    # real = numpy.empty(no2p1)
    # imag = numpy.empty(no2p1-2)
    # real[0] = random.gauss(0, math.sqrt(self.noise_power[0]/2.0))
    # real[ n/2 ]  = random.gauss(0, math.sqrt(self.noise_power[ n/2 ]/2.0))
    # noise_fft[0] = numpy.complex( real[0] , 0)
    # noise_fft[ n/2 ] = numpy.complex( real[ n/2 ] , 0)

    # for i in range(1,  n/2 ):
    #   width = math.sqrt(self.noise_power[i]/2.0)
    #   real[i], imag[i-1] = random.gauss(0, width  ), random.gauss(0, width)
    #   noise_fft[i] = numpy.complex( real[i], imag[i-1])

    # self.noise_instance_numpy = math.sqrt(n) * numpy.fft.irfft( noise_fft, n ) #this should be entirely real

    # hcarray = numpy.concatenate( (real,imag[::-1]) )  #puts the real/imag in half-complex array format
    # self.hc2r.SetInputPulse(hcarray, len(hcarray))
    # self.hc2r.RunProcess()
    # self.noise_instance = numpy.empty( self.hc2r.GetOutputPulseSize())
    # self.noise_instance_diff = numpy.empty( self.hc2r.GetOutputPulseSize())
    # for i in range(self.hc2r.GetOutputPulseSize()):
    #   self.noise_instance[i] = math.sqrt(n) * self.hc2r.GetOutputPulse()[i]
    #   self.noise_instance_diff[i] = self.noise_instance[i] - self.noise_instance_numpy[i]



  def __iter__(self):
    i = 0
    while i < len(self.noise_instance):
      yield self.noise_instance[i]
      i += 1


  def __array__(self):
    return self.noise_instance


  def __add__(self, rhs):
    try:
      return self.__array__() + rhs.__array__()
    except:
      return NotImplemented


  def __sub__(self, rhs):
    try:
      return self.__array__() - rhs.__array__()
    except:
      return NotImplemented


  def __mul__(self, rhs):
    try:
      return self.__array__() * rhs.__array__()
    except:
      return NotImplemented


  def __getitem__(self, index = None):
    if index is None:
      return self.__array__()
    
    return self.__array__()[index]


  def __len__(self):
    return len(self.noise_instance)


  def __call__(self, index = None):
    if index is None:
      return self.__array__()
    
    return self.__array__()(index)
  
