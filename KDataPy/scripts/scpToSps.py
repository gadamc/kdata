#!/usr/bin/env python

'''
  This script will secure copy files or folders to SPS for you. 
  
'''
import os, subprocess, shlex

def send(item, path):
  '''
  item can be a file or a directory and path is always relative to 
  /sps/edelweis. This script will ONLY work from two particular
  computer. The S7 machine at Modane (134.158.176.27) and the KIT 
  kalinka machine (ikkatrinsrv1.fzk.de). This is because the CC in Lyon
  has created a special account for us to use for non-interactive
  authentication via ssh key exchange. The keys for each of these computer
  have been register with CC and are only valid for connection of the user
  'adam' at Modane and the user 'edwdata' at KIT. If this needs to change,
  or we need to add users, contact CC, or me (adam.cox@kit.edu)
  and they will set us up.
  '''
  option = ''
  if os.path.isdir(item):
    option = '-r' #add the recursive transfer
  elif os.path.isfile(item) == False:
    print 'wie bitte? item is neither a directory or a file?'
    return
  
  pathToSps = 'gadamc@edwdata.in2p3.fr:' + os.path.join('/sps/edelweis/', path)
  if option != '':
    command = '/usr/bin/scp' + ' ' + option + ' ' + item + ' ' + pathToSps
  else:
    command = '/usr/bin/scp' + ' ' + item + ' ' + pathToSps
  print command

  args = shlex.split(command)
  print args
  try:
    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    for line in p.stdout:
      print line

    for line in p.stderr:
      print line

  except Exception as e:
    print 'an exception occured', e
    

def sendBoloData(item, path = 'kdata/data/current/raw/'):
  if os.path.isfile(item):
    send(item,path) 
  else:
    print 'scpToSps.sendBoloData can only send files, not directories.'
    
def sendMuonData(item, path = 'kdata/data/muon/'):
  send(item, path)
