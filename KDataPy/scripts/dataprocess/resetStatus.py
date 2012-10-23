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
  '''

  s = Server(server)
  db = s[databaseName]

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


