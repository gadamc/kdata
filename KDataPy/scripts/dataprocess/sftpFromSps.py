#!/usr/bin/env python

#sftpFromSps.py
#!/usr/bin/env python
import pexpect
import datetime
import sys
import os, subprocess, shlex, re, tempfile
from KDataPy.exceptions import *

def get(username, password, localpath, spspath):
  '''
  gets the spspath from ccage.in2p3.fr and copies it to the localpath
  localpath must be a directory
  '''
  try:
    theRet = dict()
  
    if os.path.isdir(localpath) is False: 
      print 'this tool requires that you give the localpath to the file/files you wish to sftp from SPS.'
      return None

    #spspath = os.path.join(path ,os.path.basename(item))
    theRet['transfer_method'] = 'sftp'
    theRet['command'] = 'mget'
    theRet['mget'] = spspath
    theRet['localpath'] = localpath
    theRet['hostname'] = 'ccage.in2p3.fr'
    theRet['localuname'] = os.uname()
    theRet['stdOut'] = ''
        
    startTime = datetime.datetime.now()
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
      p.sendline("mget %s %s" % (spspath, localpath) )
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
        
      raise KDataTransferError('KDataTransferError. sftpFromSps.py line57. key or connection timeout.\n')
        
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
        
    
    raise KDataTransferError('KDataTransferError. sftpFromSps.py line64  \n' + str(type(e)) + ' : ' + str(e))



def getBoloData(username, password, runName, localpath='.', spsrawdatadir = '/sps/edelweis/kdata/data/raw/'):
  try:
    return get(username, password, localpath , os.path.join(spsrawdatadir, runName)) 
  except Exception as e:
    raise KDataTransferError('KDataTransferError. sftpFromSps.py line73  \n' + str(type(e)) + ' : ' + str(e))
  
    



