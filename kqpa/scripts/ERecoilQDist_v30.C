//_____________________________________________
//
// ERecoilQDist_v30.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This script is used to create histograms to show the accordance between monte
// carlo generated (E_recoil,Q)-events from given heat and ion energy means and 
// uncertainties by
// E_recoil = (1 + VoltageBias/epsilon) * E_heat - VoltageBias/epsilon * E_ion
// Q = E_ion/E_recoil
// and the distribution described by an analytical function
// (ERecoilQDistribution.pdf)
//
// void ERecoilQDist_v30( ...)
//   generates the analytical function "fkt"
//   generates TH2D* histograms
//      "mchist" for monte carlo generated events
//      "pdfhist" for the the analytical function
//    the normalisation factor c  for pdfhist is found by fitting mchist with
//    the function
//   everything is generated for the specified parameters
//   the ranges of the histograms  are determined by error propagation
//   it is set to E_recoil_modal +/- aNumSigmas * sigma_E_recoil, 
//   Q_modal +/- aNumSigmas * sigma_Q
//   where E_recoil_modal and Q_modal are the modal values
//   i. e. the most likely values calculated from the heat and ion energy means
//  additionally the histogram mchist is saved in a ROOT file of format
// %E_Ion_mean_%E_Heat_mean_%_E_Ion_sigma_%E_Heat_sigma.root
//
//  void ShowBinGausDistribution(...)
//    generates  a histogram "histres" = mchist - pdfhist
//    showing the differences of the bin contents of mchist and pdfhist
//    the quantity sum_i   (n_(mc,i) - n_(pdf,i) ) / sqrt( n_(mc,i) ) 
//    where the sum goes over all bins with entries between
//    aMinNumEntries and aMaxNumEntries, is  filled in a histogram "gaushist"
//
// MakeGraphs(...)
//   loads all mchists from root files in the current working directory and
//   and builds graphs for the distributions, their projections on the ERecoil-
//   and Q axis, the residual histograms "histres" stored in image files of the 
//   specified format


#include "TObject.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TF2.h"
#include "TStopwatch.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TString.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TFileCollection.h"
#include "TFileInfo.h"
#include "THashList.h"
#include <iostream>
#include <fstream>
using namespace std;

TF2* fkt = 0;
TF2* QMeanFkt = 0;
TF2* ERecoilMeanFkt = 0;
TH2D* mchist = 0;
TH2D* pdfhist = 0; 
TH2D* totalmchist = 0;
TH2D* totalpdfhist = 0;
TH2D* histres = 0;
TH1D* gaushist = 0;
TH1D* chi2hist = 0;
TFile* file = 0;
TTree* tree = 0;
Double_t chi2 = 0;
Double_t ndf = 0;
Double_t coverage = 0;

TDirectory* Rint = gDirectory;

Double_t  ERecoilQPropDensity(Double_t *x, Double_t *par)
{
  Double_t c = 1 + par[5]/par[6];
  Double_t c2 = 1+ x[1]*par[5]/par[6];
  Double_t a = par[0]/2/TMath::Pi()/par[3]/par[4]/c*TMath::Abs(x[0]);
  Double_t n = x[0]*x[1]-par[1];
  Double_t m = c2/c*x[0]-par[2];
  
  Double_t result = a*TMath::Exp(-0.5*(n*n/par[3]/par[3]+m*m/par[4]/par[4]));
  return result;
}

