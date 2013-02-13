#!/usr/bin/env python
import os, sys, time, argparse, tarfile, datetime, re
import couchdbkit, json
import KDataPy.samba_utilities as sut
import KDataPy.datadb
from KDataPy.scripts.dataprocess.KDataPyRods import KDataPyRods

'''
This script MUST be run from a node on CC in Lyon. It assumes that is where it is running and where it can find the data files.
'''


def _getHpssMonthName(sambamonth):
  doc =  {'a':'jan', 'b':'fev', 'c':'mar', 'd':'avr', 'e':'mai', 'f':'jun', 'g':'jul', 'h':'aou', 'i':'sep', 'j':'oct', 'k':'nov', 'l':'dec'}
  return doc[sambamonth]

def _getHpssLocation(tarredFileName):
  '''
    the tarredFileName must be of the form "ma+.tar" where "ma" is a Samba YM string.
    this handles individual runs like 'ma22a000.tar' and compound tar files like 'ma_small.tar'

  '''
  if re.match('^[a-z][a-l].+\.tar$', tarredFileName) is None:
    raise Exception ('_getHpssLocation failed. bad name %s', tarredFileName)
  
  basedir = '/edw/edw3rawdata'

  year = sut.yearfromsamba(tarredFileName[0])
  month = _getHpssMonthName( tarredFileName[1] ) + str(year)[2:4]

  yeardir = os.path.join(basedir, str(year))
  monthdir = os.path.join(yeardir, str(month))

  finaldir = os.path.join(monthdir, 'events')

  return os.path.join(finaldir, tarredFileName)


def _getNewestPossibleSambaRunNameToProcessFromEpoch(db, fromThisTime):
  '''
  returns the most recently available Samba run to send to hpss.
  All Samba runs with a name alphanumerically "less than" the name returned by this function
  are older than 'fromThisTime' (expressed in seconds from pc epoch) and are safe to assume have been completed runs
  and their data may be sent to IRODs

  This uses the db view 'hpss/logtime', which returns the _log documents sorted by 
  the time the data was copied to Lyon. (via the doc['metaproc0']['date_unixtime'] key value). It is  assumed that
  the _log data docs are the last documents to be transfered to Lyon (this should be the case assuming that Jules scripts on s7 
  perform as advertised and Samba successfully creates a _log file). Additionally, this function is normally used with fromThisTime set to ten days in the past, 
  increasing the likelihood that this function returns an accurate Samba run name. 

  If there are no Samba files that need to be sent to IRODs this function returns None.
  '''


  vr = db.view( 'hpss/logtime', descending=True, startkey=fromThisTime, limit=1 )

  result = vr.first()

  if result is not None:
    return result['value']
  else:
    return None

def _getListOfIdsNotOnHpssSinceSambaRun(db, sambaRun):
  
  vr = db.view('hpss/notonhpss', descending=True, reduce=False, startkey=sambaRun)
  
  runlist = []
  for row in vr:
      runlist.append(row['id'])

  return runlist


def _getListOfIdsForSambaRun(db, sambaRun):

  vr = db.view('hpss/notonhpss', descending=True, reduce=False, key=sambaRun)
  
  runlist = []
  for row in vr:
      runlist.append(row['id'])

  return runlist
  

def _getListOfAllRunsAndSizeSinceSambaRun(db, sambaRun):
  
  vr = db.view('hpss/notonhpss', descending=True, group_level=1, startkey=sambaRun)
  
  sambarunlist = []
  for row in vr:
      sambarunlist.append([row['key'], row['value']])
      #row['key'] is run_name and row['value'] is file_size

  return sambarunlist


def _getListsOfBigAndSmallRunsSinceSambaRun(sambarunlist, threshold = 200.0*1024.0*1024.0):
  
  biglist = []
  smalllist = []
  for arun in sambarunlist:
    if arun[1] > threshold: #200 MB
      biglist.append(arun[0])
    else:
      smalllist.append(arun[0])

  return biglist, smalllist


def _packageFiles(db, idList, fileName):
  
  filelist = []
  members = []
  datasize = 0
  for docid in idList:
    doc = db[docid]
    if doc.has_key('proc0'):
      thelocalfile = doc['proc0']['file']  
    elif doc.has_key('metaproc0'):
      thelocalfile = doc['metaproc0']['file']
    else:
      raise Exception("WTF! this doesn't have a proc0 or metaproc0.")
    
    filelist.append( thelocalfile )
    
    datasize += os.path.getsize( thelocalfile )
    
      

  print 'Creating Tar File', fileName
  thetarfile = tarfile.open(fileName, 'w')
  for afile in filelist:
    print 'adding to tarfile %s' % os.path.basename(afile)
    members.append(os.path.basename(afile))
    thetarfile.add(afile, arcname = os.path.basename(afile))

  thetarfile.close()
  return fileName, members, datasize


