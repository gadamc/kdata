def functionlist():
  '''
  returns a list of dictionaries that define the different pulse templates. Each dictionary has a 'name', 'python', 'par', 'time_unit' and 'tf1' key.
  the 'python' key contains a string that defines a function called "template(x, par)" which can be used within a Python script by calling
  exec(doc['python'])
  the 'tf1' key contains a string that defines a formula used in ROOT's TF1 class
  the 'par' is an array of initial values for the formula found in 'python' and 'tf1'
  the 'time_unit' holds the number of seconds for each parameter is 'par' that has a dimension of time. For example, in the 'rise_double_decay' template
  the first, third, fourth and fifth parameters are the trigger time, rise time, first decay time and second decay time. 
  Thus par[0] * time_unit = pretrigger time in seconds.  
  '''
  _rise_double_decay = {'python':"\ndef template(xx,par):\n  import math\n  x = float(xx)\n  if x < float(par[0]): return 0\n  else: return float(par[1])*(1 - math.exp(-(x-float(par[0]))/float(par[2])))*(math.exp(-(x-float(par[0]))/float(par[3])) + float(par[5])*math.exp(-(x-float(par[0]))/float(par[4]))\n",
                          'tf1': "x < [0] ? 0.0 : [1]*(1 - ([2] > 0.0 ? TMath::Exp(-(x-[0])/[2]) : 0.0))*(([3] > 0.0 ? TMath::Exp(-(x-[0])/[3]) : 0.0) + [5]*([4] > 0.0 ? TMath::Exp(-(x-[0])/[4]) : 0.0))",
                          'par': [514.08, -1.0, 5.0, 15.0, 70.0, 0.25], 'time_unit': 0.001, 'name':'rise_double_decay'}
  _single_decay = {'python' :"\ndef template(xx,par):\n  import math\n  x = float(xx)\n  if x < float(par[0]): return 0\n  else: return float(par[1])*math.exp(-(x-float(par[0]))/float(par[2]))\n",
                          'tf1': "x < [0] ? 0.0 : [1]*(([2] > 0.0 ? TMath::Exp(-(x-[0])/[2]) : 0.0))",
                          'par': [4095.0, 1.0, 1000.0], 'time_unit' : 0.00001,'name':'single_decay'}
  _step = {'python':"\ndef template(xx,par):\n  import math\n  x = float(xx)\n  if x < float(par[0]): return 0\n  else: return float(par[1])\n",
                          'tf1': "x < [0] ? 0.0 : [1]",
                          'par': [4095.0, 1.0], 'time_unit' : 0.00001,'name':'step'}
                
  return [_single_decay, _rise_double_decay, _step]
            
def getType():

  listOfFuncs = functionlist()
  
  ptype = raw_input('Select type: 0 = single decay, 1 = rise double decay, 2 = step ')
  theRet = {}
  try:
      theRet = listOfFuncs[int(ptype)]
  except:
    print 'unknown type', ptype
    return getType()
  
  return theRet
  
  
def fitByHand(pulse, ptype = None):
  
  import ROOT
  import KDataPy
  from KDataPy import util
  import matplotlib.pyplot as plt
  import numpy as np

  try:
    ROOT.gSystem.Load('libkpta')
  except Exception as e:
    print 'unable to load libkpta. make sure you\'ve properly set your KDATA environment variables.'
    raise e
    
  if ptype == None:
    ptype = getType()
  else:
    try:
     functionlist()[int(ptype)]
    except:
      print ptype, 'is invalid option'
      ptype = getType
      
  ff1 = ROOT.TF1(ptype['name'], ptype['tf1'], 0, pulse.GetPulseTimeWidth()*(1e-9)*pulse.GetPulseLength())
  
  vpar = np.array(ptype['par'], dtype="double")
  vpar[0] = ptype['time_unit']*vpar[0]
  try:
    vpar[2:] = ptype['time_unit']*vpar[2:]
  except: pass
  
  ff1.SetParameters( ptype['time_unit']*np.array(ptype['par'], dtype="double") )
  #vpulse_y = np.array(pulse.GetTrace(), dtype="double")
  #vpulse_x = pulse.GetPulseTimeWidth()*(1e-9)*np.array(range(1, pulse.GetPulseLength()), dtype="double")
  bas = ROOT.KBaselineRemoval()
  bas.SetInputPulse(pulse.GetTrace())
  bas.RunProcess()
  
  gpulse = ROOT.TGraph(pulse.GetPulseLength(), pulse.GetPulseTimeWidth()*(1e-9)*np.array(range(1, pulse.GetPulseLength()), dtype="double"), np.array(bas.GetOutputPulse(), dtype="double"))
  gpulse.Fit(ff1)
  gpulse.Draw('A*')
  raw_input('what do you think? ') 
  
def fit(pulse, name, initialpars):
  
  import ROOT
  import KDataPy
  from KDataPy import util
  import matplotlib.pyplot as plt
  import numpy as np

  try:
    ROOT.gSystem.Load('libkpta')
  except Exception as e:
    print 'unable to load libkpta. make sure you\'ve properly set your KDATA environment variables.'
    raise e
    
  for typedict in functionlist():
    if typedict['name'] == name: ptype = typedict
    
  try:
    vpar = np.array( ptype['par'] )
    ff1 = ROOT.TF1(ptype['name'], ptype['tf1'], 0, 1.0 / ptype['time_unit'])
  except:
    print name, 'not found'
    return
    
  
  
  