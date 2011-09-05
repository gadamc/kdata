#!/usr/bin/env python

'''
  This script will secure copy files or folders to SPS for you. 
  
'''
import os, subprocess, shlex, re

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
  
  spspath = os.path.join('/sps/edelweis/', path + os.path.basename(item))
  
  scppath = 'gadamc@edwdata.in2p3.fr:' + spspath
  
  command = '/usr/bin/scp -q ' + ' ' + option + ' ' + item + ' ' + scppath
  print command
  
  theRet['scpcommand'] = command
  theRet['file'] = spspath
  theRet['hostname'] = 'ccali.in2p3.fr'
  theRet['localuname'] = os.uname()
  theRet['scpErrs'] = list()
  
  #the following line could throw an exception, but lets let it throw that exception
  #if it happens. The script will break, of course, but we'll know there's a problem
  print 'running secure copy'
  
  p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  p.wait()
  for line in p.stdout:
    print line

  for line in p.stderr:
    print line
    if re.search('cannot read file system information for',line) == False and re.search('missing second argument',line) == False:
      #skip these errors... for some reason scp to in2p3 doesn't find the file
      theRet['scpErrs'].append(line)
  
  print 'returning', theRet
  return theRet
  

def sendBoloData(item, path = 'kdata/data/current/raw/'):
  if os.path.isfile(item):
    return send(item,path) 
  else:
    print 'scpToSps.sendBoloData can only send files, not directories.'
    return dict()
    
def sendMuonData(item, path = 'kdata/data/muon/'):
  return send(item, path)
