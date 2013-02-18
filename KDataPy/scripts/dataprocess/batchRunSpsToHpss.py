#!/usr/bin/env python

import sys, os, subprocess, datetime, string, math
import argparse
import json, copy

def submitBatchJob(scriptOut, scriptErr, script, server, dbname, scriptOpts = ''):

  #these options will request a long node.
  command = 'qsub -P P_edelweis -o %s -e %s -l irods=1 -l sps=1 -l hpss=1 -l ct=100000 -l vmem=4G -l fsize=30G  %s %s %s %s' % (scriptOut, scriptErr, script, server, dbname, scriptOpts) 

  print command


  proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,)
  val = proc.communicate()[0]
  if val != '':
    print val
 
      
if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  
  parser.add_argument('server', help='[required] couch server, with credentials')
  parser.add_argument('databasename', help='[required] the name of database (datadb)')
  parser.add_argument('logdir',  help='[required] Location on sps where log files are placed. Should be /sps/edelweis/kdata/log/spsToHpss.')
  parser.add_argument('-o', '--options', help='[optional]. Send the string of options to spsToHpss.py script', default='') 

  args = parser.parse_args()

  script = '$KDATA_ROOT/lib/KDataPy/scripts/dataprocess/spsToHpss.py'

  scriptOut = os.path.join(args.logdir, 'qsubout')
  scriptErr = os.path.join(args.logdir, 'qsubout')
  print 'batch run sps to hpss called'
  print scriptOut, scriptErr, script, args.server, args.databasename, args.options 
  submitBatchJob(scriptOut, scriptErr, script, args.server, args.databasename, args.options) 