def _sendToIrods(iputOptions, tarredFile, irodFileName):
  pyrods = KDataPyRods()
  command = '-p %s' % os.path.dirname(irodFileName)
  print 'making directory:imkdir %s' % command
  pyrods.imkdir(command)
  command = '%s %s %s' % (iputOptions, tarredFile, irodFileName)
  print 'calling irods iput %s' % command
  out, err = pyrods.iput(command)
  

  if err != '':
    raise Exception(err)

  return out

def _tarAndFeather(db, idList, tarfilebasename, iputOpts=''):

  try:
    tarredFileName, members, datasize = _packageFiles(db, idList, os.path.join('/sps/edelweis/kdata/data/raw',  tarfilebasename ))
  except OSError as e:
    print e
    print '...... something happened. probably could not find a file. quiting tarAndFeader'
    print '     will need to do something with the idList. I should raise the exception here and then deal with it in the caller'
    return None

  docs = []
  hpssTarredFileName = _getHpssLocation(os.path.basename(tarredFileName))
  icommandOut = _sendToIrods(iputOpts, tarredFileName, hpssTarredFileName)

  for docid in idList:
    doc = db[docid]
    doc['hpss'] = {}
    doc['hpss']['file'] = hpssTarredFileName
    doc['hpss']['date'] = str(datetime.datetime.utcnow())
    doc['hpss']['date_unixtime'] = time.time()
    doc['hpss']['file_size'] = os.path.getsize(tarredFileName)
    doc['hpss']['icommandOut'] = icommandOut
    doc['status'] = 'good'
    docs.append(doc)
  
  
  #also, create a tarred file object on the database!
  doc = {}
  doc['type'] = 'hpsstarfiledocument'
  doc['file'] = hpssTarredFileName
  doc['_id'] = os.path.basename(doc['file']) #the file name SHOULD be unique. So, we do this to guarantee that we will observe a conflict and can then resolve it!
  doc['members'] = members
  doc['date'] = str(datetime.datetime.utcnow())
  doc['date_unixtime'] = time.time()
  doc['file_lastmodified'] = os.path.getctime(tarredFileName)
  doc['file_size'] = os.path.getsize(tarredFileName)
  doc['data_size'] = datasize
  doc['icommandOut'] = icommandOut

  docs.append(doc)
  print 'save docs', ' '.join([x['_id'] for x in docs])
  print 'datasize [MB]:', datasize/(1024.0*1024.0)
  #raw_input()
  db.bulk_save(docs)

  #remove the tarred file
  os.remove(tarredFileName)

