#!/usr/bin/env python
import pexpect
import datetime
import sys
import os, subprocess, shlex, re, tempfile
from KDataPy.exceptions import *

def send(username, password, item, path):
  '''
  item must be a file. sends the item to the path on CC in Lyon
  '''
  try:
    theRet = dict()
    option = ''
    if os.path.isfile(item) == False:
      raise KDataTransferError('KDataTransferError. sftpToSps.send can only send files, not directories.\n')
  
    spspath = os.path.join(path ,os.path.basename(item))
    theRet['transfer_method'] = 'sftp'
    theRet['command'] = 'put'
    theRet['file'] = spspath
    theRet['file_size'] = os.path.getsize(item)
    theRet['file_size_mb'] = os.path.getsize(item)/1024./1024.
    theRet['hostname'] = 'ccage.in2p3.fr'
    theRet['localuname'] = os.uname()
    theRet['stdOut'] = ''
        
    startTime = datetime.datetime.utcnow()
    print startTime
    p = pexpect.spawn('/usr/bin/sftp %s@%s' % (username, theRet['hostname']) , timeout=3600*24)
    fout = tempfile.NamedTemporaryFile(delete=False)
    foutName = fout.name
    p.logfile_read = fout
    ssh_newkey = 'Are you sure you want to continue connecting'
    i=p.expect([ssh_newkey,'password:',pexpect.EOF])
    if i==0:
      print 'accepting key'
      p.sendline('yes')
      i=p.expect([ssh_newkey,'password:',pexpect.EOF])
    if i==1:
      print startTime, 'sending password'
      p.sendline(password)
      p.expect('sftp> ')
      p.sendline("put %s %s" % (item, spspath) )
      p.expect('sftp> ')
      p.sendline("ls -al %s" % (spspath,) )
      p.expect('sftp> ')
      theRet['pexpect_obj'] = p.__str__()
      p.sendline('bye')
      fout.close()
      p.close()
      with open(foutName,'r') as fff:
          theRet['log'] = fff.read()
    elif i==2:
      print "I either got key or connection timeout"
      try:
        fout.close()
      except:
        pass
      finally:
        try:
          os.unlink(foutName)
        except:
          pass
        
      raise KDataTransferError('KDataTransferError. sftpToSps.py line57. key or connection timeout.\n')
        
    return theRet

  except Exception as e:
    try:
      fout.close()
    except:
      pass
    finally:
      try:
        os.unlink(foutName)
      except:
        pass
        
    
    raise KDataTransferError('KDataTransferError. sftpToSps.py line64  \n' + str(type(e)) + ' : ' + str(e))



def sendBoloData(username, password, item, path = '/sps/edelweis/kdata/data/raw/'):
  if os.path.isfile(item):
    try:
      return send(username, password, item , path) 
    except Exception as e:
      raise KDataTransferError('KDataTransferError. sftpToSps.py line73  \n' + str(type(e)) + ' : ' + str(e))
  else:
    raise KDataTransferError('KDataTransferError. sftpToSps.py sftpToSps.sendBoloData can only send files, not directories.\n')

    


