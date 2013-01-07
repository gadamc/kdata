#!/usr/bin/env python

from couchdbkit import Server, Database
import sys
  
import argparse

def resetStatus(server, databaseName, newstatus, **kwargs):
  '''
    This script will reset the "status" key from a database run document.  You must give a range of runs numbers
    by using the kwargs['startkey'] and kwargs['endkey']. You can change the status of a single run by specifing that
    run name for both 'startkey' and 'endkey'
    It will set the 'status' keyword for all documents unless you specify 'oldstatus'=='oldvalue' in the kwargs. If kwargs['oldstatus']
    is given, then only documents with 'status' = 'oldvalue' will be updated to the new status.
    (For now, it will change the status key of ALL partition files within a single run.)

    This script will reset the "status" key from a run document in the database.

    There are three options specified by this function. They are:
    
    newstatus = the value of doc['status']
    server = the full URL to the main database server (https://edelweiss.cloudant.com)
    databaseName = the name of the database (datadb)


    This script will remove the "procname" object from a subset of the database documents. 
    To specify this particular subset of documents, you must specify the following kwargs:
    
    startkey = first run name (ex. "ma22a003_003")
    endkey = final run name (ex. "mb24b001_010")
    viewname = the DB View function that you use to get the list of documents. The most general view is "proc/daqdoc". 
      You can use any of the "Views" that return the run_name + file_name as a key. These are "proc/bad", "proc/daqdoc",
      "proc/failed", "proc/inprogress", "proc/inqueue", "proc/procX", "proc/procXinprogress" (X = 1,2,3), and "proc/raw"

    All documents that are returned by the viewname within the given startkey/endkey will be affected.

    Optional kwargs:

    status = if you set kwargs['status'] then only database documents with this status will be affected
    test = if this is set to True, then no documents will be saved back to the database. All other operations will
      be performed so that you may test the use of this function before affecting docs in the database. 


  '''

  s = Server(server)
  db = s[databaseName]

  print 'kwargs:' , kwargs

  if kwargs['startkey'] == None or kwargs['endkey'] == None:
    print 'you should supply a range of runs to use with **kwargs["startkey"] and kwargs["endkey"]'
    return

  vr = db.view('proc/daqdoc',  reduce = False, startkey=kwargs['startkey'], endkey=kwargs['endkey'])

  for row in vr:
    doc = db.get(row['id'])
    print 'checking', doc['_id']

    if doc.has_key('status'):
      try:  
        if doc['status'] == kwargs['oldstatus']:
          print '   changing', kwargs['oldstatus'], 'to', newstatus, 'for', doc['_id']
          doc['status'] = newstatus
          db.save_doc(doc)
      except KeyError:
        print '   setting status to', newstatus, 'for', doc['_id']
        doc['status'] = newstatus
        db.save_doc(doc)

  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  
  parser.add_argument('newstatus', help='new status')
  parser.add_argument('startkey', help='the name of the samba run that defines the beginning of the range of runs')
  parser.add_argument('endkey', help='the name of the samba run that defines the end of the range of runs')
  parser.add_argument('-db', '--database', help='select the database you wish to use. "datadb" is the default', default='datadb')
  parser.add_argument('-s', '--server', help='select the couchdb server you wish to use. You must supply the username and password or this will not work.')
  parser.add_argument('-o', '--oldstatus', help='if this is set, only documents with the current "status" keyword equal to the value given will be updated. otherwise, all documents will have their "status" keyword updated.')

  args = parser.parse_args()

  mykwargs = {}
  mykwargs['startkey'] = args.startkey
  mykwargs['endkey'] = args.endkey
  
  print ''
  print 'CouchDB Server:', args.server
  print 'CouchDB Database:', args.database
  print 'StartKey -> EndKey:', mykwargs['startkey'], '->', mykwargs['endkey']

  if args.oldstatus is not None:
    print "Changing doc['status']:", args.oldstatus, '->', args.newstatus
    mykwargs['oldstatus'] = args.oldstatus
  else:
    print "Setting all doc['status']", args.newstatus
    pass

  print ''
  resetStatus(args.server, args.database, args.newstatus, **mykwargs)