def run(server, dbname, fromThisTime = 0, iputOpts=''):
  '''
  server - couchdb server with credentials
  dbname - couchdb database name
  fromThisTime - This function moves data from SPS to HPSS in Lyon. 
  It only moves data that were uploaded to SPS before a specific time. 
  You can specify this time here, but otherwise it defaults to 10 days before now!

  ''' 
  db = couchdbkit.Server(server)[dbname]  
  dbs = KDataPy.datadb.datadb(server, dbname)

  print str(datetime.datetime.utcnow()), 'called spsToHpss.run(%s, %s, %d)' % (server, dbname, fromThisTime)


  #first, find the newest possible Samba run that I can transfer to HPSS. I use the date to determine this
  if fromThisTime == 0: 
    defaultNumDays = 10
    fromThisTime = time.time() - defaultNumDays*3600*24  
  
  newestPossibleSamba = _getNewestPossibleSambaRunNameToProcessFromEpoch(db, fromThisTime)

  print str(datetime.datetime.utcnow()), 'setting the hpss queue status for all files since', newestPossibleSamba


  #now I get all of the database document IDs for documents that are not on hpss via the hpss/run view.
  #and I update their status to reflect that moving them to hpss is in progress.
  idSetOfDataNotOnHpss = frozenset( _getListOfIdsNotOnHpssSinceSambaRun(db, newestPossibleSamba) )
  
  for docid in idSetOfDataNotOnHpss:
    
    resp = dbs.setkey(docid, 'status', 'hpss in progress')
    resp['doc_id'] =  docid
    print json.dumps(resp, indent=1)


  #next, I get a list of all Samba Runs since before the newestPossibleSamba run above.
  #this is passed into a function that sorts these runs based on the total size of data in the run files, including meta data files
  print str(datetime.datetime.utcnow()), 'getting big and small file list'
  bigSambaRunNames, smallSambaRunNames = _getListsOfBigAndSmallRunsSinceSambaRun( _getListOfAllRunsAndSizeSinceSambaRun(db, newestPossibleSamba) )

  #bigSambaRunNames and smallSambaRunNames is a list of samba run names.

  print str(datetime.datetime.utcnow()), 'found', len(bigSambaRunNames), 'big runs and', len(smallSambaRunNames), 'small runs'

  
  idSetOfBigData = set()


  for arun in bigSambaRunNames:
    print str(datetime.datetime.utcnow()), 'big run:', arun
    
    #for each samba run name, get all of the doc _ids for that run

    idListOfBigDataDocs = _getListOfIdsForSambaRun(db, arun)

    #all of the data for each of these runs can be tarred and transfered to HPSS via irods
    #but only if a _log file is found in the list
    if arun + '_log' in idListOfBigDataDocs:
      idSetOfBigData.update(idListOfBigDataDocs)
      _tarAndFeather(db, idListOfBigDataDocs, arun+'.tar', iputOpts)
    
    
  #Now deal with the small runs.  First, I only want to deal with small files from the months prior to the 
  #month of fromThisTime since we have to wait for the month to finish to package together all of the small data files. 

  #first, need to determine the previous month's Samba YM string from the time of fromThisTime.
  #use the date fromThisTime
  ddftt = datetime.datetime.fromtimestamp(fromThisTime)
  ddlastmonth = datetime.datetime(ddftt.year, ddftt.month, 1) - datetime.timedelta(days=1) #subtract one day from the first day of the month
  #ddlastmonth is now the last day of the month prior to fromThisTime.
  #now get the samba year-month string.
  sambaMaxMonth = sut.sambayearmonth(ddlastmonth.year, ddlastmonth.month) 
  #sambaMaxMonth is something like "ml"...

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
  #idSetOfDataNotOnHpss to figure out which docs on the database need to have their status reset 
  #from 'irods in progress' back to 'good'
  idSetOfSmallData = set()

  for sambaYM, runNameList in smallSambaRunDict.iteritems():
    #sambaYM is the key and runNameList is the value, which is a list of Samba Run Names for the particular sambaYM
    #now, get all of the docids for these Samba Runs and put them in a list
    print str(datetime.datetime.utcnow()), 'small data from Samba YM:', sambaYM
    idListofSmallRunDocs = []
    for aSmallRun in runNameList:
      print '      ', aSmallRun

      #we do not check for the _log file here. This data is at least one month old, and up to 40 days old
      #if there is not a _log file already, then there never will be.
      #something bad must have happened. 
      idListofSmallRunDocs += _getListOfIdsForSambaRun(db, aSmallRun)

    if len(idListofSmallRunDocs) > 0:    
      print 'tar and feather', sambaYM+'_small.tar'
      _tarAndFeather(db, idListofSmallRunDocs, sambaYM+'_small.tar', iputOpts)
    
      idSetOfSmallData.update(idListofSmallRunDocs)


  #now, for all of the document ids that weren't sent to HPSS
  #reset the status to 'good' so that they show up again the next time
  notOnHpssSet = idSetOfDataNotOnHpss - idSetOfSmallData - idSetOfBigData
  for docid in notOnHpssSet:
    
    resp = dbs.setkey(docid, 'status', 'good')
    resp['doc_id'] = docid
    print json.dumps(resp, indent=1)      

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  
  parser.add_argument('server', help='[required] couch server, with credentials')
  parser.add_argument('databasename', help='[required] the name of database (datadb)')
  parser.add_argument('-d', '--date', type=int, default = 0, help='[optional] This script moves data from SPS to HPSS in Lyon. It only moves data that were last modified before a specific date. You can specify this date here, but otherwise it defaults to 10 days before the run function is called')
  parser.add_argument('-f', '--force', action='store_true', help='[optional] If you use this switch, then the iput command will be called with the -f (force) option. This means that files on HPSS will overwritten if they already exist.')
 
  args = parser.parse_args()

  forceOpt = ''
  if args.force:
    forceOpt = '-f'

  run(args.server, args.databasename, args.date, forceOpt) 

