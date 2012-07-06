#!/usr/bin/env python
from couchdbkit import Server, Database
import sys, os, subprocess, datetime, string


def main(*argv, **kwargs):
  '''                                                                                                                                                                                   
      This script will create jobs that call $KDATA_ROOT/KDataPy/scripts/dataprocess/runProc2.py
      to be run on a Sun Grid Engine based system, such as the computing center in Lyon. You must 
      provide the URI (with username/password) to the couchdb, the name of the database, and
      the directory where the standard out and standard err files will be put after processing.
      Additionally, you may add any couchdbkit-based options when it queries the database to find
      suitable data to process. This routine calls the db.view('proc/proc2') View (which reduce=False),
      along with your options. 
      
      This routine will update the database document with the metadata results of this process. 

      For example:                                                                                                                                                                            
      ./batchRunProc2.py https://edwdbuser:password@edwdbik.fzk.de:6984 datadb /sps/edelweis/kdata/dataprocessing/schedule/ key=lg23b002 

      ./batchRunProc2.py https://edwdbuser:password@edwdbik.fzk.de:6984 datadb /sps/edelweis/kdata/dataprocessing/schedule/ startkey=lg23b002  endkey=lh18c005                                                                                         
  '''
  
  s = Server(argv[0])
  db = s[argv[1]]

  if kwargs:
    print kwargs

  vr = db.view('proc/proc2', reduce=False, **kwargs)
  

  scriptDir = argv[2]
  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/runProc2.py'

  scriptOut = os.path.join(scriptDir, 'qsubout')
  scriptErr = os.path.join(scriptDir, 'qsubout')
  
  for row in vr:
    doc = db[row['id']]
    doc['status'] = 'proc2 queued'
    
    command = 'qsub -P P_edelweis -o %s -e %s -l sps=1 -l vmem=2G -l fsize=1024M  %s %s %s %s' % (scriptOut, scriptErr, script, argv[0], argv[1], row['id']) 
  
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,)
    val = proc.communicate()[0]
    if val != '':
      print val
    
    if doc.has_key('batchJob') == False:
      doc['batchJob']= []
    jobStuff = {}
    jobStuff['number'] = int(val.split(' ')[2])
    jobStuff['script'] =  script
    jobStuff['stdout'] = os.path.join(scriptOut, os.path.basename(script)) + '.o' + str(jobStuff['number'])
    jobStuff['stderr'] = os.path.join(scriptErr, os.path.basename(script)) + '.e' + str(jobStuff['number'])
    jobStuff['command'] = command
    jobStuff['type'] = 'proc2'
    jobStuff['message'] = val
    
    jobStuff['date'] = str(datetime.datetime.now())
    doc['batchJob'].append(jobStuff)
    proc = {}
    proc['batchjob'] = jobStuff['number']
    doc['proc2'] = proc
    
    db.save_doc(doc)
       
if __name__ == '__main__':

  myargs = []
  mykwargs = {}
  for arg in sys.argv[1:]:
    if string.find(arg, '=') == -1:
      myargs.append(arg)
    else:
      mykwargs[arg.split('=')[0]]=arg.split('=')[1]

  main(*myargs, **mykwargs)
