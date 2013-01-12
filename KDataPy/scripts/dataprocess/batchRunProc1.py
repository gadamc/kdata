#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os, subprocess, datetime, string, math
from KDataPy.uploadutilities import splitargs
import json, copy

def submitProc1BatchJobs(scriptOut, scriptErr, script, server, dbname, docids):

  if len(docids) == 0:
    return []

  command = 'qsub -P P_edelweis -o %s -e %s -l sps=1 -l ct=100000 -l vmem=4G -l fsize=20G  %s %s %s useProc0=True %s' % (scriptOut, scriptErr, script, server, dbname, ' '.join(docids) ) 
  
  proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,)
  val = proc.communicate()[0]
  if val != '':
    print val
    
  #now we need to update the 
  #database documents with this batch job submission
  #metadata
  s = Server(server)
  db = s[dbname]
    
  docs = []
  
  for anId in docids:
    doc = db[anId]
    
    if doc.has_key('batchJob') == False:
      doc['batchJob']= []
    jobStuff = {}
    jobStuff['number'] = int(val.split(' ')[2])
    jobStuff['script'] =  script
    jobStuff['stdout'] = os.path.join(scriptOut, os.path.basename(script)) + '.o' + str(jobStuff['number'])
    jobStuff['stderr'] = os.path.join(scriptErr, os.path.basename(script)) + '.e' + str(jobStuff['number'])
    jobStuff['command'] = command
    jobStuff['type'] = 'proc1'
    jobStuff['message'] = val

    jobStuff['date'] = str(datetime.datetime.now())
    doc['batchJob'].append(jobStuff)
    proc = {}
    proc['batchjob'] = jobStuff['number']
    doc['proc1'] = proc
    docs.append(doc)
  
  db.bulk_save(docs)  
  
  return []  #return an empty list
  
def main(*argv, **kwargs):
  
  s = Server(argv[0])
  db = s[argv[1]]

  print json.dumps(kwargs, indent=1)

  newkwargs = copy.deepcopy(kwargs)
  del newkwargs['maxDocs']  #remove kwargs that should not be sent to the database

  #maxDocs is increase to "infinity" here.... but can be set by a kwargs
  maxDocs = -1  
  try:
    maxDocs = kwargs['maxDocs']
  except:
    pass

  vr = db.view('proc/proc1', reduce=False, **newkwargs )
    
  scriptDir = argv[2]
  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/runProc1.py'

  scriptOut = os.path.join(scriptDir, 'qsubout')
  scriptErr = os.path.join(scriptDir, 'qsubout')
  
  

  #for process 1, we can submit up to 100 files to be processed
  #by a particular batch job. This is because this is a relatively fast process
  #to convert a Samba file to a KData file and we want to play nice with 
  #the batch system in Lyon
  
  docids = []
  
  for row in vr:
    doc = db[row['id']]
    print row['id'], 'proc1 queued'
    doc['status'] = 'proc1 queued'
    db.save_doc(doc) 
    docids.append(row['id'])
    
    if len(docids) == maxDocs:
      docids = submitProc1BatchJobs(scriptOut, scriptErr, script, argv[0], argv[1], docids)
      
  #don't forget last chunk of data files
  docids = submitProc1BatchJobs(scriptOut, scriptErr, script, argv[0], argv[1], docids)



      
if __name__ == '__main__':
  
  (myargs, mykwargs) = splitargs( sys.argv[1:] )
  main(*myargs, **mykwargs)
  
