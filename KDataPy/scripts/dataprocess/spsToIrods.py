#!/usr/bin/env python

import os, sys, time, argparse, tarfile
import couchdbkit
import KDataPy.samba_utilities as sut
import KDataPy.datadb

'''
This script MUST be run from a node on CC in Lyon. It assumes that is where it is running and where it can find the data files.
'''

def _getIrodsMonthName(sambamonth):
  doc =  {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}
  return doc[sambamonth]

def _getIrodLocation(tarredFileName):
  '''
    the tarredFileName must be of the form "ma+.tar" where "ma" is a Samba YM string.
    this handles individual runs like 'ma22a000.tar' and compound tar files like 'ma_small.tar'

  '''
  if re.match('^[a-z][a-l].+(.tar)', tarredFileName) is None:
    raise Exception ('_getIrodLocation failed. bad name %s', tarredFileName)
  
  basedir = '/edw/edw3rawdata'

  year = sut.yearfromsamba(tarredFileName[0])
  month = _getIrodsMonthName( tarredFileName[1] ) + str(year)[2:4]

  yeardir = os.path.join(basedir, year)
  monthdir = os.path.join(yeardir, month)

  finaldir os.path.join(monthdir, 'events')

  return finaldir


def _getNewestPossibleSambaRunNameToProcessFromEpoch(db, fromThisTime):
  '''
  returns the most recently available Samba run to send to IRODs.
  All Samba runs with a name alphanumerically "less than" the name returned by this function
  are older than 'fromThisTime' (expressed in seconds from pc epoch) and are safe to assume have been completed runs
  and their data may be sent to IRODs

  This uses the db view 'irods/metatime', which returns the meta data documents (_BB, _log, _ntp) sorted by 
  the time the data was copied to Lyon. (via the doc['metaproc0']['date_unixtime'] key value). It is somewhat assumed that
  the meta data docs are the last documents to be transfered to Lyon (this should be the case assuming that Jules scripts on s7 
  perform as advertised). Additionally, this function is normally used with fromThisTime set to ten days in the past, 
  increasing the likelihood that this function returns an accurate Samba run name. 

  If there are no Samba files that need to be sent to IRODs this function returns None.
  '''


  vr = db.view( 'irods/metatime', descending=True, startkey=fromThisTime, limit=1 )

  result = vr.first()

  if result is not None:
    return result['value']
  else:
    return None

