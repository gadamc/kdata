#!/usr/bin/env python

from csv import DictReader
import time, sys, subprocess, math, os, array, re, numpy as np

from ROOT import TFile, TTree

#______________
# parseDoc
def parseDoc(doc):
    for k,v in doc.items():
        k = k.replace('.','_')  #remove those pesky "."
        if (isinstance(v,str)):
            #print k, v, v.isdigit()
            # #see if this string is really an int or a float
            if v.isdigit()==True: #int
                doc[k] = int(v)
            else: #try a float
                try:
                    if math.isnan(float(v))==False:
                        doc[k] = float(v) 
                except:
                    pass
        
    return doc
    
    
def formatname(aname):
  return aname.replace(':','').replace('(','').replace(')','').replace('-','_')
  
#_______________
def buildRootFile(fname, outputdir):
  

  if outputdir.rstrip('/') == '.':
    outFname = os.path.basename(fname) + '.root'
  else:
    outFname = outputdir.rstrip('/') + '/' + os.path.basename(fname) +'.root'
    
  print 'building root file from', fname, '>> output:', outFname
  
  #loop on file for upload
  file = open(fname)
  reader = DictReader(open(fname), delimiter=' ', skipinitialspace = True)
  
  try:
      doc = parseDoc(reader.next()) #read the first line
  except Exception as e:
      print e
      return None

  #here, i search through the key/value pairs of doc trying to determine
  #the type of value and then creating a list of variable names, an array for 
  #each variable stored in a list (necessary for TTree branching), and then
  #a TTree branch format descriptor 
  varNames = list()
  arrayList = list()
  descriptor = list()
  for k, v in doc.items():
    #print k, v
    name = formatname(k)
    
    if isinstance(v,str)==False:
      if isinstance(v, int): #int
          varNames.append(name)
          if name == 'Stamp' or name == 'Position' or name == 'GigaStamp' or name == 'Evt' or re.match('List',name) or re.match('Date',name):
            arrayList.append(np.arange(1, dtype=np.uint32))
            descriptor.append(str(name) + '/i')
          else:
            arrayList.append(array.array('i',[0]))  #we have to use arrays because of the way that Python deals with memory and the way that TTrees deal with memory
            descriptor.append(str(name) + '/I')
      else: #must be a float
          try:
            if math.isnan(float(v))==False:
              varNames.append(name) 
              arrayList.append(array.array('f',[0]))
              descriptor.append(str(name) + '/F')
          except:
            pass
        
    else:  #must be a string
      # we're skipping strings.
      #varNames.append(name)
      #arrayList.append(array.array('i',[0]))
      #descriptor.append(str(name) + '/C')
      pass

  file = TFile.Open(outFname, 'recreate')
  tree = TTree('ntp_tree','A Tree based on the Samba NTP files.')
  
  #print varNames
  #print arrayList
  #print descriptor
  
  for i in range(len(arrayList)): #set up the branches
    tree.Branch(varNames[i],arrayList[i],descriptor[i]) 
  
  #re-read the file so that we start at the first line
  reader = DictReader(open(fname), delimiter=' ', skipinitialspace = True)
  try:
    for line in reader:
      #print 'next line'
      line = parseDoc(line)
      for k, v in line.items():  
        name = formatname(k)    
        try:                      
          i = varNames.index(name)  #its not guaranteed the the order of key/value pair is
                                  #maintained. So, we have to use the list.index function
                                  #to find the proper index for this particular key
          try:
            arrayList[i][0] = v       #set the value to the proper array  (arrayList[i] returns an array and arrayList[i][0] is the zero'th element of the array)
          except OverflowError:
            print i
            print k, v
            raise OverflowError

            #print k, v
          #print i, arrayList[i][0]
        except ValueError:
          pass  #this will throw if varNames doesn't have an index named 'name' In the code above,
              #strings are ignored. so when we come across a key that isn't in our list,
              #which is probably a string, we ignore it here.
      #print 'fill'
      tree.Fill()
  except Exception as e:
    print e
      
  file.cd()
  tree.Write()
  file.Close()
  
  return outFname
  
def buildDirectory(dirname, outputpath):
    #will now search in the directory dirname for Samba event folders
  #and then for _ntp files within those folders.
  #once they are found, will then pass them to the uploadFile function
  
  if os.path.isdir(dirname)==False:
    sys.exit(-1)


  regex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]_ntp'
  
  for dirpath, dirs, files in os.walk(dirname):
    for f in files:
      if re.match(regex, f) and re.search('.root', f) == None:
        buildRootFile(os.path.join(dirpath, f), outputpath)
  
def main(*argv):
  name = argv[0]
  outputdir = argv[1]
 
  if os.path.isfile(name):
    buildRootFile(name, outputdir)
    
  elif os.path.isdir(name):
    buildDirectory(name, outputdir)
  
#______________
# start script here
if __name__=='__main__':
  main(*sys.argv[1:])


