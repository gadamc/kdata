#!/usr/bin/env python

from couchdbkit import Server, Database
import sys, os, subprocess


def main(*argv):
  s = Server(argv[0])
  db = s[argv[1]]

  if len(argv > 2):
    vr = myProc.view('proc/proc1', reduce=False, limit = int(argv[2]) )
  else:
    vr = myProc.view('proc/proc1', reduce=False)
  
  
  scriptDir = '/sps/edelweis/kdata/dataprocessing/schedule/tenminutes'
  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/runProc1.py'

  scriptOut = os.path.join(scriptDir, 'qsubout')
  
  for row in vr:
    command = 'qsub -P P_edelweis -b y -o %s -e %s -l sps=1 -l vmem=3G -l fsize=4096M  %s %s %s' % (scriptOut, scriptOut, script, argv[0], argv[1], row['id']) 
  
    proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,)
    val = proc.communicate()[0]
    if val != '':
      print val
      
if __name__ == '__main__':
   main(*sys.argv[1:])