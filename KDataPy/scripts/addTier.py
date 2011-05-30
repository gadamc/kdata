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

def addDocsFromView(vr, tiername):
  global db
       
  for row in vr:
    print 'adding tier to', row['id']
    doc = db.get(row['id'])
    tierdict = dict()
    if tiername == 'tier0':
      tierdict['processname'] = 'rootifyAndCopyToSps'
      tierdict['file'] = '/sps/edelweiss/kdata/data/current/raw/' + os.path.basename(doc['file']) + '.root'
      tierdict['hostname'] = 'ccali.in2p3.fr'
      tierdict['command'] = '/usr/bin/scp '  + doc['file'] + ' gadamc@edwdata.in2p3.fr:/sps/edelweiss/kdata/data/current/raw/' + os.path.basename(tierdict['file'])
      tierdict['scpErrs'] = list()
      tierdict['uname'] = os.uname()
    else:
      print 'only supporting tier0'
      sys.exit(-1)
    
    doc['status'] = 'good'
    doc[tiername] = tierdict
    db.save_doc(doc)

  
def addTierToRun(uri, dbname, tiername, runname):
  global db
  vr = db.view('proc/daqdoc', reduce = False, key = runname)

  addDocsFromView(vr, tiername)
  
  
def addTierToRangeOfRuns(uri, dbname, tiername, startrun, endrun):
  global db
  vr = db.view('proc/daqdoc',  reduce = False, startkey = startrun, endkey = endrun)
  
  addDocsFromView(vr, tiername)
  
def main(*args):
  
  
  if len(args) < 4:
    return Done
    
  uri = args[0]
  dbname = args[1]
  tiername = args[2]

  global db
  s = Server(uri)
  db = s[dbname]

  if len(args) == 4:
    runname = args[3]
    addTierToRun(uri, dbname, tiername, runname)
  
  elif len(args) == 5:
    startrun = args[3]
    endrun = args[4]
    addTierToRangeOfRuns(uri, dbname, tiername, startrun, endrun)

if __name__ == '__main__':
  main(*sys.argv[1:])
