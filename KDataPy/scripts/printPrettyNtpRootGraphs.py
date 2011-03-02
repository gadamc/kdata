#!/usr/bin/env python

from ROOT import TFile, TGraph, gROOT, TCanvas, gPad
import os, re, sys, string

def main(*args):
  outdir = args[1]
  if os.path.isdir(outdir)==False:
    print outdir, 'is not a directory. exiting print pretty ntp root graphs.'
    return None
    
  gROOT.SetBatch(True)
  
  f = TFile.Open(args[0])
  #f.ls()
  graphdict = args[2]
  #print graphdict
  c = TCanvas('c')
  c.Divide(2,2)

  
  for det, graphs in graphdict.items():
    padnumber = 1
    pagenumber = 1
    for graph in graphs:
    
      if (graph.split('_')[1] == 'chaleur' or graph.split('_')[2] == 'chaleur') == False or \
          (graph.split('_')[1] == 'chaleur' and graph.split('_')[3] == 'chaleur'):
        g = f.Get(graph)
            
        if padnumber > 4:
          c.SaveAs(os.path.join(outdir, det + '_' + str(pagenumber) + '.pdf'))
          pagenumber += 1
          padnumber = 1
          c.Clear('D')
      
        c.cd(padnumber)
        gPad.SetGrid(1,1)
      
        g.SetMarkerStyle(1)
        g.Draw('AP')
        padnumber += 1
        
        if re.search('sumIon_v_Ampl_chaleur',graph) != None:
          if padnumber > 4:
            c.SaveAs(os.path.join(outdir, det + '_' + str(pagenumber) + '.pdf'))
            pagenumber += 1
            padnumber = 1
            c.Clear('D')
          
          g = f.Get(graph)
          g.GetXaxis().SetRangeUser(0,3000)
          g.GetYaxis().SetRangeUser(0,3000)
        
          c.cd(padnumber)
          gPad.SetGrid(1,1)
          g.SetMarkerStyle(1)
          g.Draw('AP')
          padnumber += 1
        

    #don't forget to save the last page
    if padnumber > 1:
      c.SaveAs(os.path.join(outdir, det + '_' + str(pagenumber) + '.pdf'))
      
  
  
  
if __name__ == '__main__':
  main(*sys.argv[1:])