#!/usr/bin/env python

from couchdbkit import Server, Database
import sys
  
def deleteFromView(vr, db, tiername):
    
  for row in vr:
    doc = db.get(row['id'])
    print 'checking', doc['_id']
    if doc.has_key(tiername):
      print 'deleting', tiername, 'from', doc['_id']
      del doc[tiername]
      if tiername == 'tier0':
        doc['status'] = 'closed'
      else:
        doc['status'] = 'good'
      db.save_doc(doc)

def deleteTierFromFile(uri, dbname, tiername, runname):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, key = runname)
  deleteFromView(vr, db, tiername)
  
def deleteTierFromListOfRuns(uri, dbname, tiername, startrun, endrun):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, startkey = startrun, endkey = endrun)
  deleteFromView(vr, db, tiername)

def main(*args):
  '''
    This script will remove a "tier" key from a database run document.  You can give this script
    a single run, or a list of runs. (For now, it will remove the tier from ALL partition files within
    a single run. 
  '''
  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  tiername = args[2]
  
  if len(args) == 4:
    runname = args[3]
    deleteTierFromFile(uri, dbname, tiername, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    deleteTierFromListOfRuns(uri, dbname, tiername, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])
