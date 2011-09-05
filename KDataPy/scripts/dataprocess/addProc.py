#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os

  
def getyeardict():
  return {'i':2008, 'j':2009, 'k':2010, 'l':2011, 'm':2012, 'n':2013, 'o':2014, 'p':2015, 'q':2016}

def getmonthnamedict():
  return {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}

def getmonthdir(sambaDirName):
  return getmonthnamedict()[sambaDirName[1]]  #assume, of course, standard samba file structure...

def getyeardir(sambaDirName):
  return getyeardict()[sambaDirName[0]]  #assume, of course, standard samba file structure...


def formatSrbYearMonthDirStructure(sambaDirName):
  basename = os.path.basename(sambaDirName)
  return str(getyeardir(basename)) + '/' + getmonthdir(basename) + '{0:02d}'.format((getyeardir(basename) - 2000)) + '/events'

def addDocsFromView(vr, procname):
  global db
       
  for row in vr:
    print 'adding proc to', row['id']
    doc = db.get(row['id'])
    procdict = dict()
    if procname == 'proc0':
      if doc.has_key(procname):
        print 'cannot add %s record to document because it already has one', % procname
        continue
      procdict['processname'] = 'rootifyAndCopyToSps'
      procdict['file'] = '/sps/edelweiss/kdata/data/current/raw/' + os.path.basename(doc['file']) + '.root'
      procdict['hostname'] = 'ccali.in2p3.fr'
      procdict['command'] = '/usr/bin/scp '  + doc['file'] + ' gadamc@edwdata.in2p3.fr:/sps/edelweiss/kdata/data/current/raw/' + os.path.basename(procdict['file'])
      procdict['scpErrs'] = list()
      procdict['uname'] = os.uname()
    else:
      print 'only supporting proc0'
      sys.exit(-1)
    
    if db.doc_exist(doc['_id']):
      doc['_rev'] = db.get_rev(doc['_id'])
    db.save_doc(doc)

  
def addProcToRun(uri, dbname, procname, runname):
  global db
  vr = db.view('proc/daqdoc', reduce = False, key = runname)

  addDocsFromView(vr, procname)
  
  
def addProcToRangeOfRuns(uri, dbname, procname, startrun, endrun):
  global db
  vr = db.view('proc/daqdoc',  reduce = False, startkey = startrun, endkey = endrun)
  
  addDocsFromView(vr, procname)
  
def main(*args):
  '''
    This script will add a "proc" key to a database run document.  You can give this script
    a single run number, or a range of runs numbers. (For now, it will remove the proc from ALL partition files within
    a single run. 
    
    example:
    ./addProc https://edwdbuser:password@edwdbik.fzk.de:6984 edwdb proc1 lg23b002
    
    ./addProc https://edwdbuser:password@edwdbik.fzk.de:6984 edwdb proc1 lg23b002  lh18c005
    
    In this file, the addDocsFromView function needs to directly define what will be added to the document
    Currently, you'll this function only supports 'proc0', but you should explicitly check above because
    not everybody documents their code changes! :)
  '''
  
  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  procname = args[2]

  global db
  s = Server(uri)
  db = s[dbname]

  if len(args) == 4:
    runname = args[3]
    addProcToRun(uri, dbname, procname, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    addProcToRangeOfRuns(uri, dbname, procname, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])