void ERecoilQDist_v30(Double_t anEIonMean = 100,
           Double_t anEHeatMean = 100,
           Double_t anEIonSigma = 1,
           Double_t anEHeatSigma = 1,
           Int_t aNumBinsX = 2000,
           Int_t aNumBinsY = 2000,
           Long_t aNumEntries = 1E9,
           Double_t aV = 3,
           Double_t anEpsilon = 1,
           Option_t* aFitOption = "0LI",
           Int_t aNumSigmas = 6
        )
{
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(2211);
  
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetFrameFillColor(kWhite);

  gStyle->SetBarOffset(1);
  
  if(gRandom) {
    delete gRandom;
    gRandom = 0;
  }
    gRandom = new TRandom3(0);

  TStopwatch t;
  
  if(!gROOT->FindObject("f"))
  {
    fkt = new TF2("f",ERecoilQPropDensity,0,1000,0,2,7);
    fkt->SetParName(0,"c");
    fkt->SetParName(1,"#bar{E_{ion}}");
    fkt->SetParName(2,"#bar{E_{heat}}");
    fkt->SetParName(3,"#sigma_{E_{ion}}");
    fkt->SetParName(4,"#sigma_{E_{heat}}");
    fkt->SetParName(5,"V");
    fkt->SetParName(6,"#epsilon_{#gamma}");
  }
    else
      fkt = (TF2*)gROOT->FindObject("f");
  fkt->SetParameter(0,1.);
  fkt->FixParameter(1,anEIonMean);
  fkt->FixParameter(2,anEHeatMean);
  fkt->FixParameter(3,anEIonSigma);
  fkt->FixParameter(4,anEHeatSigma);
  fkt->FixParameter(5,aV);
  fkt->FixParameter(6,anEpsilon);
  fkt->SetNpx(1000);
  fkt->SetNpy(1000);
  fkt->SetLineWidth(1);
  
    if(!gROOT->FindObject("QMeanf"))
  {
    QMeanFkt = new TF2("QMeanf",ERecoilQPropDensity,0,1000,0,2,7);
    QMeanFkt->SetParName(0,"c");
    QMeanFkt->SetParName(1,"#bar{E_{ion}}");
    QMeanFkt->SetParName(2,"#bar{E_{heat}}");
    QMeanFkt->SetParName(3,"#sigma_{E_{ion}}");
    QMeanFkt->SetParName(4,"#sigma_{E_{heat}}");
    QMeanFkt->SetParName(5,"V");
    QMeanFkt->SetParName(6,"#epsilon_{#gamma}");
  }
    else
      QMeanFkt = (TF2*)gROOT->FindObject("QMeanf");
  QMeanFkt->SetParameter(0,1.);
  QMeanFkt->FixParameter(1,anEIonMean);
  QMeanFkt->FixParameter(2,anEHeatMean);
  QMeanFkt->FixParameter(3,anEIonSigma);
  QMeanFkt->FixParameter(4,anEHeatSigma);
  QMeanFkt->FixParameter(5,aV);
  QMeanFkt->FixParameter(6,anEpsilon);
  QMeanFkt->SetNpx(1000);
  QMeanFkt->SetNpy(1000);
  QMeanFkt->SetLineWidth(1);
  
  if(!gROOT->FindObject("ERecoilMeanf"))
  {
    ERecoilMeanFkt = new TF2("ERecoilMeanf",ERecoilQPropDensity,0,1000,0,2,7);
    ERecoilMeanFkt->SetParName(0,"c");
    ERecoilMeanFkt->SetParName(1,"#bar{E_{ion}}");
    ERecoilMeanFkt->SetParName(2,"#bar{E_{heat}}");
    ERecoilMeanFkt->SetParName(3,"#sigma_{E_{ion}}");
    ERecoilMeanFkt->SetParName(4,"#sigma_{E_{heat}}");
    ERecoilMeanFkt->SetParName(5,"V");
    ERecoilMeanFkt->SetParName(6,"#epsilon_{#gamma}");
  }
    else
      ERecoilMeanFkt = (TF2*)gROOT->FindObject("ERecoilMeanf");
  ERecoilMeanFkt->SetParameter(0,1.);
  ERecoilMeanFkt->FixParameter(1,anEIonMean);
  ERecoilMeanFkt->FixParameter(2,anEHeatMean);
  ERecoilMeanFkt->FixParameter(3,anEIonSigma);
  ERecoilMeanFkt->FixParameter(4,anEHeatSigma);
  ERecoilMeanFkt->FixParameter(5,aV);
  ERecoilMeanFkt->FixParameter(6,anEpsilon);
  ERecoilMeanFkt->SetNpx(1000);
  ERecoilMeanFkt->SetNpy(1000);
  ERecoilMeanFkt->SetLineWidth(1);
  
  
  
    TString aFileName(TString::Format("%f_%f_%f_%f.root",
             anEIonMean,
             anEHeatMean,
             anEIonSigma,
             anEHeatSigma));
    
      cout << "filename: " << aFileName.Data() << endl;
  if(!gROOT->FindObject(aFileName.Data()))
  {
    file = new TFile(aFileName.Data(),
         "update");

  }
  else
    file = (TFile*)gROOT->FindObject(aFileName.Data());
  
  
  if(!gROOT->FindObject("mchist"))
  {
    if(!file->Get("mchist")) {
      mchist = new TH2D("mchist",
        "mchist",
        aNumBinsX,
        0,
        0,
        aNumBinsY,
        0,
        0);
      mchist->GetXaxis()->SetTitle("E_{Recoil} [keV]");
      mchist->GetYaxis()->SetTitle("Q");
    }
    else
      mchist = (TH2D*)file->Get("mchist");
  }
  else
    mchist = (TH2D*)gROOT->FindObject("mchist");
  

  Double_t anERecoilMean, anERecoilSigma;
  Double_t aQMean, aQSigma;
  Double_t Q,ERecoil,EIon,EHeat;
  
  anERecoilMean = (1+aV/anEpsilon)*anEHeatMean-aV/anEpsilon*anEIonMean;
  aQMean = anEIonMean/anERecoilMean;
  anERecoilSigma = TMath::Sqrt((1+aV/anEpsilon)*(1+aV/anEpsilon)*
            anEHeatSigma*anEHeatSigma
            +aV*aV/anEpsilon/anEpsilon*
            anEIonSigma*anEIonSigma);
  
  aQSigma = TMath::Sqrt(
    TMath::Power(anEIonMean/anERecoilMean/anERecoilMean*(1+aV/anEpsilon
    ) * anEHeatSigma,2)
    + TMath::Power((1/anERecoilMean + aV/anEpsilon*
anEIonMean/anERecoilMean/anERecoilMean)*anEIonSigma,2));
 
  
    t.Start();
    mchist->SetBins(aNumBinsX,
      anERecoilMean - aNumSigmas * anERecoilSigma,
      anERecoilMean + aNumSigmas * anERecoilSigma,
      aNumBinsY,
      aQMean - aNumSigmas * aQSigma,
      aQMean + aNumSigmas * aQSigma);
      

    
    for(Long64_t k = 0; k<aNumEntries; ++k) {
      EIon = gRandom->Gaus(anEIonMean,anEIonSigma);
      EHeat = gRandom->Gaus(anEHeatMean,anEHeatSigma);
      ERecoil = (1+aV/anEpsilon)*EHeat-aV/anEpsilon*EIon;
      Q = EIon/ERecoil;
      mchist->Fill(ERecoil,Q);
    }    
    cout << "mc histo filled ..." << endl;
    t.Stop();
    cout << "cpu time: " << t.CpuTime() << endl;
    cout << "real time: " << t.RealTime() << endl;
    t.Start();
    mchist->Draw("colz");
    if(strcmp(aFitOption,""))
      mchist->Fit(fkt,aFitOption);
    fkt->Draw("same");
    

    t.Stop();
    cout << "cpu time: " << t.CpuTime() << endl;
    cout << "real time: " << t.RealTime() << endl;
    mchist->Write("",TObject::kOverwrite);
}

