#!/usr/bin/env python

from couchdbkit import Server, Database
import sys
  
import argparse

def resetStatus(server, databaseName, oldstatus, newstatus, **kwargs):
  '''
    This script will reset the "status" key from a database run document.  You must give a range of runs numbers
    by using the kwargs['startkey'] and kwargs['endkey']. You can change the status of a single run by specifing that
    run name for both 'startkey' and 'endkey'

    (For now, it will change the status key of ALL partition files within a single run.)
  '''

  s = Server(server)
  db = s[databaseName]

  if kwargs['startkey'] == None or kwargs['endkey'] == None:
    print 'you should supply a range of runs to use with **kwargs["startkey"] and kwargs["endkey"]'
    return

  vr = db.view('proc/daqdoc',  reduce = False, **kwargs)

  for row in vr:
    doc = db.get(row['id'])
    print 'checking', doc['_id']
    if doc.has_key('status'):
      if doc['status'] == oldstatus:
        print 'changing', oldstatus, 'to', newstatus, 'for', doc['_id']
        #doc['status'] = newstatus
        #db.save_doc(doc)

  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument('password', help='you must supply the password')
  parser.add_argument('oldstatus', help='current status')
  parser.add_argument('newstatus', help='new status')
  parser.add_argument('startkey', help='the name of the samba run that defines the beginning of the range of runs')
  parser.add_argument('endkey', help='the name of the samba run that defines the end of the range of runs')
  parser.add_argument('-u', '--username', default='edwdbuser')
  parser.add_argument('-db', '--database', help='select the database you wish to you. "datadb" is the default', default='datadb')
  parser.add_argument('-s', '--server', help='select the couchdb server you wish to you.', default='edwdbik.fzk.de')
  parser.add_argument('-p', '--port', help='select the couchdb server you wish to you.', default='6984')

  args = parser.parse_args()

  mykwargs = {}
  mykwargs['startkey'] = args.startkey
  mykwargs['endkey'] = args.endkey


  protocol = 'https'
  if args.port == '5984':
    protocol = 'http'
  server = '%s://%s:%s@%s:%s' % (protocol, args.username, args.password, args.server, args.port)
 

  print server
  print args.oldstatus, '->', args.newstatus
  print mykwargs['startkey'], '->', mykwargs['endkey']

  resetStatus(server, args.database, args.oldstatus, args.newstatus, **mykwargs)


