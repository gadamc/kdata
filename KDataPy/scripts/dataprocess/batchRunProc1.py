#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os, subprocess, datetime


def main(*argv):
  s = Server(argv[0])
  db = s[argv[1]]

  if len(argv) > 3:
    vr = db.view('proc/proc1', reduce=False, limit = int(argv[3]) )
  else:
    vr = db.view('proc/proc1', reduce=False)
  
  
  scriptDir = argv[2]
  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/runProc1.py'

  scriptOut = os.path.join(scriptDir, 'qsubout')
  
  for row in vr:
    doc = db[row['id']]
    doc['status'] = 'proc1 queued'
    db.save_doc(doc)
    
    command = 'qsub -P P_edelweis -b y -o %s -e %s -l sps=1 -l vmem=3G -l fsize=4096M  %s %s %s %s' % (scriptOut, scriptOut, script, argv[0], argv[1], row['id']) 
  
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,)
    val = proc.communicate()[0]
    if val != '':
      print val
    
    if db.has_key('batchJob') == False:
      db['batchJob']= []
    jobStuff = {}
    jobStuff['type'] = 'proc1'
    jobStuff['message'] = val
    jobStuff['number'] = int(val.split(' ')[3])
    jobStuff['date'] = datetime.datetime.now()
    db.['batchJob'].append(jobStuff)
    
    db.save_doc(doc)  

if __name__ == '__main__':
   main(*sys.argv[1:])