void SetFile(TFile* aFile)
{
  file = aFile;
}





void ShowBinGausDistribution(Double_t aMinNumEntries = 400,
                             Double_t aMaxNumEntries = 1E50
)
{
  if(!mchist) {
    cout << "void ShowBinGausDistribution(): no mc hist initialized " << endl;
    return;
  }
  pdfhist = (TH2D*)mchist->Clone();
  histres = (TH2D*)mchist->Clone();
  histres->Reset();
  pdfhist->Reset();
 
    fkt = (TF2*)mchist->GetFunction("f");
  
   Double_t content = 0;
    for(Int_t k = 0; k<pdfhist->GetNbinsX()-1; ++k)
      for(Int_t l = 0; l<pdfhist->GetNbinsY()-1; ++l) {
        
        content = fkt->Eval(pdfhist->GetXaxis()->GetXmin() + (k-0.5)*
        pdfhist->GetXaxis()->GetBinWidth(0),
                            pdfhist->GetYaxis()->GetXmin() + (l-0.5) * 
                            pdfhist->GetYaxis()->GetBinWidth(0));
                            
        /*
       content = fkt->Integral(pdfhist->GetXaxis()->GetXmin() + (k-1)*
       pdfhist->GetXaxis()->GetBinWidth(0),
                               pdfhist->GetYaxis()->GetXmin() + (l-1)*
                               pdfhist->GetYaxis()->GetBinWidth(0),
                               pdfhist->GetXaxis()->GetXmin() + k *
                               pdfhist->GetXaxis()->GetBinWidth(0),
                               pdfhist->GetYaxis()->GetXmin() + l *
                               pdfhist->GetYaxis()->GetBinWidth(0));
                               */
        pdfhist->SetBinContent(k,l,content);

      }
        cout << "pdf histo filled ..." << endl;
        
   if(!gROOT->FindObject("gaushist")) {
     gaushist = new TH1D("gaushist",
                         "gaushist",
                         1000,
                         -5,
                         5);
   }
  else
    gaushist = (TH1D*)gROOT->FindObject("gaushist");
  gaushist->Reset();
  
     if(!gROOT->FindObject("chi2hist")) {
     chi2hist = new TH1D("chi2hist",
                         "chi2hist",
                         1000,
                         -5,
                         5);
   }
  else
    chi2hist = (TH1D*)gROOT->FindObject("chi2hist");
  chi2hist->Reset();
 
  
  ndf = 0;
  chi2 = 0;
  Double_t gaus;
  Double_t dn, sdn;
  Double_t nevents = 0;
  Double_t aCorrectionFactor = mchist->Integral()/
  pdfhist->Integral();

  for(Int_t k = 1; k<mchist->GetNbinsX(); ++k)
    for(Int_t l = 1; l<mchist->GetNbinsY(); ++l)     {
      histres->SetBinContent(k,l,aCorrectionFactor*pdfhist->GetBinContent(k,l)
      -mchist->GetBinContent ( k , l));
      if(pdfhist->GetBinContent(k,l)>aMinNumEntries
        &&pdfhist->GetBinContent(k,l)<aMaxNumEntries
       //&&  mchist->GetBinContent(k,l)>1.01*pdfhist->GetBinContent(k,l)
      ) {
        cout << " k = " << k << " l = " << l << " mc events : " <<
mchist->GetBinContent(k,l) <<
        " pdf events: " << pdfhist->GetBinContent(k,l) <<endl; 
        nevents += mchist->GetBinContent(k,l);
        dn =
        mchist->GetBinContent(k,l)-pdfhist->GetBinContent(k,l)*
        aCorrectionFactor ;
        sdn = TMath::Sqrt(mchist->GetBinContent(k,l));
        gaus = dn/sdn;   
      chi2hist->Fill(dn*dn/pdfhist->GetBinContent(k,l)/aCorrectionFactor);
        gaushist->Fill(gaus);
        ++ndf;
        chi2 += gaus*gaus;
      }
    }
    --ndf; 
    
    
    coverage = nevents/mchist->GetEffectiveEntries();
    
      cout << "percentage events mc/ pdf: " << aCorrectionFactor << endl;
    cout << nevents << " of " << mchist->GetEffectiveEntries() << " events"
    << " = " << coverage << endl;
    cout << "chi2 / ndf : " << chi2 << "/" << ndf << " = " << chi2/ndf << endl;
    cout << "TMath::Prob(chi2,ndf): " << TMath::Prob(chi2,ndf) << endl;
    cout << "histres filled" << endl;
}

