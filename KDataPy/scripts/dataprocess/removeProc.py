#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, string
  
import argparse


def deleteFromView(server, databaseName, procname, **kwargs):
  '''
    This script will remove a "procname" key from database run documents.  You can give this script
    a single run number, or a range of runs numbers. (For now, it will remove the proc from ALL partition files within
    a single run. 
    Additionally, you can specify options to the db.view('proc/daqdoc') call by including key=value pairs to the command line args 
    kwargs should at least contain startkey and endkey
  '''

  s = Server(server)
  db = s[databaseName]

  if kwargs['startkey'] == None or kwargs['endkey'] == None:
    print 'you should supply a range of runs to use with **kwargs["startkey"] and kwargs["endkey"]'
    return

  vr = db.view('proc/daqdoc',  reduce = False, **kwargs)

  for row in vr:
    if row.has_key('doc'):
      doc = row['doc']
    else:
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

  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument('password', help='you must supply the password')
  parser.add_argument('procname', help='the name of the process key you wish to remove')
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
  
  deleteFromView(server, args.database, args.procname, **mykwargs)

