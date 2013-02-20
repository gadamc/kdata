
import datetime
import KDataPy.uploadutilities as util
import re

def sambadateregex():
  '''
    returns a regular expression string that matches a samba date string.
    something like 'ma22', for example.
    you can use this in the python re module.
  '''
  return '^[a-z]{1}[a-l]{1}(((0)[1-9])|((1|2)[0-9])|(3[0-1]))$'

def sambarunnameregex():
  '''
    returns a regular expression string that matches a samba run string.
    something like 'ma22a012', for example.
    you can use this in the python re module.
  '''
  return sambadateregex().split('$')[0] + '[a-z]{1}[0-9]{3}$'  

def sambadatafileregex():
  '''
    returns a regular expression string that matches a samba file (aka 'partition') string.
    something like 'ma22a012_023', for example.
    you can use this in the python re module.
  '''
  return sambarunnameregex().split('$')[0] + '_[0-9]{3}$'

def sambametadatafileregex():
  '''
    returns a regular expression string that matches a samba meta data file string.
    something like 'ma22a012_BB', '_ntp', or '_log' for example. It only matches _BB, _ntp and _log.
    you can use this in the python re module.
  '''
  return sambarunnameregex().split('$')[0] + '_(BB|log|ntp|seuils|setup\.csv)$'

def isvalidsambadate(sambadate):
  '''
    Test if sambadate is a valid date for Samba file names. Such as ma22, or lk14. 

  '''
  if re.match( sambadateregex() , sambadate) is None: 
    return False
  else: 
    return True

def isvalidsambadatafilename(name):
  '''
    Test if name is a valid name for Samba file (aka partition) names. Such as ma22000_012

  '''
  if re.match( sambadatafileregex() , name) is None: 
    return False
  else: 
    return True

def isvalidsambarunname(name):
  '''
    Test if name is a valid name for Samba run names. Such as ma22000

  '''
  if re.match( sambarunnameregex() , name) is None: 
    return False
  else: 
    return True

def isvalidsambametadatafilename(name):
  '''
    Test if name is a valid name for Samba meta data file names. Such as ma22000_BB (or ntp or log)

  '''
  if re.match( sambametadatafileregex() , name) is None: 
    return False
  else: 
    return True

def sambamac(name):
  '''
    given name is either valid samba datafile name, metafile name or run name, returns
    the Samba mac number, such as 1, 2, 5, etc.. where a = 1, b = 2, etc..
    returns None if it fails for some reason
  '''
  if isvalidsambadatafilename(name) or isvalidsambametadatafilename(name) or isvalidsambarunname(name):
    return daqfromsamba(name[4])

def sambahote(name):
  '''
    given name is either valid samba datafile name, metafile name or run name, returns
    the Samba "Hote" name, such as s1, s2, s5, etc..
  '''
  macnum = sambamac(name)
  if macnum is not None:
    return 's' + str(macnum)


def sambayear(year):
  '''
    returns the Samba date format year string value for a given year.
    For example, if year = 2012, this function will return 'm'
    The input arguemnt, year, must be greater than or equal to 2000 and less than 2026
  '''
  year = util.formatvalue(year)
  expmsg = '%s, must be integer greater than or equal to 2000 and less than 2026' % year

  if isinstance(year, str) or isinstance(year, float):
    raise TypeError(expmsg)

  if year < 2000 or year >= 2026:
    raise TypeError(expmsg)

  return chr( year - 2000 + ord('a') )

def sambamonth(month):
  '''
    returns the Samba date format month string value for a given month.
    For example, if month = 1, this function will return 'a'.
    The input argument, month, must be number from 1 to 12
  '''
  month = util.formatvalue(month)
  expmsg = '%s, must be integer between 1 and 12, inclusively' % month

  if isinstance(month, str) or isinstance(month, float):
    raise TypeError(expmsg)

  if month < 1 or month > 12:
    raise TypeError(expmsg)

  return chr( month - 1 + ord('a') )

def sambayearmonth(year, month):
  '''
    returns the Samba date format year-month string value for a given year and month.
    For example, if year = 2012, and month = 2, this function will return 'mb'.
    The input arguments follow the rules of getsambayear, and getsambamonth.
  '''
  return sambayear(year) + sambamonth(month)

def sambadate(year, month, day):
  '''
    returns the Samba date format year-month0day string value for a given year, month and day.
    For example, if year = 2012, and month = 2, and day = 12 this function will return 'mb12'.
    The input arguments follow the rules of getsambayear, and getsambamonth.
  '''
  return sambayearmonth(year, month) + str(day)

def getcurrentsambayear(): 
  '''
    returns the Samba date format year string value for right now.
  '''
  return sambayear(datetime.datetime.now().year)

def sambamonthnow():
  '''
    returns the Samba date format month string value for right now.
  '''
  return sambamonth(datetime.datetime.now().month)

def sambayearmonthnow():
  '''
    returns the Samba date format year-month string value for right now.
  '''
  n = datetime.datetime.now()
  return sambayearmonth(n.year, n.month)

def sambadatenow():
  '''
    returns the Samba date format year-month-day string value for right now.
  '''
  n = datetime.datetime.now()
  return sambadate(n.year, n.month, n.day)


def monthfromsamba(month):
  '''
    returns month number between 1 and 12 for a give Samba date format month string.
    The input argument, month, must be a string between a and l, inclusively.
  '''

  if re.match( '^[a-l]{1}$', month) is None:
    raise TypeError( '%s, must be a string between a and l, inclusively' % month )

  return ord(month) - ord('a') + 1

def yearfromsamba(year):
  '''
    returns year number between 2000 and 2025 for a give Samba date format year string.
    The input argument, year, must be a string between a and z, inclusively.
  '''

  if re.match('^[a-z]{1}$', year) is None:
    raise TypeError( '%s, must be a string between a and z, inclusively' % year )

  return ord(year) - ord('a') + 2000

def daqfromsamba(name):
  '''
    returns daq number between 1 and 26 for a give Samba DAQ machine letter.
    The input argument, name, must be a string between a and z, inclusively.
  '''

  if re.match( '^[a-z]{1}$', name) is None:
    raise TypeError( '%s, must be a string between a and z, inclusively' % name )

  return ord(name) - ord('a') + 1

def datetimeobjectfromsamba(date):
  '''
    returns a datetime object for a given Samba date format year-month-day string.
    For example, the input argument, date, must look something like ma22
  '''

  if not isvalidsambadate(date):
    raise TypeError( '%s, must be a valid samba date string, like ma22, or lk15' % date )

  return datetime.datetime(yearfromsamba(date[0]), monthfromsamba(date[1]), int(date[2:4]))


