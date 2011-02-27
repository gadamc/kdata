#!/usr/bin/env python

import time, sys, subprocess, math, os, glob, array, re

from ROOT import TFile, TTree, TGraph, gPad

if __name__ == '__main__':

  ''' handle the arguments'''
  filename = sys.argv[1]
  if len(sys.argv) >2:
    if os.path.isfile(sys.argv[2]):
      print 'second argument must be an output directory.'
      sys.exit(-1)
    outputDir = sys.argv[2].rstrip('/')
  else:
    outputDir = '.'
    
  if os.path.isfile(filename) == False or os.path.isdir(outputDir) == False:
    print 'first argument must be a file (the NTP ROOT file). the second argument must an output directory.'
    sys.exit(-1)


  '''process the file'''
  f = TFile.Open(filename)
  t = f.Get('ntp_tree')
  
  
  '''determine the Ampl branches'''
  branches = t.GetListOfBranches()
  detector= dict()
  
  for i in range(branches.GetEntries()):
    branch = branches.At(i).GetName()
    ret = re.match('Ampl_.',branch)
    if ret != None:
      chan = branch.split('_')[2]
      det = chan[:len(chan)-2]  #we assume here that the channel name is *IDXXXYY, where YY are letters
                              #its a bit of a hack for now... could be improved using 
                              #regular expression matching
    
      if detector.has_key(det)==False:
        detector[det] = list()
      
      if chan not in detector[det]:
        detector[det].append(branch)
    
  '''create the output file, fill graphs with TTree.Draw and save them'''
  fout = TFile.Open(outputDir + '/' + filename.strip('.root') + '_graphs.root','recreate')


  for det, branch in detector.items():
    for i in range(len(branch)):
      for j in range(i+1, len(branch)):
     
        t1 = branch[i].split('_',1)[1]  #essentially removes the Ampl part 
        t2 = branch[j].split('_',1)[1]
        
        '''always put AB before CD before GH and centre before garde'''
        chan1 = str(branch[i]).split('_')[2]
        chan1 = chan1[len(chan1)-2:]  #holds AB, CD or GH
        type1 = str(branch[i]).split('_')[1]  #holds centre, garde or chaleur
        chan2 = str(branch[j]).split('_')[2]
        chan2 = chan2[len(chan2)-2:]
        type2 = str(branch[j]).split('_')[1]
        
        normalOrder = False
        if chan1 > chan2:  #if true, then chan2/chan1 is either CD/AB, or GH/CD and switch order
          normalOrder = True
        elif chan1 == chan2:  #if the same, then check the type
          if type2 < type1:
            normalOrder = True
            
        if normalOrder:
          vars = str(branch[j]) + '/3.:' + str(branch[i]) +'/3.'  #vars is passed to TTree::Draw
        
        else:
          vars = str(branch[i]) + '/3.:' + str(branch[j]) +'/3.'  #the factor of 3 is an approximate 'calibration'
          temp = t2
          t2 = t1
          t1 = temp
          
          temp = chan2
          chan2 = chan1
          chan1 = temp
          
          temp = type2
          type2 = type1
          type1 = temp
        
        if type1 == 'centre':
          xaxisName = chan1[0]  #takes the first letter in AB (or CD or GH)
        elif type1 == 'garde':
          xaxisName = chan1[1]  #takes the second letter in AB (or CD or GH)
        else:
          xaxisName = t1
        
        if type2 == 'centre':
          yaxisName = chan2[0]
        elif type2 == 'garde':
          yaxisName = chan2[1]
        else:
          yaxisName = t2
        ''' end of ordering and axis nameing '''
        
        t.Draw(vars)
      
        gr = gPad.GetPrimitive('Graph')
       
        
        graphname = det + '_' + yaxisName + '_' + xaxisName
        graphtitle = filename.split('_')[0] + ' ' + det + ' ' + yaxisName + ' ' + xaxisName
        
        gr.GetXaxis().SetTitle(xaxisName)
        gr.GetYaxis().SetTitle(yaxisName)
        gr.SetName(graphname)
        gr.SetTitle(graphtitle)
        gr.Write()
        
  
  fout.Close()
  
  
  
  
  