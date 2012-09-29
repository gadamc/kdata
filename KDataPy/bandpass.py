#!/usr/bin/env python
import json
import scipy.signal
import KDataPy.database

def getBlankDoc():
  return {
     "coef": {
         "a": [],
         "b": []
     },
     "bandtype": "",
     "filtertype": "",
     "order": "",
     "responsetype": "",
     "frequencies": [],
     "binwidth_ns": "",
     "doctype": "filterdefinition"
  }
  
#
def uploadIIRFilter(binwidth, order, docid, freqs, filtertype="butter", bandtype="highpass", replace=False):
  '''
  the filtertype and bandtype variables should match the expected values in scipy.signal.iirfilter
  binwidth is in nanoseconds
  freqs is either a scalar or a list of size two.
  '''
  db = KDataPy.database.kmultiprocdb(serverName='https://edelweiss:3000kgd@edelweiss.cloudant.com')
  doc = getBlankDoc()
  doc['frequencies'] = freqs
  doc['binwidth_ns'] = binwidth
  doc['order'] = order
  doc['filtertype'] = filtertype
  doc['bandtype'] = bandtype
  doc['responsetype'] = 'iir'
  doc['_id'] = docid
  
  if len(freqs) > 2:
    print 'only two frequencies can be given. you gave: ', freqs
  
  nyquist = 0.5 / (binwidth*1e-9)
  
  if len(freqs) == 2:
    cornerFreqs = []
    for freq in freqs:
      cornerFreqs.append(freq / nyquist) 
  elif len(freqs) == 1:
    cornerFreqs = freqs[0] / nyquist
    
  print cornerFreqs, nyquist
  
  try:
    (b,a) = scipy.signal.iirfilter(order, cornerFreqs, btype=bandtype, ftype=filtertype)
  
    doc['coef']['a'] = list(a)
    doc['coef']['b'] = list(b)
  
    if db.doc_exist(doc.get("_id")) and replace==False:
      answer = raw_input('This document exists - do you want to replace it (yes/no)')
      if answer == 'no': 
        print 'quiting'
        return
      if answer != 'yes':
        print 'didn\'t understand. quiting'
        return
      if answer == 'yes':
        doc['_rev'] = db.get_rev(doc.get('_id'))
        
    if db.doc_exist(doc.get("_id")) and replace:
      doc['_rev'] = db.get_rev(doc.get('_id'))
  
  except Exception as e:
    print 'Exception caught. quiting'
    print e
    return
     
  print 'saving doc to database'
  print json.dumps(doc, indent=1)
  db.save_doc(doc)

