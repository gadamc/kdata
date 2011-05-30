#!/usr/bin/env python

'''
  This script will secure copy files or folders to SPS for you. 
  
'''
import os, subprocess, shlex

def send(item, path):
  '''
  item can be a file or a directory and path is always relative to 
  /sps/edelweis. This script will ONLY work from two particular
  computers. The S7 machine at Modane (134.158.176.27) and the KIT 
  kalinka machine (ikkatrinsrv1.fzk.de). This is because the CC in Lyon
  has created a special account for us to use for non-interactive
  authentication via ssh key exchange. The keys for each of these computer
  have been register with CC and are only valid for connection of the user
  'adam' at Modane and the user 'edwdata' at KIT. If this needs to change,
  or we need to add users, contact CC, or me (adam.cox@kit.edu)
  and they will set us up.
  '''
  theRet = dict()
  option = ''
  if os.path.isdir(item):
    option = '-r' #add the recursive transfer
  elif os.path.isfile(item) == False:
    print 'wie bitte? item is neither a directory or a file?'
    return
  
  spspath = os.path.join('/sps/edelweis/', path)
  
  scppath = 'gadamc@edwdata.in2p3.fr:' + spspath
  
  command = '/usr/bin/scp' + ' ' + option + ' ' + item + ' ' + scppath
  print command
  
  theRet['command'] = command
  theRet['item'] = os.path.basename(item)
  theRet['spspath'] = spspath
  theRet['uname'] = os.uname()
  theRet['scpErrs'] = list()
  
  args = shlex.split(command)
  print args
  
  #the following line could throw an exception, but lets let it throw that exception
  #if it happens. The script will break, of course, but we'll know there's a problem
  p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  p.wait()
  for line in p.stdout:
    print line

  for line in p.stderr:
    print line
    theRet['scpErrs'].append(line)

  return theRet
  

def sendBoloData(item, path = 'kdata/data/current/raw/'):
  if os.path.isfile(item):
    return send(item,path) 
  else:
    print 'scpToSps.sendBoloData can only send files, not directories.'
    return dict()
    
def sendMuonData(item, path = 'kdata/data/muon/'):
  return send(item, path)
