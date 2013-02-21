#!/usr/bin/env python
from couchdbkit import Server
from csv import DictReader
import time, sys, subprocess, math, os, datetime, pytz, calendar, json

#______________
# parseDoc
def parseDoc(doc):
  for k,v in doc.items():
    
    #strips off any spaces found in the keys.
    del doc[k]
    kk = k.strip(' ') 
    doc[kk] = v  

    # see if this string is really an int or a float
    if (isinstance(v,str) or isinstance(v, unicode)):

      if v.isdigit()==True: #int
        doc[kk] = int(v)
      else: #try a float
        try:
          if math.isnan(float(v))==False:
            doc[kk] = float(v) 
        except:
          pass  

  return doc
  

#______________
# uploadFile
def uploadFile(fname, uri, dbname):
     
  print 'Upload contents of %s to %s/%s' % (fname, uri, dbname)
  
  # #connect to the db
  theServer = Server(uri)
  db = theServer[dbname]
  
  #loop on file for upload
  reader = DictReader(open(fname, 'rU'), dialect = 'excel')
  
  #store a list of docs for upload to db
  docs = []

  for doc in reader:

    newdoc = parseDoc(doc)
  
    #enforce some sort of schema. that is, require the existence of a set of keys in the database documents
    requiredSet = set(['muonmodule', 'end', 'year', 'month', 'day', 'HV channel', 'ADC channel', 'ADC card', 'TDC channel'])
    if (requiredSet < set(newdoc.keys())) is False:
      print 'Quitting! Your CVS Muon Veto DAQ map MUST have the following columns'
      print ', '.join([x for x in requiredSet])
      sys.exit(1)


    #reformat the date to a single dictionary
    newdoc['date_valid'] = {} 
    for dk in ['year', 'month', 'day', 'hour', 'minute']:
      try:
        newdoc['date_valid'][dk] = newdoc[dk]
        del newdoc[dk]  
      except: pass

    #also save the date in unix time.
    yy,mm,dd = newdoc['date_valid']['year'], newdoc['date_valid']['month'], newdoc['date_valid']['day']
    newdoc['date_valid_unixtime'] = calendar.timegm( datetime.datetime(yy,mm,dd,tzinfo=pytz.utc).utctimetuple() )
    

    #fill in additional information for database document
    newdoc['type'] = 'muon_veto_daq_map'
    newdoc['author'] = 'KIT'
    newdoc['content'] = 'Muon Veto DAQ Mapping'
    dd = datetime.datetime.utcnow()
    newdoc['date_filed'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second} 
    newdoc['date_filed_unixtime'] = time.time()
    newdoc['date_filed_isoformat'] = dd.isoformat() + '+0:00'  #add the +0:00 to indicate clearly that this is UTC

    #parse comments into a list of comments
    commentList = newdoc['comments'].split('/')
    newdoc['comments'] =  [x.strip() for x in commentList] #strip off any extra spaces 
    
    #append to list for bulk upload when ready
    docs.append(newdoc)
  
  
  db.bulk_save(docs)
  

if __name__=='__main__':
  csvfile = sys.argv[1]
  uri = sys.argv[2]
  
  dbname = 'muonvetohardwaremap'
  if len(sys.argv) >= 4:
    dbname = sys.argv[3]
  
  uploadFile(csvfile, uri, dbname)
  
