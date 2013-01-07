#!/usr/bin/env python

from couchdbkit import Server, Database
from restkit import request
import sys, string, copy, json, urllib

import argparse

def resetStatus(server, databaseName, newstatus, **kwargs):
  '''
    This script will reset the "status" key from a database run document.  You must give a range of runs numbers
    by using the kwargs['startkey'] and kwargs['endkey']. You can change the status of a single run by specifing that
    run name for both 'startkey' and 'endkey'
    It will set the 'status' keyword for all documents unless you specify 'status'=='oldvalue' in the kwargs. If kwargs['status']
    is given, then only documents with 'status' = 'oldvalue' will be updated to the new status.
    (For now, it will change the status key of ALL partition files within a single run.)

    This script will reset the "status" key from a run document in the database.

    There are three options explicitly specified by this function. They are:
    
    newstatus = the value of doc['status']
    server = the full URL to the main database server (https://<username>:<password>@edelweiss.cloudant.com)
    databaseName = the name of the database (datadb)


    This script will affect only a subset of the database documents. 
    To specify this particular subset of documents, you MUST specify the following kwargs:
    
    startkey = first run name (ex. "ma22a003_003")
    endkey = final run name (ex. "mb24b001_010")
    viewname = the DB View function that you use to get the list of documents. The most general view is "proc/daqdoc". 
      You can use any of the "Views" that return the run_name_file_name as a key (i.e. ma22a003_003). 
      These are "proc/bad", "proc/daqdoc", "proc/failed", "proc/inprogress", "proc/inqueue", "proc/procX", 
      "proc/procXinprogress" (X = 1,2,3), and "proc/raw"

    All documents that are returned by the viewname within the given startkey/endkey range will be affected.

    Optional kwargs:

    status = if you set kwargs['status'] then only database documents with this status will have their status changed.

    test = if this is set to True, then no documents will be saved back to the database. All other operations will
      be performed so that you may test the use of this function before affecting docs in the database. 


    example: 
    
    import resetStatus
    
    options = {};
    options['startkey'] = 'mj00a000_000'
    options['endkey'] = 'zz99z999';
    options['test'] = True;  #this will just be a test
    options['viewname'] = 'proc/inqueue'
    options['status'] = 'proc1 queued'

    #reset these status values to 'good'
    resetStatus.resetStatus('https://edelweissuser:password@edelweiss.cloudant.com', 'datadb', 'good', options)

  '''

  db = Server(server)[databaseName]

  print 'kwargs:' , kwargs

  if kwargs['startkey'] == None or kwargs['endkey'] == None:
    print 'you should supply a range of runs to use with **kwargs["startkey"] and kwargs["endkey"]'
    return

  vr = db.view(kwargs['viewname'],  reduce = False, startkey=kwargs['startkey'], endkey=kwargs['endkey'])

  for row in vr:
    print 'checking', row['id']

    reqCom = '%s/%s/_design/proc/_update/resetstatus/%s' % (server, databaseName, row['id'] )

    reqComDic = {'new_status':newstatus}

    if kwargs.has_key('status') and kwargs['status'] != '':
      reqComDic['old_status'] = kwargs['status']

    if kwargs.has_key('test') and kwargs['test'] != '' and  kwargs['test'] is not False:
      reqComDic['test'] = kwargs['test']

    reqCom += '?' + urllib.urlencode(reqComDic)

    print 'HTTP PUT', reqCom
    resp = request(reqCom, method='PUT')
    respj = json.loads(resp.body_string())
    print json.dumps(respj, indent=1)
    print ''



  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  
  parser.add_argument('newstatus', help='new status')
  parser.add_argument('startkey', help='the name of the samba run that defines the beginning of the range of runs')
  parser.add_argument('endkey', help='the name of the samba run that defines the end of the range of runs')
  parser.add_argument('-db', '--database', help='select the database you wish to use. "datadb" is the default', default='datadb')
  parser.add_argument('-v', '--viewname', help='[optional] with this option you can specify which couchdb mapReduce View to use to select data files', default='proc/daqdoc')
  parser.add_argument('-s', '--server', help='select the couchdb server you wish to use. You must supply the username and password or this will not work.')
  parser.add_argument('-o', '--status', help='if this is set, only documents with the current "status" keyword equal to the value given will be updated. otherwise, all documents will have their "status" keyword updated.')
  parser.add_argument('--test', help='[optional] only test - do not save document back to database. for debugging', action='store_true')

  args = parser.parse_args()

  mykwargs = {}
  mykwargs['startkey'] = args.startkey
  mykwargs['endkey'] = args.endkey
  mykwargs['viewname'] = args.viewname

  print ''
  print 'CouchDB Server:', args.server
  print 'CouchDB Database:', args.database
  print 'StartKey -> EndKey:', mykwargs['startkey'], '->', mykwargs['endkey']

  if args.status is not None:
    print "Changing doc['status']:", args.status, '->', args.newstatus
    mykwargs['status'] = args.status
  else:
    print "Setting all doc['status']", args.newstatus
    pass

  print ''
  resetStatus(args.server, args.database, args.newstatus, **mykwargs)


