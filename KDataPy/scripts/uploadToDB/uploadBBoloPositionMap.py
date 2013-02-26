#!/usr/bin/env python

from couchdbkit import Server, Database
from couchdbkit.loaders import FileSystemDocsLoader
from csv import DictReader
import time, sys, subprocess, math, os, datetime, string, re, pytz, calendar
from KDataPy.uploadutilities import formatvalue
    
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

  #each line in the reader object is a document that will be uploaded
  #to the databse.
  for doc in reader:
    
    #parse the doc so that integers and floats are not stored as strings
    newdoc = parseDoc(doc)
  
    #enforce some sort of schema. that is, require the existence of a set of keys in the database documents
    requiredSet = set(['bolometer', 'channels', 'repartition_number', 'year', 'month', 'day'])
    if (requiredSet  < set(newdoc.keys())) is False:
      print 'Quitting! Your CVS file map MUST have the following columns'
      print ', '.join([x for x in requiredSet])
      sys.exit(1)

    #if the bolometer field is empty, skip this document
    if newdoc['bolometer'] == 0 or newdoc['bolometer'] == '':
      continue #skip to the next line in the file. this one is empty
      
    #now deal with special circumstances to convert the spreadsheet values into 
    #something a little nicer for the database / analysis tools. 
    
    #make sure bolometer characters are all uppercase to be consistent with past analysis and Samba files
    newdoc['bolometer'] = string.upper(str(newdoc['bolometer']))
    
    #strip "RUN" from the Run field
    newdoc['Run'] = formatvalue( string.replace(str(newdoc['Run']), 'RUN','').strip() )
        
    #reformat the date
    del newdoc['date']
    #reformat the date to a single dictionary
    newdoc['date_valid'] = {} 
    for dk in ['year', 'month', 'day', 'hour', 'minute']:
      try:
        newdoc['date_valid'][dk] = newdoc[dk]
        del newdoc[dk]  
      except: pass

    yy,mm,dd = newdoc['date_valid']['year'], newdoc['date_valid']['month'], newdoc['date_valid']['day']
    try:
      hh = newdoc['date_valid']['hour']
    except KeyError:
      hh = 0
    try: 
      min =  newdoc['date_valid']['minute']
    except KeyError:
      min = 0

    validdate = datetime.datetime(yy,mm,dd,hh, min, tzinfo=pytz.utc)
    newdoc['date_valid_unixtime'] = calendar.timegm( validdate.utctimetuple() )
    newdoc['date_valid_isoformat'] = validdate.isoformat() 

      
    #add the type, author, content, and date_filed fields. 
    newdoc['type'] = 'bbolo_position_map'
    newdoc['author'] = 'Bernard Paul'
    newdoc['content'] = 'Bolometer and readout electronics mapping'
    dd = datetime.datetime.utcnow()
    newdoc['date_filed'] = {'year':dd.year, 'month':dd.month, 'day':dd.day, 'hour':dd.hour, 'minute':dd.minute, 'second':dd.second} 
    newdoc['date_filed_unixtime'] = time.time()
    newdoc['date_filed_isoformat'] = dd.isoformat() + '+0:00'  #add the +0:00 to indicate clearly that this is UTC

    #parse the channels field into an array holding the channels that are read out
    parsechannels(newdoc)
    
    #append the newdoc to the docs list. The docs list will be uploaded
    #in 'bulk' mode, which is faster than uploading individual documents
    docs.append(newdoc)
    
    print newdoc['bolometer'], newdoc['channels'], newdoc['date_valid_isoformat']
         
  db.bulk_save(docs)
  
  
  
#______________
# start script here
if __name__=='__main__':
  csvfile = sys.argv[1]
  uri = sys.argv[2]
  dbname = 'bolohardwaremap'

  if len(sys.argv) >= 4:
    dbname = sys.argv[3]  
  
  uploadFile(csvfile, uri, dbname)
  


