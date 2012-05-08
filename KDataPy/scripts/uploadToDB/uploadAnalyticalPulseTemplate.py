#!/usr/bin/env python
from couchdbkit import Server, Database
from ROOT import TF1
import pickle, sys, datetime, json, string

def fillVars(vlist, skip, number):
  
  for i in range(skip,number):
    val = ''
    while type(val) != type(0.0):
      val = raw_input('Enter value for par[%d]: ' % i)
      try:
        val = float(val)
        vlist[i] = val
      except:
        print '   fail. you didn\'t enter a number'
        val = ''
        pass
  
def addFormulaResult(n, channel):
  doc = {}
  sf = TF1('single_decay', 'x > [0] ? [1] * exp(-(x-[0])/[2]) : 0', 0, 8192)
  sf.SetName('single_decay')
  df = TF1('rise_double_decay', 'x > [0] ? [1]*(1 - exp(-(x-[0])/[2]))*(exp(-(x-[0])/[3]) + [4]*exp(-(x-[0])/[5])) : 0', 0, 512)
  df.SetName('rise_double_decay')
  stepf = TF1('step', 'x > [0] ? [1] : 0', 0, 8192)
  stepf.SetName('step')
  
  if n == 1:
    subdoc = {}
    doc['single_decay'] = subdoc
    
    subdoc['python'] = "\ndef template(x,par):\n  import math\n  if x < par[0]: return 0\n  else: return par[1]*math.exp(-(x-par[0])/par[2])\n"
    
    print 'you selected the following functional form (python syntax): '
    print subdoc['python']
    
    subdoc['par'] = [0,1,0]
    
    if channel.startswith('chal'):
      sf.SetRange(0,512)
      subdoc['par'][0] = 255
      subdoc['par'][1] = -1
    else: 
      sf.SetRange(0,8192)
      subdoc['par'][0] = 4095
    
    fillVars(subdoc['par'], 2, len(subdoc['par']))
    
    for i in range(len(subdoc['par'])):
      sf.SetParameter(i, subdoc['par'][i])
    
    subdoc['tf1'] = pickle.dumps(sf)
    
  elif n == 2:
    subdoc = {}
    doc['rise_double_decay'] = subdoc 
    subdoc['python']= "\ndef template(x,par):\n  import math\n  if x < par[0]: return 0\n  else: return par[1]*(1 - math.exp(-(x-par[0])/par[2]))*(math.exp(-(x-par[0])/par[3]) + par[4]*math.exp(-(x-par[0])/par[5]))\n"
    
    print 'you selected the following functional form (python syntax): '
    print subdoc['python']
    
    subdoc['par'] = [255,-1,0,0,0,0]
    fillVars(subdoc['par'], 2, len(subdoc['par']))
    
    for i in range(len(subdoc['par'])):
      df.SetParameter(i, subdoc['par'][i])
    
    subdoc['tf1'] = pickle.dumps(df)
    
  elif n == 3:
    subdoc = {}
    doc['step'] = subdoc 
    subdoc['python']= "\ndef template(x,par):\n  import math\n  if x < par[0]: return 0\n  else: return par[1]\n"
    
    print 'you selected the following functional form (python syntax): '
    print subdoc['python']
    
    subdoc['par'] = [0,0]
    
    if channel.startswith('chal'):
      stepf.SetRange(0,512)
      subdoc['par'][0] = 255
      subdoc['par'][1] = 1
    else: 
      stepf.SetRange(0,8192)
      subdoc['par'][0] = 4095
      subdoc['par'][1] = 1
      
    fillVars(subdoc['par'], 2, len(subdoc['par']))
    
    for i in range(len(subdoc['par'])):
      stepf.SetParameter(i, subdoc['par'][i])
    
    
    subdoc['tf1'] = pickle.dumps(stepf)
    
  else:
    print 'unrecognized option...'
    doc = ''
    
  return doc
  
def printGreeting(now):
  if now.hour < 12 and now.hour > 5: print 'Good morning'
  elif now.hour < 17 and now.hour > 12: print 'Good afternoon'
  elif now.hour < 24 and now.hour > 17: print 'Good evening'
  else : print 'Burning the midnight oil?'
  print 'Please answer the following questions'
  
  
