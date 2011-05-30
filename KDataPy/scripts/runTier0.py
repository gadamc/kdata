#!/usr/bin/env python

from TierProcess import *
import os, sys, tempfile
import rootifySambaData as rt
import scpToSps as scp



def rootifyAndScp(*args, **kwargs):
  
  if len(args) > 1:
    print 'takes just one argument... the file name.'
    sys.exit(-1)
    
  tempDir = tempfile.mkdtemp()
  outputFile = os.path.join(tempDir, args[0])
  
  #rootify the File into a KData File!
  theReturn = rt.convertfile(args[0], outputFile)

  if theReturn != outputFile:
    print 'something happened with rootification'
    sys.exit(-1)
    
  #now send it via secure copy!
  scpRet = scp.sendBoloData(outputFile)
  
  #clean up
  shutil.rmtree(tempDir)
  
  return scpRet
  
  
def main(*argv):
  '''
  argv[0] is the server (http://127.0.0.1:5984)
  argv[1] is the database (edwdb)
  '''
  
  #create a TierProcess instance, which will assist in uploading the tier
  #document to the database
  myTier = TierProcess(argv[0], argv[1], rootifyAndScp)
  
  vr = myTier.view('proc/tier0', include_docs=True, reduce=False)
  
  for row in vr:
    doc = row['doc']
    
    tierDict = myProcessor.doprocess(doc['file']) #this step calls rootfiyAndScp
    
    if len(tierDict) > 0:
      #add a few more items to the document
      
      tierDict['processname'] = 'rootifyAndCopyToSps'
      
      if len(tierDict['scpErrs']) > 0:
        doc['status'] = 'bad'
      
      #this step will add the tierDict dictionary to the 
      #database document and then upload it to the DB
      doc['tier0'] = tierDict
      myProcessor.upload(doc)
      
    else:
      print 'the process returned an empty dictionary!'
      sys.exit(-1)
    


if __name__ == '__main__':
  main(*sys.argv[1:])
