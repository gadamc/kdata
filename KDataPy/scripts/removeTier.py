#!/usr/bin/env python

from couchdbkit import Server, Database
import sys

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
  
  
def deleteTierFromFile(uri, dbname, tiername, runname):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc', include_docs = True, reduce = False, key = runname)

  docs = list()
  
  for row in vr:
    doc = row['doc']
    print 'checking', doc['_id']
    if doc.has_key(tiername):
      print 'deleting', tiername, 'from', doc['_id']
      del doc[tiername]
      docs.append(doc)
  
  if len(docs) > 0:
    upload(db, docs)
  
  
def deleteTierFromListOfRuns(uri, dbname, tiername, startrun, endrun):
  s = Server(uri)
  db = s[dbname]
  
  vr = db.view('proc/daqdoc', include_docs = True, reduce = False, start_key = startrun, end_key = endrun)
  
  docs = list()
  
  for row in vr:
    doc = row['doc']
    print 'checking', doc['_id']
    if doc.has_key(tiername):
      print 'deleting', tiername, 'from', doc['_id']
      del doc[tiername]
      if tiername == 'tier0':
        doc['status'] = 'closed'
      else:
        doc['status'] = 'good'
      docs.append(doc)
  
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
    deleteTierFromFile(uri, dbname, tiername, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    deleteTierFromListOfRuns(uri, dbname, tiername, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])