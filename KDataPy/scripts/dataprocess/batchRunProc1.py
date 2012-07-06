#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os, subprocess, datetime, string


def submitBatchJob(scriptOut, scriptErr, script, server, dbname, docids):

  if len(docids) == 0:
    return []

  command = 'qsub -P P_edelweis -o %s -e %s -l sps=1 -l vmem=1G -l fsize=1024M  %s %s %s %s' % (scriptOut, scriptErr, script, server, dbname, ' '.join(docids) ) 
  
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
  vr = db.view('proc/proc1', reduce=False, **kwargs )
    
  scriptDir = argv[2]
  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/runProc1.py'

  scriptOut = os.path.join(scriptDir, 'qsubout')
  scriptErr = os.path.join(scriptDir, 'qsubout')
  
  maxDocs = 30  #for process 1, we can submit up to 30 files to be processed
  #by a particular batch job. This is because this is a relatively fast process
  #to convert a Samba file to a KData file and we want to play nice with 
  #the batch system in Lyon
  
  docids = []
  
  for row in vr:
    doc = db[row['id']]
    doc['status'] = 'proc1 queued'
    db.save_doc(doc) 
    docids.append(row['id'])
    
    if len(docids) == maxDocs:
      docids = submitBatchJob(scriptOut, scriptErr, script, argv[0], argv[1], docids)
      
  #don't forget last chunk of data files
  docids = submitBatchJob(scriptOut, scriptErr, script, argv[0], argv[1], docids)
      
if __name__ == '__main__':
  myargs = []
  mykwargs = {}
  for arg in sys.argv[1:]:
    if string.find(arg, '=') == -1:
      myargs.append(arg)
    else:
      mykwargs[arg.split('=')[0]]=arg.split('=')[1]

  main(*myargs, **mykwargs)
  
