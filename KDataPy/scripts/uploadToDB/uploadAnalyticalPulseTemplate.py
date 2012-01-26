#!/usr/bin/env python
from couchdbkit import Server, Database
from ROOT import TF1
import pickle, sys, datetime, json

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
  sf = TF1('single exponential', 'x < [0] ? [1] * exp(-(x-[0])/[2]) : 0', 0, 8192)
  sf.SetName('single exponential')
  df = TF1('double exponential heat template', 'x < [0] ? [1]*(1 - exp(-(x-[0])/[2]))*(exp(-(x-[0])/[3]) + [4]*exp(-(x-[0])/[5])) : 0', 0, 512)
  df.SetName('double exponential heat template')
  
  if n == 1:
    subdoc = {}
    doc['single exponential'] = subdoc
    
    subdoc['c'] = {}
    subdoc['c']['functionargs'] = ['double *x', 'double *par']
    subdoc['c']['value'] = 'double xx = x[0];\n if(xx < par[0]) return 0;\n else return par[1]*exp(-(xx-par[0])/par[2]);\n'
    subdoc['python'] = {}
    subdoc['python']['function'] = "def %s_template(x,par):\n  import math\n  xx = x[0]\n  if xx < par[0]: return 0\n  else: return par[1]*math.exp(-(xx-par[0])/par[2])\n" % string.replace(channel, ' ', '_')
    
    
    print 'you selected the following functional form (c syntax): '
    print subdoc['c']['value']
    
    subdoc['par'] = [0,1,0]
    
    if channel.startswith('chal'):
      sf.SetRange(0,512)
      subdoc['par'][0] = 256
    else: 
      sf.SetRange(0,8192)
      subdoc['par'][0] = 4096
    
    fillVars(subdoc['par'], 2, len(subdoc['par']))
    
    sf.SetParameter(0, subdoc['par'][0])
    sf.SetParameter(1, subdoc['par'][1])
    sf.SetParameter(2, subdoc['par'][2])
    
    psf = pickle.dumps(sf)
    #print psf
    subdoc['tf1'] = psf
    
  elif n == 2:
    subdoc = {}
    doc['double exponential heat template'] = subdoc 
    subdoc['c'] = {}
    subdoc['c']['functionargs'] = ['double *x', 'double *par']
    subdoc['c']['value'] = 'double xx = x[0];\n if(xx < par[0]) return 0;\n else return par[1]*(1 - exp(-(xx-par[0])/par[2]))*(exp(-(xx-par[0])/par[3]) + par[4]*exp(-(xx-par[0])/par[5]));\n'
    subdoc['python'] = {}
    subdoc['python']['function'] = "def %s_template(x,par):\n  import math\n  xx = x[0]\n  if xx < par[0]: return 0\n  else: return par[1]*(1 - math.exp(-(xx-par[0])/par[2]))*(math.exp(-(xx-par[0])/par[3]) + par[4]*math.exp(-(xx-par[0])/par[5]))\n" % string.replace(channel, ' ', '_')
       
       print 'you selected the following functional form (c syntax): '
    print subdoc['c']['value']
    
    subdoc['par'] = [256,1,0,0,0,0]
    fillVars(subdoc['par'], 2, len(subdoc['par']))
    
    df.SetParameter(0, subdoc['par'][0])
    df.SetParameter(1, subdoc['par'][1])
    df.SetParameter(2, subdoc['par'][2])
    df.SetParameter(3, subdoc['par'][3])
    df.SetParameter(4, subdoc['par'][4])
    df.SetParameter(5, subdoc['par'][5])
    
    pdf = pickle.dumps(df)
    #print psf
    subdoc['tf1'] = pdf
    
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
  
  
def getFormattedDate():
  
  value = ''
  while value == '':
    value = raw_input("Start UTC date-time from which this template is valid (yyyy-mm-dd hh:mm:ss): ")
    #check format
    try:
      theDate = datetime.datetime.strptime(value,'%Y-%m-%d %H:%M:%S')
      value = str(theDate)
    except:
      print 'that did not seem to match the format. try again.'
      value = ''
  
  return value
  
def addTemplates(doc):
  
  option = ''
  if doc.has_key('formula') == False:
    doc['formula'] = {}
  while option != 'done':
    option = raw_input('Add a record: Single exponential[1], heat double exponential template[2] (or type "done"): ')
    try:
      formNum = int(option)
      formulaDoc = addFormulaResult(formNum, doc['channel'])
      if formulaDoc != '':
        doc['formula'].update(formulaDoc)
    except Exception as e:
      print e
      print option, 'is not a number. try again'
  
  
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
      try:
        kampDoc[answer][addParameter] = float(raw_input('Give a numerical value for this parameter: '))
      except:
        print 'That was not a number. try again'
        
    if doc.has_key('kampsites') == False:
      doc['kampsites'] = {}
      
    doc['kampsites'].update(kampDoc)
  
  
  
    
def fillDoc():
  
  now = datetime.datetime.utcnow()
  printGreeting(now)
  
  doc = {}
  doc['channel'] = raw_input("Channel Name: ")
  doc['author'] = raw_input("What is your name? ")
  doc['date'] = getFormattedDate()
  doc['date submited'] = str(now)
  doc['type'] = 'analytical'
  doc['method'] = raw_input('Short description of analysis method (i.e "by hand"): ')
  doc['formula'] = {}
  addTemplates(doc)
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

def main(*args):
  '''
  you should pass in the username / password of the edwdbik.fzk.de couch database
  '''
  if len(args) != 2:
    print help(main)
    sys.exit(-1)
    
  s = Server('https://%s:%s@edwdbik.fzk.de:6984' % (args[0], args[1]))
  db = s['pulsetemplates']

  doc = fillDoc()
  
  if doc != '':
    db.save_doc(doc)
  
if __name__ == '__main__':
  main(*sys.argv[1:])