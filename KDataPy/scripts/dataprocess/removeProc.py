#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, string, copy
  
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
  print kwargs['viewname']
  vr = db.view(kwargs['viewname'],  reduce = False, startkey=kwargs['startkey'], endkey=kwargs['endkey'])
  print len(vr)

  for row in vr:
    if row.has_key('doc'):
      doc = row['doc']
    else:
      doc = db.get(row['id'])
    
    print 'checking', doc['_id'], 'status: ', doc['status']
    if doc.has_key(procname):
      if kwargs['status'] == '':
        print 'deleting', procname, 'from', doc['_id']
        del doc[procname]
      elif kwargs['status'] == doc['status']:
        print 'deleting', procname, 'from', doc['_id']
        del doc[procname]
      
      if procname == 'proc0':
        doc['status'] = 'closed'
      else:
        doc['status'] = 'good'

      if kwargs['test'] == False: db.save_doc(doc)
      else: print '     debugging: document not saved'
  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument('procname', help='the name of the process key you wish to remove')
  parser.add_argument('startkey', help='the name of the samba run that defines the beginning of the range of runs (example mj00a000_000')
  parser.add_argument('endkey', help='the name of the samba run that defines the end of the range of runs (example zz99z999_999')
  parser.add_argument('-db', '--database', help='select the database you wish to you. "datadb" is the default', default='datadb')
  parser.add_argument('-s', '--server', help='select the couchdb server you wish to use. You must supply the username and password.', default='https://edelweiss.cloudant.com')
  parser.add_argument('-v', '--viewname', help='with this option you can specify which couchdb mapReduce View to use to select data files', default='proc/daqdoc')
  parser.add_argument('-o', '--status', help='documents with the status keyword set to this will only be affected', default='')
  parser.add_argument('--test', help='only test - do not save document back to database. for debugging', action='store_true')
  args = parser.parse_args()

  print 'Removing', args.procname, 'from database records on', args.server,'/',args.database, 'from startkey', args.startkey, 'to endkey', args.endkey, 'using the view', args.viewname, 'and only documents with status set to: ', args.status, '(empty means all docs)'

  if args.test: print '\nThis is only a test.\n'

  deleteFromView(args.server, args.database, args.procname, startkey=args.startkey, endkey=args.endkey, viewname=args.viewname, status=args.status, test=args.test)

