#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, datetime, string, re
from KDatatPy.uploadutilities import formatvalue
    
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



      
def parsechannels(newdoc):

  def parsechannelLine(line):
    ml = line.split('+')
    for i in range(len(ml)):
      ml[i] =  ml[i].strip() #strip off any extra spaces        
     
    return ml
    
  if newdoc.has_key('channels'):
    try:
      newdoc['channels'] = parsechannelLine(newdoc['channels'])
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
  checkpoint = 100
  n=0
  start = time.time()

  #each line in the reader object is a document that will be uploaded
  #to the databse.
  for doc in reader:
    
    #parse the doc so that integers and floats are not stored as strings
    newdoc = parseDoc(doc)
    
    #if the bolometer field is empty, skip this document
    if newdoc['bolometer'] == 0 or newdoc['bolometer'] == '':
      continue #skip to the next line in the file. this one is empty
  
    n+=1
  
    #test to make sure that all of the appropriate fields are 
    #present for this particular document type.   
    if newdoc.has_key('bolometer')==False \
    or newdoc.has_key('year') == False or newdoc.has_key('month') == False \
    or newdoc.has_key('day') == False:
      print 'Quitting! Your CSV file map MUST have the columns called'
      print '"bolometer", "Run", "year", "month" and "day". These are used to generate'
      print 'the unique document identifier "_id" in the database.'
      sys.exit(1)
      
      
    #now deal with special circumstances to convert the spreadsheet values into 
    #something a little nice for the database / analysis toosl. 
    
    #make sure bolometer characters are all uppercase to be consistent with past analysis and Samba files
    newdoc['bolometer'] = string.upper(str(newdoc['bolometer']))
    
    #strip "RUN" from the Run field
    newdoc['Run'] = formatvalue( string.replace(str(newdoc['Run']), 'RUN','').strip() )
    
    #split the repartition_number into 'type' and 'number'
    p = re.compile('[0-9]', re.IGNORECASE)
    newdoc['repartition_type'] = p.split(str(newdoc['repartition_number']), 1)[0]
    p = re.compile('[a-z]+', re.IGNORECASE)
    newdoc['repartition_number'] = formatvalue( p.split(str(newdoc['repartition_number']))[1] )
    
    #remove the bbv2_type if its empty.
    if newdoc['bbv2_type'] == '' or newdoc['bbv2_type'] == 0:  
      del newdoc['bbv2_type']
    
    
    #reformat the date
    del newdoc['date']
    newdoc['date_valid'] = {} #change it to a dictionary
    newdoc['date_valid']['year'] = newdoc['year']
    newdoc['date_valid']['month'] = newdoc['month']
    newdoc['date_valid']['day'] = newdoc['day']
    del newdoc['year'] #remove the old fields
    del newdoc['month']
    del newdoc['day']
    
    #get a nicely formated string holding the channels for this bolometer and use
    #it for the unique
    chans = str.replace(str.replace(str(newdoc['channels']),' ', ''), '+', '') #remove the spaces and the "+" sign from the channels field
    
    newdoc['_id'] = 'bbolo_position_map_' + str(newdoc['bolometer']) + '_' + chans  \
    +'_' + str(newdoc['date_valid']['year']) + '_'+ str(newdoc['date_valid']['month']) \
    +'_'+ str(newdoc['date_valid']['day'])
    
    #add the type, author, content, and date_filed fields. 
    newdoc['type'] = 'bbolo_position_map'
    newdoc['author'] = 'Bernard Paul'
    newdoc['content'] = 'Bolometer and readout electronics mapping'
    dd = datetime.datetime.utcnow()
    newdoc['date_filed'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second, 'microsecond':dd.microsecond} 
    
    #parse the channels field into an array holding the channels that are read out
    parsechannels(newdoc)
    
    #check to see if the document exists in the database already. if it
    #does, get the revision number in order to upload the document.
    if db.doc_exist(newdoc.get('_id')):
      newdoc['_rev'] = db.get_rev(newdoc.get('_id'))
    
      
    #append the newdoc to the docs list. The docs list will be uploaded
    #in 'bulk' mode, which is faster than uploading individual documents
    docs.append(newdoc)
    
    print newdoc['_id'], newdoc['bolometer'], newdoc['channels'], newdoc['date_valid']
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
  dbname = 'bolohardwaremap'
  if len(sys.argv) >= 4:
    dbname = sys.argv[3]  #should be "bolohardwaremap"
  
  uploadFile(csvfile, uri, dbname)
  