void LoadFile(const Char_t* aFileName)
{
  cout << "... loading " << aFileName << endl;
  if(file)
  if(file->IsOpen())
    file->Close();
  file = TFile::Open(aFileName);
  mchist = (TH2D*)file->Get("mchist");
  fkt = (TF2*)mchist->GetFunction("f");
  cout << " ... loading finished" << endl;
}

void MakeGraphs(const Char_t* aFileFormat = "pdf",
                Double_t aSignificanceLevel = 0.01)
{
  ofstream ostextable("table.tex");
  ofstream ostexgraphics("graphics.tex");
  TCanvas c1("c1","c1");
  TFileCollection aFileCollection("file collection","file collection");
  aFileCollection.Add("*.root");
  TString aFilePrefix;
  TFileInfo* info;
  
  vector<Double_t> chi2s;
  vector<Double_t> ndfs;
  vector<Double_t> chiprobs;
  vector<Double_t> coverages;
  vector<Double_t> EIonMeans;
  vector<Double_t> EHeatMeans;
  vector<Double_t> EIonSigmas;
  vector<Double_t> EHeatSigmas;
  vector<string> H0s;
  
  for(Int_t k = 0; k< aFileCollection.GetNFiles(); ++k) {
    info = (TFileInfo*)aFileCollection.GetList()->At(k);
    LoadFile(info->GetFirstUrl()->GetFile());
    ShowBinGausDistribution();
    aFilePrefix = info->GetFirstUrl()->GetFile();
    for(Int_t l = 0; l<5; ++l)
      aFilePrefix.Chop();
    mchist->Draw("colz");
    c1.SaveAs(TString::Format("%s_mchist.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_mchist.%s}",
                                           aFilePrefix.Data(),
                                           aFileFormat).Data() << endl;
    cout << TString::Format("... saved %s_mchist.%s",
                            aFilePrefix.Data(),
                            aFileFormat).Data() << endl;
                            
    mchist->ProjectionY()->Draw();
    c1.SaveAs(TString::Format("%s_py.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_py.%s}",
                             aFilePrefix.Data(),
                             aFileFormat).Data() << endl;
    cout << TString::Format("... saved %s_py.%s",
                            aFilePrefix.Data(),
                            aFileFormat).Data() << endl;
                            
    mchist->ProjectionX()->Draw();
    c1.SaveAs(TString::Format("%s_px.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_px.%s}",
                             aFilePrefix.Data(),
                             aFileFormat).Data() << endl;
    cout << TString::Format("... saved %s_px.%s",
                            aFilePrefix.Data(),
                            aFileFormat).Data() << endl;
                            
    histres->ProjectionY()->Draw();

    c1.SaveAs(TString::Format("%s_pyres.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_pyres.%s}",
                             aFilePrefix.Data(),
                             aFileFormat).Data() << endl;
    cout << TString::Format("... saved %s_pyres.%s",
                            aFilePrefix.Data(),
                            aFileFormat).Data() << endl;
                            
    histres->ProjectionX()->Draw();
    c1.SaveAs(TString::Format("%s_pxres.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_pxres.%s}",
                             aFilePrefix.Data(),
                             aFileFormat).Data() << endl;
    cout << TString::Format("... saved %s_pxres.%s",
                            aFilePrefix.Data(),
                            aFileFormat).Data() << endl;
                            
    gaushist->Draw();
    gaushist->Fit("gaus");
    c1.SaveAs(TString::Format("%s_gaus.%s",
                              aFilePrefix.Data(),
                              aFileFormat).Data());
    ostexgraphics << TString::Format("\\includegraphics{%s_gaus.%s}",
                             aFilePrefix.Data(),
                             aFileFormat).Data() << endl;
                             
     chi2s.push_back(chi2);
     ndfs.push_back(ndf);
     coverages.push_back(coverage);
     chiprobs.push_back(TMath::Prob(chi2,ndf));
     EIonMeans.push_back(fkt->GetParameter(1));
     EHeatMeans.push_back(fkt->GetParameter(2));
     EIonSigmas.push_back(fkt->GetParameter(3));
     EHeatSigmas.push_back(fkt->GetParameter(4));
     
     if(TMath::Prob(chi2,ndf)>aSignificanceLevel)
       H0s.push_back("yes");
     else
       H0s.push_back("no");
     

  }
 
  ostextable << "\\begin{minipage}{\\textwidth}" << endl;
  ostextable << "\\captionof{table}{$\\chi^2$ values for the corresponding"
   " parameter "
  " combinations and acceptance of the null hypothesis $H_0$}" << endl;
  ostextable << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|} \\hline" << endl;
  ostextable << "$\\overline{E_{Ion}}$ & $\\overline{E_{Heat}}$ & "
   "$\\sigma_{E_{Ion}}$"
  " & $\\sigma_{E_{Heat}}$ & $\\chi^2$ & ndf & $n_{min}$ & "
  " TMath::Prob($\\chi^2$,ndf) & CL of pdf & $H_0$ \\\\ \\hline \\hline" << endl;
  for(Int_t k = 0; k< aFileCollection.GetNFiles(); ++k)
    ostextable << EIonMeans[k] << " & " << EHeatMeans[k] << " & " << 
    EIonSigmas[k] << " & " << EHeatSigmas[k] << " & " << chi2s[k] << " & "
    << ndfs[k] << " & " << coverages[k] << " & " << H0s[k].c_str()
    << " \\\\ \\hline " << endl;
  ostextable << "\\end{tabular} \\\\[1cm] " << endl;
    
  
}


  
           
      