def _getListOfIdsNotOnIrodsSinceSambaRun(db, sambaRun):
  
  vr = db.view('irods/runs', descending=True, reduce=False, startkey=sambaRun)
  
  runlist = []
  for row in vr:
      runlist.append([row['id'])

  return runlist


def _getListOfIdsForSambaRun(db, sambaRun):
  
  vr = db.view('irods/runs', descending=True, reduce=False, key=sambaRun)
  
  runlist = []
  for row in vr:
      runlist.append([row['id'])

  return runlist


def _getListOfAllRunsSinceSambaRun(db, sambaRun):
  
  vr = db.view('irods/runs', descending=True, group_level=1, startkey=sambaRun)
  
  sambarunlist = []
  for row in vr:
      sambarunlist.append([row['key'], row['value']])

  return sambarunlist


def _getListsOfBigAndSmallRunsSinceSambaRun(sambarunlist):
  
  biglist = []
  smalllist = []
  for arun in sambarunlist:
    if arun[1] > 200*1024*1024 #200 MB
      biglist.append(arun[0])
    else:
      smalllist.append(arun[0])

  return biglist, smalllist



def _getFileName(fileList):
  #figures out how to name the tarred file
  pass


def _packageFiles(db, idList, fileName):
  
  filelist = []
  for docid in idList:
    doc = db[docid]
    if doc.has_proc('proc0'):
      filelist.append(doc['proc0']['file'])
    elif doc.has_proc('metaproc0'):
      filelist.append(doc['metaproc0']['file'])
    else:
      raise Exception("WTF! this doesn't have a proc0 or metaproc0.")

  print 'Creating Tar File', fileName
  thetarfile = tarfile.open(fileName, 'w')
  for afile in filelist:
    thetarfile.add(afile, arcname = os.path.basename(afile))

  thetarfile.close()
  return fileName


def _sendToIrods(tarredFile, irodFileName):
  


def _tarAndFeather(db, idList, tarfilebasename):

  tarredFileName = _packageFiles(db, idList, os.path.join('/sps/edelweis/kdata/data/raw',  tarfilebasename ))
  docs = []
  for docid in idList:
    doc = db[docid]
    doc['irods'] = {}
    doc['irods']['file'] = _getIrodLocation(tarredFileName)
    doc['irods']['date'] = str(datetime.datetime.utcnow())
    doc['irods']['date_unixtime'] = time.time()
    doc['irods']['file_size'] = os.path.getsize(tarredFileName)
    doc['status'] = 'good'
    docs.append(doc)
  
  _sendToIrods(tarredFileName, _getIrodLocation(tarredFileName))

  db.bulk_save(docs)


def run(server, dbname, fromThisTime = 0):
  '''
  server - couchdb server with credentials
  dbname - couchdb database name
  fromThisTime - This function moves data from SPS to HPSS in Lyon. 
  It only moves data that were uploaded to SPS before a specific time. 
  You can specify this time here, but otherwise it defaults to 10 days before now!

  ''' 
  db = couchdbkit.Server(server)[dbname]  
  dbs = KDataPy.datadb.datadb(kwargs['server'], kwargs['database'])



  #first, find the newest possible Samba run that I can transfer to HPSS. I use the date to determine this
  if fromThisTime == 0: 
    defaultNumDays = 10
    fromThisTime = time.time() - defaultNumDays*3600*24  
  
  newestPossibleSamba = _getNewestPossibleSambaRunNameToProcessFromEpoch(db, fromThisTime)

  print str(datetime.datetime.utcnow()), 'setting the irods queue status for all files since', newestPossibleSamba


  #now I get all of the database document IDs for documents that are not on irods via the irods/runs view.
  #and I update their status to reflect that moving them to irods is in progress.
  idSetOfDataNotOnIrods = frozenset( _getListOfIdsNotOnIrodsSinceSambaRun(db, newestPossibleSamba) )
  
  for docid in idSetOfDataNotOnIrods:
    resp = dbs.setkey(docid, 'status', 'irods in progress')
    print resp, '\n'


  #next, I get a list of all Samba Runs since before the newestPossibleSamba run above.
  #this is passed into a function that sorts these runs based on the total size of data in the run files, including meta data files
  print str(datetime.datetime.utcnow()), 'getting big and small file list'
  bigSambaRunNames, smallSambaRunNames = _getListsOfBigAndSmallRunsSinceSambaRun( _getListOfAllRunsSinceSambaRun(db, newestPossibleSamba) )

  #bigSambaRunNames and smallSambaRunNames is a list of samba run names.

  print 'found', len(bigSambaRunNames), 'big runs and', len(smallSambaRunNames), 'small runs'

  
  idSetOfBigData = set()

  for arun in bigSambaRunNames:
    
    #for each samba run name, get all of the doc _ids for that run 
    idListOfBigDataDocs = _getListOfIdsForSambaRun(db, arun)
    
    idSetOfBigData.update(idListOfBigDataDocs)

    #all of the data for each of these runs can be tarred and transfered to HPSS via IRods
    _tarAndFeather(db, idListOfBigDataDocs, arun+'.tar')

    

  #Now deal with the small runs.  First, I only want to deal with small files from the months prior to the current
  #month since we have to wait for the month to finish to package together all of the small data files. 

  #first, need to determine the previous month's Samba YM string.
  ddnow = datetime.datetime.utcnow()
  ddlastmonth = datetime.datetime(ddnow.year, ddnow.month, 1) - datetime.timedelta(days=1) #subtract one day from the first day of the current month
  #ddlastmonth is now the last day of the last month, from which we get the samba year-month string.
  sambaMaxMonth = sut.sambayearmonth(ddlastmonth.year, ddlastmonth.month) 
  #sambaMaxMonth is something like "ml"...

  #make sure that the run is from the previous month than the current month. 
  #need to group these run names in the smallSambaRunNames according to their samba date
  #can do this by creating a dictionary like { "mk" : [], "ml": [], "mj": [], ....}
  smallSambaRunDict = {}

  for arun in smallSambaRunNames:
    if arun[0:2] <= sambaMaxMonth:
      
      if smallSambaRunDict.has_key(arun[0:2]) is False:
        smallSambaRunDict[arun[0:2]] = []

      smallSambaRunDict[arun[0:2]].append(arun)
  


  #I'm going to keep the set of all of the database document _ids that I copy to HPSS
  #then I can use this list, along with the idSetOfBigData and compare it to the 
  #idSetOfDataNotOnIrods to figure out which docs on the database need to have their status reset 
  #from 'irods in progress' back to 'good'
  idSetOfSmallData = set()

  for sambaYM, runNameList in smallSambaRunDict.iteritems():
    #sambaYM is the key and runNameList is the value, which is a list of Samba Run Names for the particular sambaYM
    #now, get all of the docids for these Samba Runs and put them in a list
    idListofSmallRunDocs = []
    for aSmallRun in runNameList:
      idListofSmallRunDocs += _getListOfIdsForSambaRun(db, aSmallRun)

    _tarAndFeather(db, idListofSmallRunDocs, sambaYM+'_small.tar')
    
    idSetOfSmallData.update(idListofSmallRunDocs)


  #now, for all of the document ids that weren't sent to HPSS
  #reset the status to 'good' so that they show up again the next time
  notOnIrodsSet = idSetOfDataNotOnIrods - idSetOfSmallData - idSetOfBigData
  for docid in notOnIrodsSet:
    resp = dbs.setkey(docid, 'status', 'good')
    print resp, '\n'    

   

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  
  parser.add_argument('server', help='[required] couch server, with credentials')
  parser.add_argument('databasename', help='[required] the name of database (datadb)')
  parser.add_argument('-d', '--date', type=int, default = 0, 
            help='[optional] This script moves data from SPS to HPSS in Lyon. It only moves data that were uploaded to SPS before a specific time. You can specify this time here, but otherwise it defaults to 10 days before now!')
 
  args = parser.parse_args()

  run(args.server, args.databasename, args.date) 

