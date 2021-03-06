import string

# format value
def formatvalue(value):
  '''
    This function reformats 'value' in an expected way. 
    It tests if value is a string, and tries to convert it into an integer or float, 
    returning the integer of float value if possible.
    If 'value' is a string, it strips off any whitespace or double quotation marks.
    This function is used extensively to format values found in the Samba header or
    in various Excel spreadsheets that are uploaded to our databases. 
  '''
  if (isinstance(value,str) or isinstance(value, unicode)):
    # #see if this string is really an int or a float
    if value.isdigit()==True: #int
      return int(value)
    else: #try a float
      try:
        if math.isnan(float(value))==False:
          return float(value)
      except:
        pass

    return value.strip('" ') #strip off any quotations and extra spaces found in the value
  else:
    return value


#splitargs
def splitargs(argv):
  '''
  A simple arg parser. This is useful because argparse is not installed on some Python systems with version < 2.7.
  This supports two kinds of arguments.
  1. a single value
  2. a key=value pair. 

  For example, this works
  "option1 option2 keyword1=value1 keyword2=value2 "

  But this function doesn't support "switches" that have values like "-k 5". you must use the "=" sign and NO spaces.
  
  It returns a tuple of ( [args], {kwargs} ).  In the example above, for example it would return

  ( ['option1', 'option2'], {'keyword1':'value1', 'keyword2':'value2'})
  

  disclaimer:  It is highly recommended that you test this function before completely trusting what it does!!
  If you want a real tool for parsing arguments, you should use/install argparse.

  '''
  
  myargs = []
  mykwargs = {}
  for arg in argv:
    if string.find(arg, '=') == -1:
      print arg
      myargs.append(arg)
    else:
      key, val = tuple(arg.split('='))
      
      if val in ('True', 'true'):
        val = True
      elif val in ('False', 'false'):
        val = False
      else:
        val = formatvalue(val)
     
      print key,val
      mykwargs[key] = val

  return (myargs, mykwargs)
