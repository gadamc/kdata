#!/usr/bin/env python

import time, sys, subprocess, math, os, glob, array, re

from ROOT import TFile, TTree, TGraph, gPad, gROOT, TMath #,SetOwnership

def main(*args):
  
  gROOT.SetBatch(True)
    
  ''' handle the arguments'''
  filename = args[0]
  if len(args) > 1:
    if os.path.isfile(args[1]):
      print 'second argument must be an output directory.'
      sys.exit(-1)
    outputDir = args[1].rstrip('/')
  else:
    outputDir = '.'
  
  try:
    if os.path.isfile(filename) == False or os.path.isdir(outputDir) == False:
      print 'first argument must be a file (the NTP ROOT file). the second argument must an output directory.'
      return None
  except Exception as e:
    print e
    return None

  '''open the file'''
  f = TFile.Open(filename)
  t = f.Get('ntp_tree')
  #SetOwnership(f, False)
  #SetOwnership(t, False)
  
  '''determine the Ampl branches'''
  branches = t.GetListOfBranches()
  detector= dict()
  #SetOwnership(branches, False)
  
  for i in range(branches.GetEntries()):
    branch = branches.At(i).GetName()
    ret = re.match('Ampl_.',branch)
    if ret != None:
      try:
        if chan.find('veto') != -1:
          continue  #skip muon veto channels

        chan = branch.split('_')[2]
        det = chan[:len(chan)-2]  #we assume here that the channel name is *IDXXXYY, where YY are letters
                              #its a bit of a hack for now... could be improved using 
                              #regular expression matching
      except:
        continue  #if the branch name isn't how we expect it, then ignore and continue. This could be from the GCC detector, which are heat only

      if detector.has_key(det)==False:
        detector[det] = list()
      
      if chan not in detector[det]:
        detector[det].append(branch)
    
  '''create the output file, fill graphs with TTree.Draw and save them'''
  foutname = outputDir + '/' + os.path.basename(filename).strip('.root') + '_graphs.root'
  fout = TFile.Open(foutname,'recreate')
  graphsout = dict()
  #SetOwnership(fout, False)
  
  for det, branch in detector.items():
    for i in range(len(branch)):
      for j in range(i+1, len(branch)):
     
        if graphsout.has_key(det) == False:
          graphsout[det] = list()
        
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
        graphtitle = os.path.basename(filename).split('_')[0] + ' ' + det + ' ' + yaxisName + ' ' + xaxisName
        fout.cd()
        gr.GetXaxis().SetTitle(xaxisName)
        gr.GetYaxis().SetTitle(yaxisName)
        gr.SetName(graphname)
        gr.SetTitle(graphtitle)
        gr.Write()
        graphsout[det].append(graphname)
        
  
  for det, branch in detector.items():
    
    if graphsout.has_key(det) == False:
          graphsout[det] = list() 
          
    for item in branch:
      if item.split('_')[1] == 'chaleur':
        xaxis = 'TMath::Abs(' + item + ')'
        #now search the other branches for the center/garde branches
        yaxis = ''
        for i in branch:
          if i.split('_')[1] == 'centre' or i.split('_')[1] == 'garde':
            yaxis += 'TMath::Abs(' + i + '/3.) + '
          
        yaxis = yaxis.rstrip(' + ')  #strip off the last '+'
          
        graphname = det + '_sumIon_v_' + item
        graphtitle = os.path.basename(filename).split('_')[0] + ' ' + graphname
        #print yaxis + ':' + xaxis
        t.Draw(yaxis + ':' + xaxis)
        gr = gPad.GetPrimitive('Graph')
        fout.cd()
        gr.GetXaxis().SetTitle(xaxis)
        gr.GetYaxis().SetTitle('#Sigma Abs(Ion)')
        gr.SetName(graphname)
        gr.SetTitle(graphname)
        gr.SetMarkerStyle(1)
        gr.Write()
        graphsout[det].append(graphname)
  
  print 'Created NTP Amplitude Correlations Graphs ->', foutname
  fout.Close()
  
  return (graphsout, foutname)
  
if __name__ == '__main__':
  main(*sys.argv[1:])
  
  
  
