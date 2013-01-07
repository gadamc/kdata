#!/usr/bin/env python

from couchdbkit import Server, Database
from restkit import request
import sys, string, copy, json, urllib
  
import argparse


def removeProc(server, databaseName, procname, **kwargs):
  '''
    This script will remove a "procname" key from database run document and set the doc['status'] = 'good'. 
    If "procname" == "proc0", then doc['status'] will be set to 'closed'


    You need to set the following arguments:
    procname = the name of the "procname" object that will be removed from the database documents (proc0, proc1, etc...)
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


    example: 
    
    import removeProc
    
    options = {};
    options['startkey'] = 'mj00a000_000'
    options['endkey'] = 'zz99z999';
    options['test'] = True;  #this will just be a test
    options['viewname'] = 'proc/failed'
    
    removeProc.removeProc('https://edelweissuser:password@edelweiss.cloudant.com', 'datadb', proc1', options)

   '''

  #developer note:  In the "proc" _design document now on the database, I've added two "update" function
  # and a "show" function, which could be used instead of downloading the entire document, updating it and 
  # then sending the entire thing back to the server
  #
  # I should start to use the "updates/in-place" function to put a document on the database (if I can get this to work
  #  with cloudant -- right now the request.form field is empty!  but for standard couch request.form contains valid data.)
  #

  db = Server(server)[databaseName]
  
  if kwargs['startkey'] == None or kwargs['endkey'] == None:
    print 'you must supply a range of runs to use. Set **kwargs["startkey"] and kwargs["endkey"]'
    return
  print 'using view', kwargs['viewname']

  new_status = 'good'
  if procname == 'proc0':
    new_status = 'closed'

    
  vr = db.view(kwargs['viewname'],  reduce = False, startkey=kwargs['startkey'], endkey=kwargs['endkey'])
  print len(vr), 'rows returned by the view'

  for row in vr:
    print 'checking', row['id']

    reqCom = '%s/%s/_design/proc/_update/removeproc/%s?' % (server, databaseName, row['id'] )

    reqComDic = {'procname':procname, 'new_status':new_status}
    
    if kwargs.has_key('status') and kwargs['status'] != '':
      reqComDic['old_status'] = kwargs['status']


    if kwargs.has_key('test') and kwargs['test'] != '' and  kwargs['test'] is not False:
      reqComDic['test'] = kwargs['test']

    reqCom += '&' + urllib.urlencode(reqComDic)

    print 'HTTP PUT', reqCom
    resp = request(reqCom, method='PUT')
    respj = json.loads(resp.body_string())
    print json.dumps(respj, indent=1)
    print ''
  
if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument('procname', help='[required] the name of the process key you wish to remove')
  parser.add_argument('startkey', help='[required] the name of the samba run that defines the beginning of the range of runs (example mj00a000_000')
  parser.add_argument('endkey', help='[required] the name of the samba run that defines the end of the range of runs (example zz99z999_999')
  parser.add_argument('-db', '--database', help='[optional] select the database you wish to you. "datadb" is the default', default='datadb')
  parser.add_argument('-s', '--server', help='[optional - but required if you need to use a password] select the couchdb server you wish to use. You must supply the username and password.', default='https://edelweiss.cloudant.com')
  parser.add_argument('-v', '--viewname', help='[optional] with this option you can specify which couchdb mapReduce View to use to select data files', default='proc/daqdoc')
  parser.add_argument('-o', '--status', help='[optional] documents with the status keyword set to this will only be affected', default='')
  parser.add_argument('--test', help='[optional] only test - do not save document back to database. for debugging', action='store_true')
  args = parser.parse_args()

  print 'Removing', args.procname, 'from database records on', args.server,'/',args.database, 'from startkey', args.startkey, 'to endkey', args.endkey, 'using the view', args.viewname, 'and only documents with status set to: ', args.status, '(empty means all docs)'

  if args.test: print '\nThis is only a test.\n'

  removeProc(args.server, args.database, args.procname, startkey=args.startkey, endkey=args.endkey, viewname=args.viewname, status=args.status, test=args.test)

