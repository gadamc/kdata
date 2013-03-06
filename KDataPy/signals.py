'''
  This module defines classes to generate Heat and BBv2 Ionization signals 
  (BBv1 pulses could be easily added if needed) They can return the value 
  at a particular time, or they may be used to generate a list or numpy array
  of numerical values. Additionally, this module defines two classes to produce
  noise. The GaussianNoise class produces a randomly distributed, but
  independent, set of values about zero and of a particular width. While, the 
  ArbitraryNoise class will randomly generare a noise pulse based on an input 
  noise power spectrum.  (For technical reasons ArbitraryNoise doesn't inherit 
  directly from Signal, but it does behave similarly.)   
  These classes are iterable and can be used directly with Numpy and 
  MatPlotLib. That is, you can called

  import KDataPy.signals

  heat = KDataPy.signals.HeatSignal()
  heat.parameters[1] = -1500 #by default, this is -1, which is fine for
                             #templates, but should be larger to see 
                             #above typical noise levels

  #use iteration to make a list
  pulse = []
  for val in heat:
    pulse.append(val)

  #and get a numpy array

  pulse = heat()  # "call" the instance with the () to return a numpy array

  #and plot with MatPlotLib

  matplotlib.pyplot.plot(heat)

  These classes have been optimized for speed (in case you repeatedly want
  to generate the same pulse), by using a standard Python 'memoization' 
  decorator cache. The values are cached the first time they are calculated 
  so that they are not calculated again. 

  This means that the second time you call 

  pulse = heat()

  the values are just retrieved from the cache. (However, they are repacked 
  into a numpy array, which takes time.)  

  If you change the value of the parameters, however, the pulse will be 
  recalculated.

  Additionally, you can use the object call to return just a single 
  value of the signal. That is, 
  
  heat(10) 

  returns the heat at time = 10. 


  Signals can also be added, subtraced and multipled with the '+' , '-' 
  and '*' operators. These operations return a numpy array. 

  noise = KDataPy.signals.GaussianNoise(5)  #instantiate a GaussianNoise 
                                            #object of width = 5

  signal = noise + heat
  plt.plot(signal)

  Finally, the ArbitraryNoise class will randomly generate a noise pulse
  given an input power spectrum. 

  Say 'noise_power' is a list-like object that is the average power 
  spectrum for a particular channel. Then you would use it to 
  instantiate an ArbitraryNoise object.

  noise = KDataPy.signals.ArbitraryNoise( noise_power )

  plt.plot(heat + noise)


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
  '''

  def __init__(self, length = 8192, time_per_index = 1, parameters = None):

    self.length = length
    self.time_per_index = time_per_index
    self.parameters = parameters


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


  def __getattribute__(self, name):
    if name == 'parameters':
      self._reset()
    return object.__getattribute__(self, name)


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

  def __call__(self, time = None):
    if time is None:
      return self.__array__()
    
    return self._val(time)

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

  '''

  def __init__(self, length = 512, time_per_index = 2.016, parameters = None):
    Signal.__init__(self, length, time_per_index)

    if parameters is None:
      self.parameters = [514.08,-1,21.04,16.34,0.1331,129.54] #these are actually the parameters for chalA FID802 for 2011-11-23

    

  @memoized
  def _val(self, time):
    '''
      if time < par[0]: 
          return 0  

      return par[1]*(1 - math.exp(-(time-par[0])/par[2]))*(math.exp(-(time-par[0])/par[3]) + par[4]*math.exp(-(time-par[0])/par[5]))
    '''

    Signal._val(self, time)

    if time < self.parameters[0]: 
      return 0  

    pt = time-self.parameters[0]  #pt = post trigger

    return self.parameters[1]*(1 - math.exp(-( pt )/self.parameters[2]))*(math.exp(-( pt )/self.parameters[3]) + self.parameters[4]*math.exp(-( pt )/self.parameters[5]))




class BBv2IonSignal(Signal):
  '''
    Defines a BBv2 ionization signal (a step function)

  '''

  def __init__(self, length = 8192, time_per_index = 1.0, parameters = None):
    Signal.__init__(self, length, time_per_index)

    if parameters is None:
      self.parameters = [4095.0, 1.0]

    

  @memoized
  def _val(self, time):
    '''
    if time < par[0]: 
      return 0  
    
    return par[1]
    '''
    Signal._val(self, time)

    if time < self.parameters[0]: 
      return 0  
    
    return self.parameters[1]




class GaussianNoise(Signal):
  '''
    This inherits from Signal, but doesn't use 'memoize'. 

    This is just gauassian noise about zero with a particular width
  '''
  def __init__(self, width, length = 8192):
    Signal.__init__(self, length, 1.0)
    self.width = width

  #don't use memoize since I want to 
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

    noise = KDataPy.signals.ArbitraryNoise(noise_power)
    heat = KDataPy.signals.HeatSignal()
    heat.parameters[1] = -500

    plt.plot(heat + noise)

    You must call .generate() to create a new power spectrum based upon the input noise_power.  

    noise.generate()

    plt.plot(heat + noise)

  '''

  def __init__(self, noise_power):
    '''
      noise_power should be 'list-like'. That is, it can be a numpy array, list or tuple.
      It should also be of length N/2 + 1, where N is the length of the pulse and should be even. 
    '''
    self.noise_power = noise_power
    self.generate()    

  def generate(self):

    no2p1 = len(self.noise_power) #n over 2 plus 1
    n = 2*(no2p1 - 1)

    noise_fft = numpy.empty( n , dtype=complex)

    noise_fft[0] = numpy.complex( random.gauss(0, math.sqrt(self.noise_power[0]/2.0)) , 0)
    noise_fft[ n/2 ] = numpy.complex( random.gauss(0, math.sqrt(self.noise_power[ n/2 ]/2.0)) , 0)

    for i in range(1,  n/2 ):
      width = math.sqrt(self.noise_power[i]/2.0)
      noise_fft[i] = numpy.complex(random.gauss(0, width), random.gauss(0, width))
      noise_fft[n - i] = noise_fft[i].conjugate()

    self.noise_instance = n * numpy.fft.ifft( noise_fft ).real  #this should be entirely real




    #here is the version using KData's KHalfComplexToRealDFT()
    #this is about ~4 times as slow as using the code above. however, this is due to the sub-optimal interfaceing 
    #of python to C/kdata here and the extra for-loop. If this was all run in C/C++, I'm believe KData would just as fast
    #when optimized to handle memory in the most efficient way. I think numpy is written in FORTRAN, so it could actually be faster than C
    
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
    # self.noise_instance_diff = numpy.empty( self.hc2r.GetOutputPulseSize())
    # for i in range(self.hc2r.GetOutputPulseSize()):
    #   self.noise_instance[i] = self.hc2r.GetOutputPulse()[i]
    #   self.noise_instance_diff[i] = self.noise_instance[i] = self.noise_instance_numpy[i]



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
    #   noise_fft[n - i] = noise_fft[i].conjugate()

    # self.noise_instance_numpy = n * numpy.fft.ifft( noise_fft ).real  #this should be entirely real

    # hcarray = numpy.concatenate( (real,imag[::-1]) )  #puts the real/imag in half-complex array format
    # self.hc2r.SetInputPulse(hcarray, len(hcarray))
    # self.hc2r.RunProcess()
    # self.noise_instance = numpy.empty( self.hc2r.GetOutputPulseSize())
    # self.noise_instance_diff = numpy.empty( self.hc2r.GetOutputPulseSize())
    # for i in range(self.hc2r.GetOutputPulseSize()):
    #   self.noise_instance[i] = self.hc2r.GetOutputPulse()[i]
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

  def __call__(self, time = None):
    if time is None:
      return self.__array__()
    
    return self.__array__()[time]

  def __len__(self):
    return len(self.noise_instance)
