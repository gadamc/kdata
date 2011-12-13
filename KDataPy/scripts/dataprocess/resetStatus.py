#!/usr/bin/env python

from couchdbkit import Server, Database
import sys
  
def resetStatusFromView(vr, db, oldStatus, newStatus):
    
  for row in vr:
    doc = db.get(row['id'])
    print 'checking', doc['_id']
    if doc.has_key('status'):
      if doc['status'] == oldStatus:
        print 'changing', oldStatus, 'to', newStatus, 'for', doc['_id']
        doc['status'] = newStatus
        db.save_doc(doc)

def resetStatusForFile(uri, dbname, oldStatus, newStatus, runname):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, key = runname)
  resetStatusFromView(vr, db, oldStatus, newStatus)
  
def resetStatusFromListOfRuns(uri, dbname, oldStatus, newStatus, startrun, endrun):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc',  reduce = False, startkey = startrun, endkey = endrun)
  resetStatusFromView(vr, db, oldStatus, newStatus)

def main(*args):
  '''
    This script will reset the "status" key from a database run document.  You can give this script
    a single run number, or a range of runs numbers. (For now, it will remove the proc from ALL partition files within
    a single run. 
    
    example:
    ./resetStatus https://edwdbuser:password@edwdbik.fzk.de:6984 datadb 'old status' 'new status' lg23b002
    
    ./resetStatus https://edwdbuser:password@edwdbik.fzk.de:6984 datadb 'old status' 'new status' lg23b002  lh18c005
  '''
  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  oldStatus = args[2]
  newStatus = args[3]
  
  if len(args) == 5:
    runname = args[4]
    resetStatusForFile(uri, dbname, oldStatus, newStatus, runname)
  
  elif len(args) == 6:
    startrun = args[4]
    endrun = args[5]
    resetStatusFromListOfRuns(uri, dbname, oldStatus, newStatus, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])
