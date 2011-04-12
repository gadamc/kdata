
#include "ChannelName.C"
#include <vector>
#include "TMath.h"

vector<ULong_t> HeatIonTimePeriods(Int_t timebin, string periodfile) {
  
  vector<ULong_t> ti,tf; ULong_t theti,thetf;
  ifstream buf(periodfile.c_str(),ios::in);
  while (buf >> theti >> thetf) {
    ti.push_back(theti); tf.push_back(thetf);
    if ((thetf-theti) > timebin) 
      cout << "Warning long time period wrt heation timebin" <<endl;
  }
  buf.close();
  
  vector<ULong_t> times;
  times.push_back(ti[0]); 
  ULong_t t_current=times[0];
  for (Int_t p=0; p<tf.size(); p++) {
    if (ti[p] >= t_current+timebin) {
      times.push_back((ti[p])); t_current=ti[p];
    }
    if (tf[p] >= t_current+timebin || p == (tf.size()-1)) {
      times.push_back((tf[p])); t_current=tf[p];
    }
  }
  
  return times;
}


int HeatIonEvol(TChain* chain, TTree* Eion, string bolo, Float_t EcutIon, Int_t timebin, string periodfile, string heatgainfile="",bool use_median=1, Float_t EmaxIon=500) {
  
  Float_t chal=0, efid=0;
  ULong64_t date=0;
  Bool_t vetocut=1;
  
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("DateSec",&date);
  string chaleur=ChannelName(bolo,"heat");
  string col1=ChannelName(bolo,"col1");
  chain->SetBranchAddress(("WienerAmpl_"+chaleur+"_Sync_"+col1).c_str(),&chal);
  chain->SetBranchStatus("DateSec",1);
  chain->SetBranchStatus(("WienerAmpl_"+chaleur+"_Sync_"+col1).c_str(),1);
  
  if (!Eion->GetBranch("Efid")) cerr <<"incomplete ion tree" <<endl;
  Eion->SetBranchAddress("Efid",&efid);
  Eion->SetBranchAddress("VetoCut",&vetocut);
  
  Int_t nb = chain->GetEntries();
  if (nb != Eion->GetEntries()) cout << "pbl taille trees" <<endl;
  
  vector<ULong_t> timeperiods=HeatIonTimePeriods(timebin,periodfile);
  
  Int_t nbins=timeperiods.size()-1;
  Double_t* xbins = new Double_t[nbins+1];
  for (int i=0; i<=nbins; i++) xbins[i]=(Double_t)(timeperiods[i]);
  TProfile* hprof=  new TProfile("hprof","Heat/Ion",nbins,xbins); // ylow/up
  
  vector<Float_t> TheTime;
  vector<Float_t> TheRatio;
  int nbnan=0;
  
  for (Int_t i=0; i<nb; i++) {
    chain->GetEntry(i); Eion->GetEntry(i);
    
    Float_t Eion_fid=0, VetoIon=0;
    //    if (vetocut == 1 && efid > EcutIon && chal > 0) {
    if (vetocut == 1 && efid > EcutIon && chal > 0 && efid < EmaxIon && TMath::Finite(chal) != 0) {
      //    if (chal > 0 && efid < EmaxIon && efid > 50) {
      TheTime.push_back((Float_t)date);
      TheRatio.push_back(chal/efid);
      //      hprof->Fill(Time[i],Ratio[i]);
      hprof->Fill((Float_t)date,chal/efid);
    } else {
      //      Ratio[i]=0;
      //      Time[i]=timeperiods[0];
    }
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }
  chain->SetBranchStatus("*",1);
  
  int nbpts=TheTime.size();
  Float_t* Time = new Float_t[nbpts];
  Float_t* Ratio = new Float_t[nbpts];
  for (int i=0; i<nbpts; i++) {
    Time[i]=TheTime[i]; Ratio[i]=TheRatio[i];
  }
  
  TH1F* hmed = new TH1F("hmed","Heat/Ion",nbins,xbins);
  if (use_median) {
    Float_t* medians = new Double_t[nbins];
    for (Int_t p=0; p<nbins; p++) {
      vector<Float_t> data; 
      Float_t ti=xbins[p]; Float_t tf=xbins[p+1];
      for (int i=0; i<nbpts; i++) {
        if (Ratio[i] != 0 && Time[i] >= ti && Time[i] <= tf) data.push_back(Ratio[i]);
      }
      if (data.size() !=0) medians[p] = TMath::Median(data.size(),&data[0]);
      else medians[p]=0;
      //      cout << data.size()<<" "<<ti<<" "<<tf<<" "<<medians[p] << endl;
      hmed->SetBinContent(p+1,medians[p]);
    }
  }
  
  TCanvas* c = new TCanvas("heation","heation");
  TGraph* gr = new TGraph(nbpts,Time,Ratio);
  gr->SetTitle((bolo+" - Heat/Ion evolution").c_str());
  gr->GetXaxis()->SetTimeDisplay(1);
  gr->SetMarkerStyle(7);
  //  gr->GetYaxis()->SetRangeUser(-0.1,3);
  gr->Draw("AP");
  if (!use_median) {
    hprof->SetLineColor(2);
    hprof->SetLineWidth(2);
    hprof->Draw("same");
  } else {
    hmed->SetLineColor(2);
    hmed->SetLineWidth(2);
    hmed->Draw("same");
  }
  
  if (heatgainfile != "") {
    ofstream ff(heatgainfile.c_str(),ios::out);
    for (int i=1; i<=nbins; i++) {
      if (!use_median) {
        if (hprof->GetBinEntries(i) != 0) {
          ff << (ULong64_t)(hprof->GetBinLowEdge(i)) <<" "
          <<(ULong64_t)(hprof->GetBinLowEdge(i))+(ULong64_t)(hprof->GetBinWidth(i))
          <<" "<<hprof->GetBinContent(i)<<endl;
        }
      } else {
        if (hmed->GetBinContent(i) != 0) {
          ff << (ULong64_t)(hmed->GetBinLowEdge(i)) <<" "
          <<(ULong64_t)(hmed->GetBinLowEdge(i))+(ULong64_t)(hmed->GetBinWidth(i))
          <<" "<<hmed->GetBinContent(i)<<endl;
        }
      }
    }
    ff.close();
  }
  
  return 0;
}
