#!/usr/bin/env python

from couchdbkit import Server, Database
import sys
  
def deleteFromView(vr, db, procname):
    
  for row in vr:
    doc = db.get(row['id'])
    print 'checking', doc['_id']
    if doc.has_key(procname):
      print 'deleting', procname, 'from', doc['_id']
      del doc[procname]
      if procname == 'proc0':
        doc['status'] = 'closed'
      else:
        doc['status'] = 'good'
      db.save_doc(doc)

def deleteProcFromFile(uri, dbname, procname, runname):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, key = runname)
  deleteFromView(vr, db, procname)
  
def deleteProcFromListOfRuns(uri, dbname, procname, startrun, endrun):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, startkey = startrun, endkey = endrun)
  deleteFromView(vr, db, procname)

def main(*args):
  '''
    This script will remove a "proc" key from a database run document.  You can give this script
    a single run, or a list of runs. (For now, it will remove the proc from ALL partition files within
    a single run. 
  '''
  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  procname = args[2]
  
  if len(args) == 4:
    runname = args[3]
    deleteProcFromFile(uri, dbname, procname, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    deleteProcFromListOfRuns(uri, dbname, procname, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])
