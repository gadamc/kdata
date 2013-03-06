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
    Base class for Signals (HeatSignal and BBv2IonSignal.. BBv1IonSignal could be easily added following the source code)
    You shouldn't instantiate this class directly.

    These classes model Heat and BBv2 Ionization Signals. They can return the value at a particular time, 
    or they may be used to generate a list or numpy array of numerical values. 

    These classes are iterable and can be used directly with Numpy and MatPlotLib. That is, you can called

    import KDataPy.signals

    h = KDataPy.signals.HeatSignal()

    pulse = []
    for val in h:
      pulse.append(val)

    #and

    pulse = h.__array__()  # pulse is a numpy.array type.

    #and

    matplotlib.pyplot.plot(h)

    In order to make these classes fast for subsequent calculation of the signal values (in case you repeatedly want
    to generate the same pulse), a standard Python 'memoization' decorator caches the values so that they are not calculated again.
    This means that the second time you call 

    pulse = h.__array__()

    the values are actually not recalculated, but rather retrieved from the cache. (However, they are repacked into a numpy array, which takes time.)

    While convenient for making subsequent pulse generation very fast, this is slightly inconvenient if you change the parameters of the Signal.
    Due to the caching, you must call the .reset() function if you want to change a parameter.

    h.parameters[1] = -4.0
    h.reset()

    Otherwise, h.val(), h.__iter__, and h.__array__ will returned the cached values for a pulse calculated with the previous set of parameters. 

    There's probably a nice programmatic way to fix this, so feel free to do so if you know how.
    If not, just make sure to call h.reset() before you try to regenerate a pulse with new parameters.

  '''

  def __init__(self, length = 8096, time_per_index = 1, parameters = None):

    self.length = length
    self.time_per_index = time_per_index


  def val(self, time):
    if isinstance(time, int) is False and isinstance(time, float) is False and isinstance(time, long) is False:
      raise TypeError('input must be a number')
    if time < 0 or time/self.time_per_index > self.length:
      raise IndexError('out of range')


  def __iter__(self):
    i = 0
    while i < self.length:
      yield self.val(i * self.time_per_index)                   
      i += 1


  def __array__(self):
    return numpy.array([v for v in self])


  def reset(self):
    self.val.reset()




class HeatSignal(Signal):
  '''
    read the base-class (Signal) docstring    
  '''

  def __init__(self, length = 512, time_per_index = 2.016, parameters = None):
    Signal.__init__(self, length, time_per_index)

    if parameters is None:
      self.parameters = [514.08,-1,21.04,16.34,0.1331,129.54] #these are actually the parameters for chalA FID802 for 2011-11-23

    

  @memoized
  def val(self, time):
    '''
      if time < par[0]: 
          return 0  

      return par[1]*(1 - math.exp(-(time-par[0])/par[2]))*(math.exp(-(time-par[0])/par[3]) + par[4]*math.exp(-(time-par[0])/par[5]))
    '''

    Signal.val(self, time)

    if x < self.parameters[0]: 
      return 0  

    return self.parameters[1]*(1 - math.exp(-(x-self.parameters[0])/self.parameters[2]))*(math.exp(-(x-self.parameters[0])/self.parameters[3]) + self.parameters[4]*math.exp(-(x-self.parameters[0])/self.parameters[5]))




class BBv2IonSignal(Signal):
  '''
    read the base-class (Signal) docstring  
  '''

  def __init__(self, length = 8096, time_per_index = 1.0, parameters = None):
    Signal.__init__(self, length, time_per_index)

    if parameters is None:
      self.parameters = [4095.0, 1.0]

    

  @memoized
  def val(self, time):
    '''
    if time < par[0]: 
      return 0  
    
    return par[1]
    '''
    Signal.val(self, time)

    if x < self.parameters[0]: 
      return 0  
    
    return self.parameters[1]




class GaussianNoise(Signal):
  '''
    This inherits from Signal, but doesn't use 'memoize'. So, you don't need to call .reset(). 
    This is just gauassian noise about zero with a particular width
  '''
  def __init__(self, width, length = 8096):
    Signal.__init__(self, length, 1.0)
    self.width = width

  #don't use memoize since I want to 
  def val(self, index):
    '''
    '''
    Signal.val(self, index)
    return random.gauss(0,self.width)





class ArbitraryNoise(Signal):
  '''
    This does not inherit from Signal, but is iterable and numpy array compatible. You must call .generate()
    to create a new power spectrum based upon the input noise_power.  
  '''

  def __init__(self, noise_power):
    '''
      noise_power should be 'list-like'. That is, it can be a numpy array, list or tuple.
      It should also be of length N/2 + 1, where N is the length of the pulse and should be even. 
    '''
    # import ROOT
    # import KDataPy.util 
    # ROOT.gSystem.Load('libkpta')
    # self.hc2r = ROOT.KHalfComplexToRealDFT()
 
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
    #   real[i], imag[i-1] = random.gauss(0, width), random.gauss(0, width)
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

