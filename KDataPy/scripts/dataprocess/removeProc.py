#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, string
  
def deleteFromView(*args, **kwargs):
    
  s = Server(args[0])
  db = s[args[1]]
  procname = args[2]

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

def main(*args, **kwargs):
  '''
    This script will remove a "proc" key from a database run document.  You can give this script
    a single run number, or a range of runs numbers. (For now, it will remove the proc from ALL partition files within
    a single run. 
    Additionally, you can specify options to the db.view('proc/daqdoc') call. 

    for example:
    ./removeProc https://edwdbuser:password@edwdbik.fzk.de:6984 datadb proc1 key=lg23b002
    
    ./removeProc https://edwdbuser:password@edwdbik.fzk.de:6984 datadb proc1 startkey=lg23b002  endkey=lh18c005
  '''
  if len(args) < 3:
    return Done
  
  print kwargs
   
  deleteFromView(*args, **kwargs)
  
if __name__ == '__main__':

  myargs = []
  mykwargs = {}
  for arg in sys.argv[1:]:
    if string.find(arg, '=') == -1:
      myargs.append(arg)
    else:
      mykwargs[arg.split('=')[0]]=arg.split('=')[1]
      
  main(*myargs, **mykwargs)

