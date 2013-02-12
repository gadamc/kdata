import subprocess

class KDataPyRods():

  def __init__(self):
    '''
      
      This just wraps around subprocess to call i-commands for irods. 
      It returns the stdout and stderr from the commands that it supports. 

      It does not run iinit. So, you should run this first within the shell of the account
      that you want to use this class. 

      Make sure to read the Irods documentation 

      Note - these functions wait for the i-command to completely finish. It doesn't poll stdout/stderr and print
      to screen as the i-command returns to stdout/stderr. So, its not interactive. If you want to run interactive
      commands in the shell from within Python, use the subprocess module's Popen function and continously poll 
      the output (http://stackoverflow.com/questions/4760215/running-shell-command-from-python-and-capturing-the-output)
      For example, the 'iinit' command is interactive because it requires you to enter a password. If you run 'iinit' with 
      this class's _icommand method, it will just sit there until the Universe explodes (or you type Ctrl-C).
      I could code in this functionality, but why bother until its needed...
      However, I currently keep stdout and stderr separate. This way, if there's any message coming from stderr,
      the user of this class can raise an Exception (which is what I do inside of spsToHpss.py)

    '''
    pass

  def _icommand(self, command):
    if isinstance(command, str) is False:
      raise TypeError('command was not a string: %s' % command)

    proc =  subprocess.Popen( [command] , shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = proc.communicate() #this forces to wait for completion of subprocess
    return out, err

  def ils(self, commandargs = ''):
    return self._icommand('ils %s' % commandargs)

  def icd(self, commandargs = ''):
    return self._icommand('icd %s' % commandargs)

  def imkdir(self, commandargs):
    return self._icommand('imkdir %s' % commandargs)

  def iput(self, commandargs):
    return self._icommand('iput %s' % commandargs)

  def iget(self, commandargs):
    return self._icommand('iget %s' % commandargs)