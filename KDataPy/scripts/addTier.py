#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os

#_____________
# upload
def upload(db, docs):
    #print 'upload:\t%i' % n
    try:
      db.bulk_save(docs)
      del docs
      return list()
    except BulkSaveError as e:
      print e.errors
      print 'couchdbkit.BulkSaveError.'
      print 'Will not delete the docs list and return to continue adding to docs'
      return docs
  
def getyeardict():
  return {'i':2008, 'j':2009, 'k':2010, 'l':2011, 'm':2012, 'n':2013, 'o':2014, 'p':2015, 'q':2016}

def getmonthnamedict():
  return {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}

def getmonthdir(sambaDirName):
  return getmonthnamedict()[sambaDirName[1]]  #assume, of course, standard samba file structure...

def getyeardir(sambaDirName):
  return getyeardict()[sambaDirName[0]]  #assume, of course, standard samba file structure...


def formatSrbYearMonthDirStructure(sambaDirName):
  basename = os.path.basename(sambaDirName)
  return str(getyeardir(basename)) + '/' + getmonthdir(basename) + '{0:02d}'.format((getyeardir(basename) - 2000)) + '/events'

def getDocsFromView(vr, tiername):

  docs = list()
     
  for row in vr:
    doc = row['doc']
    tierdict = dict()
    if tiername == 'tier0':
      tierdict['processname'] = 'rootifyAndCopyToSps'
      tierdict['file'] = '/sps/edelweiss/kdata/data/current/raw/' + os.path.basename(doc['file']) + '.root'
      tierdict['hostname'] = 'ccali.in2p3.fr'
    
    else:
      print 'only supporting tier0'
      sys.exit(-1)
    
    doc['status'] = 'good'
    doc[tiername] = tierdict
    docs.append(doc)
  
  return docs

def addTierToRun(uri, dbname, tiername, runname):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc', include_docs = True, reduce = False, key = runname)

  docs = getDocsFromView(vr, tiername)
  
  if len(docs) > 0:
    upload(db, docs)  
  
def addTierToRangeOfRuns(uri, dbname, tiername, startrun, endrun):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc', include_docs = True, reduce = False, start_key = startrun, end_key = endrun)
  
  docs = getDocsFromView(vr, tiername)
      
  if len(docs) > 0:
    upload(db, docs)

def main(*args):

  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  tiername = args[2]
  
  if len(args) == 4:
    runname = args[3]
    addTierToRun(uri, dbname, tiername, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    addTierToRangeOfRuns(uri, dbname, tiername, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])