def getFormattedDate(value = ''):
  
  if value == '':
    value = raw_input("Start UTC date-time from which this template is valid (yyyy-mm-dd hh:mm:ss): ")
  #check format
  try:
    theDate = datetime.datetime.strptime(value,'%Y-%m-%d %H:%M:%S')
    value = str(theDate)
    return value
    
  except:
    print 'that did not seem to match the format. try again.'
    return getFormattedDate()
  
  
def addTemplates(doc):
  
  option = ''
  if doc.has_key('formula') == False:
    doc['formula'] = {}
  while option != 'done':
    option = raw_input('Add a record: Single exponential[1], heat double exponential template[2], or bbv2 step [3] (or type "done"): ')
    try:
      formNum = int(option)
    except: pass
    else:
      formulaDoc = addFormulaResult(formNum, doc['channel'])
      if formulaDoc != '':
        doc['formula'].update(formulaDoc)
    
  
def addKampSites(doc):
  
  go = ''
  while go != 'y' and go != 'n':
    go = raw_input('Do you want to add kampsite information (y/n)? ')
    if go == 'n': return #quit the function
  
  answer = ''
  kampDoc = {}
  while True:
    answer = raw_input('Add a new KampSite. What is the name? (or "done"): ')
    if answer == 'done':
      break 
    kampDoc[answer] = {}
    
    addParameter = ''
    while True:
      addParameter = raw_input('Give a name for a new parameter for this kampsite (or done): ')
      if addParameter == 'done':
        break
      value = '' 
      try:
        inputValue = raw_input('Give a value for this parameter (this can be a number, string, list, or dictionary): ')
        value = eval(inputValue)
      except SyntaxError:  #eval throws if inputValue is a string that can't be parsed into a number, list or dictionary
        value = inputValue
        
      kampDoc[answer][addParameter] = value
        
    if doc.has_key('kampsites') == False:
      doc['kampsites'] = {}
      
    doc['kampsites'].update(kampDoc)
  
  
    
def fillDoc(**kwargs):
  
  now = datetime.datetime.utcnow()
  printGreeting(now)
  
  doc = {}
  if kwargs.has_key('channel') == False: doc['channel'] = raw_input("Channel Name: ")
  else: doc['channel'] = kwargs['channel']
  
  if kwargs.has_key('author') == False: doc['author'] = raw_input("What is your name? ")
  else: doc['author'] = kwargs['author']
  
  if kwargs.has_key('date') == False: doc['date'] = getFormattedDate()
  else: doc['date'] = getFormattedDate(kwargs['date'])
  
  if kwargs.has_key('method') == False: doc['method'] = raw_input('Short description of analysis method (i.e "by hand"): ')
  else: doc['method'] = kwargs['method']
    
  doc['date submited'] = str(now)
  doc['type'] = 'analytical'
  doc['formula'] = {}

  addTemplates(doc)

  if kwargs.has_key('addkampsite') == False: 
    addKampSites(doc)
  else:
    if kwargs['addkampsite'] != 0:
      addKampSites(doc)
  print 'Review your document'
  print json.dumps(doc, indent=1)
  
  answer = ''
  while answer != 'y' and answer != 'n':
    answer = raw_input('Is this correct (y/n)? ')
  if answer == 'y': 
    return doc
  else: 
    return ''

def main(*args, **kwargs):
  '''
  you should pass in the username / password of the edwdbik.fzk.de couch database
  '''
  # if len(args) != 2:
  #     print help(main)
  #     sys.exit(-1)
      
  s = Server('https://%s:%s@edwdbik.fzk.de:6984' % (args[0], args[1]))
  db = s['pulsetemplates']

  doc = fillDoc(**kwargs)
  
  if doc != '':
    db.save_doc(doc)
  
if __name__ == '__main__':
  import json
  
  argv = sys.argv
  arg_strs = argv[1:]
  
  args = []
  kwargs = {}
  for s in arg_strs:
    if s.count('=') == 1:
      key, value = s.split('=', 1)
    else:
      key, value = None, s
    print key, value
    try:
      value = json.loads(value) 
    except ValueError:
      pass
    if key:
      kwargs[key] = value
    else:
      args.append(value)
              
  main(*args, **kwargs)