#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, datetime
import pytz, calendar

    
#______________
# parseDoc
def parseDoc(doc):
    for k,v in doc.items():
        #strips off any spaces found in the keys.
        del doc[k]
        k = k.strip(' ')
        doc[k] = v  
        if (isinstance(v,str)):
            #print k, v, v.isdigit()
            # #see if this string is really an int or a float
            if v.isdigit()==True: #int
                doc[k] = int(v)
            else: #try a float
                try:
                    if math.isnan(float(v))==False:
                        doc[k] = float(v) 
                except:
                    pass            
    return doc

#_______________
# format value
def formatvalue(value):
  if (isinstance(value,str)):
    # #see if this string is really an int or a float
    if value.isdigit()==True: #int
      return int(value)
    else: #try a float
      try:
        if math.isnan(float(value))==False:
          return float(value)
      except:
        pass

    return value.strip('" ') #strip off any quotations and extra spaces found in the value
  else:
    return value
    
    
def parsecommentLine(line):
    ml = line.split('/')
    for i in range(len(ml)):
      ml[i] =  ml[i].strip() #strip off any extra spaces
     
    return ml
      
def parsecomments(newdoc):
  if newdoc.has_key('comment') or newdoc.has_key('comments'):
    try:
      newdoc['comment'] = parsecommentLine(newdoc['comment'])
    except KeyError:
      try:
        newdoc['comments'] = parsecommentLine(newdoc['comments'])
      except KeyError:
        print 'it should be impossible to get here.'
    
      

#_____________
# upload
def upload(db, docs):
    db.bulk_save(docs)
    del docs
    return list()
    

#______________
# uploadFile
def uploadFile(fname, uri, dbname):
     
  print 'Upload contents of %s to %s/%s' % (fname, uri, dbname)
  
  # #connect to the db
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  
  #loop on file for upload
  reader = DictReader(open(fname, 'rU'), dialect = 'excel')
  
  #used for bulk uploading
  docs = list()
  checkpoint = 10
  n=0
  start = time.time()

  for doc in reader:
    n+=1
    #print doc
    newdoc = parseDoc(doc)
  
    if newdoc.has_key('muonmodule')==False or newdoc.has_key('end')==False \
    or newdoc.has_key('year') == False or newdoc.has_key('month') == False \
    or newdoc.has_key('day') == False:
      print 'Quitting! Your CVS Muon Veto DAQ map MUST have a column called'
      print '"module", "end", "year", "month" and "day". These are used to set the proper date and should be in UTC time'

      sys.exit(1)
    
    #reformat the date
    newdoc['date_valid'] = {} #change it to a dictionary
    yy,mm,dd = newdoc['year'], newdoc['month'], newdoc['day']
    newdoc['date_valid']['year'] = newdoc['year']
    newdoc['date_valid']['month'] = newdoc['month']
    newdoc['date_valid']['day'] = newdoc['day']

    del newdoc['year'] #remove the old fields
    del newdoc['month']
    del newdoc['day']
    
    newdoc['date_valid_unix'] = calendar.timegm( datetime.datetime(yy,mm,dd,tzinfo=pytz.utc).utctimetuple() )
    
    newdoc['type'] = 'muon_veto_daq_map'
    newdoc['author'] = 'KIT'
    newdoc['content'] = 'Muon Veto DAQ Mapping'
    dd = datetime.datetime.utcnow()
    newdoc['date_filed'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
    parsecomments(newdoc)
    
    docexists = False
    if db.doc_exist(newdoc.get('_id')):
      newdoc['_rev'] = db.get_rev(newdoc.get('_id'))
      docexists = True
      
    docs.append(newdoc)
    
    if len(docs)%checkpoint==0:
      docs = upload(db,docs)
        
  #don't forget the last batch        
  docs = upload(db,docs)
  
  #print summary statistics  
  delta = time.time() - start
  rate = float(n)/float(delta)
  ndocs = n
  print 'uploaded: %i docs in: %i seconds for a rate: %f docs/sec' % (ndocs, delta,rate)
  
  
#______________
# start script here
if __name__=='__main__':
  csvfile = sys.argv[1]
  uri = sys.argv[2]
  
  dbname = 'muonvetohardwaremap'
  if len(sys.argv) >= 4:
    dbname = sys.argv[3]
  
  uploadFile(csvfile, uri, dbname)
  


