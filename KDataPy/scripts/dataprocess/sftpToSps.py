#!/usr/bin/env python
import pexpect
import datetime
import sys
import os, subprocess, shlex, re

def send(username, password, item, path):
    '''
    item must be a file. sends the item to the path on the /sps/edelweis directory on CC in Lyon
    '''

    try:
        theRet = dict()
        option = ''
        if os.path.isdir(item):
            option = '-r' #add the recursive transfer
        elif os.path.isfile(item) == False:
            print 'wie bitte? item is neither a directory or a file?'
            return
  
        spspath = os.path.join(path ,os.path.basename(item))
        theRet['transfer_method'] = 'sftp'
        theRet['command'] = 'put'
        theRet['file'] = spspath
        theRet['file_size'] = os.path.getsize(item)
        theRet['hostname'] = 'ccage.in2p3.fr'
        theRet['localuname'] = os.uname()
        theRet['stdOut'] = ''
        
        startTime = datetime.datetime.now()
        print startTime
        p = pexpect.spawn('/usr/bin/sftp %s@%s' % (username, theRet['hostname']) )
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
            theRet['log'] = p.logfile
            print p.logfile
            theRet['pexpect_obj'] = p.__str__()
            p.sendline('bye')
            p.close()
        elif i==2:
            theRet['scpStdOut'] = 'fail'
            print "I either got key or connection timeout"
            pass
        
        return theRet

    except Exception as e:
        print 'Exception in sftpToSps.py'
        print e
        raise e


def sendBoloData(username, password, item, path = '/sps/edelweis/kdata/data/raw/'):
    if os.path.isfile(item):
        return send(item,path, username, password) 
    else:
        print 'sftpToSps.sendBoloData can only send files, not directories.'
        return dict()
    


