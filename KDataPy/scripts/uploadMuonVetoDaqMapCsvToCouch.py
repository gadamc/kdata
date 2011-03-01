#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, datetime

#_____________
# gunzip
def gunzip(ifname):
    cmd = 'gunzip %s' % ifname
    print cmd
    proc = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,)
    stdout_value = proc.communicate()[0]
    stdout_value = stdout_value.strip()
    print stdout_value
    return ifname.replace('.gz','')
    
#______________
# parseDoc
def parseDoc(doc):
    for k,v in doc.items():
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
def uploadFile(fname, uri, dbname, override=None):
  if override is None:
    override = False
    
  print 'Upload contents of %s to %s/%s' % (fname, uri, dbname)
  if (fname.endswith('.gz')):
    fname = gunzip(fname)
  
  # #connect to the db
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  #print db.info()
  
  #sync the views for prebuilt indices
  #loader = FileSystemDocsLoader('_design/')
  #loader.sync(db, verbose=True)
  
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
  
    if newdoc.has_key('module')==False or newdoc.has_key('end')==False:
      print 'Quitting! Your CVS Muon Veto DAQ map MUST have a column called'
      print '"module" and a column called "end". These are used to generate'
      print 'the unique document identifier "_id" in the database.'
      sys.exit(1)
    
    newdoc.update({'_id':'muon_veto_daq_map_module_' + str(newdoc['module']) + '_end_' + str(newdoc['end'])})	
    newdoc['type'] = 'muon_veto_daq_map'
    newdoc['author'] = 'KIT'
    newdoc['content'] = 'Muon Veto DAQ Mapping'
    dd = datetime.datetime.utcnow()
    newdoc['date_filed'] = [dd.year,dd.month,dd.day,dd.hour,dd.minute,dd.second,dd.microsecond,0] # the 0 indicates the time zone relative to UTC
    parsecomments(newdoc)
    
    docexists = False
    if db.doc_exist(newdoc.get('_id')):
      newdoc['_rev'] = db.get_rev(newdoc.get('_id'))
      docexists = True
      
    if override==True:
      docs.append(newdoc)
    elif docexists==False:
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
  dirname = sys.argv[1]
  uri = sys.argv[2]
  dbname = sys.argv[3]
  
  if len(sys.argv)>=5:
    if sys.argv[4]=='True':
      override = True   
    else:
      override = False
  else:
    override = False
  
  if override==True:
    print 'Will force add new documents, causing a new revision for duplicate documents'

  uploadFile(dirname, uri, dbname, override)
  


