#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math

def gunzip(ifname):
    cmd = 'gunzip %s' % ifname
    print cmd
    proc = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,)
    stdout_value = proc.communicate()[0]
    stdout_value = stdout_value.strip()
    print stdout_value
    return ifname.replace('.gz','')
    

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

def upload(db, docs):
    print 'upload:\t%i' % n
    db.bulk_save(docs)
    del docs
    return list()
    
if __name__=='__main__':
  fname = sys.argv[1]
  uri = sys.argv[2]
  dbname = sys.argv[3]
  
  print 'Upload contents of %s to %s/%s' % (fname, uri, dbname)
  if (fname.endswith('.gz')):
      fname = gunzip(fname)
  
  # #connect to the db
  theServer = Server(uri)
  db = theServer.get_or_create_db(dbname)
  print db.info()
  
  #sync the views for prebuilt indices
  #loader = FileSystemDocsLoader('_design/')
  #loader.sync(db, verbose=True)
  
  #loop on file for upload
  reader = DictReader(open(fname),delimiter=' ')
  
  #used for bulk uploading
  docs = list()
  checkpoint = 100
  n=0
  start = time.time()

  for doc in reader:
    n+=1
    #print doc
    newdoc = parseDoc(doc)
    docs.append(newdoc)
    if (len(docs)%checkpoint==0):
        #print newdoc
        docs = upload(db,docs)
        
  #don't forget the last batch        
  upload(db, docs)
  
  #print summary statistics  
  delta = time.time() - start
  rate = float(checkpoint)/float(delta)
  ndocs = n
  print 'uploaded: %i docs in: %i seconds for a rate: %f docs/sec' % (ndocs, delta,rate)