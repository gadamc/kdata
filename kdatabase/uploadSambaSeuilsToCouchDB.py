#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os

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

#_____________
# upload
def upload(db, docs):
    print 'upload:\t%i' % n
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
  print db.info()
  
  #sync the views for prebuilt indices
  #loader = FileSystemDocsLoader('_design/')
  #loader.sync(db, verbose=True)
  
  #loop on file for upload
  reader = DictReader(open(fname),delimiter=' ', skipinitialspace = True)
  
  #used for bulk uploading
  docs = list()
  checkpoint = 10
  n=0
  start = time.time()

  for doc in reader:
    n+=1
    #print doc
    newdoc = parseDoc(doc)
  
    newdoc.update({"_id":os.path.basename(fname) + "_" + repr(n)})

    #need to loop through all of the keys in the dictionary
    #and replace them, removing any '.' and '-' values
    #those are commonly found in the Samba files
    #and should be removed befor placing them in the database
    #this just makes using the database easier
    for k, v in newdoc.iteritems()
      del newdoc[k]
      newdoc[k.replace('.','_').replace('-','_')] = v
             
    if override==True:
      docs.append(newdoc)
    elif db.doc_exist(newdoc.get("_id"))==False:
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
  
  
def uploadDirectory(dirname, uri, dbname, override=None):
  if override is None:
    override = False
  
  #will now search in the directory dirname for Samba event folders
  #and then for _ntp files within those folders.
  #once they are found, will then pass them to the uploadFile function
  
  if os.path.isdir(dirname)==False:
    sys.exit(-1)
  
  filelist = glob.glob(dirname + '/[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]/*_ntp')
  
  for i in filelist:
    uploadFile(i, uri, dbname, override)
    
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
    print 'Will force new documents onto database without _id check'

  uploadDirectory(dirname, uri, dbname, override)
  


