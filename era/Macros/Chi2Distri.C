
#include "ChannelType.C"
#include "ChannelName.C"

int Chi2Distri(TChain* chain, string channel, string ntpfile, string cutntpfile="", string ntp2="") {

  // First get correct channel on the ntp!
  Float_t chi2, energy;
  string channeltype=ChannelType(channel);
  if (channeltype == "") { cout <<"wrong channel" << endl; return 0; }
  chain->SetBranchStatus("*",0);
  string thebranch="WienerChi2_"+channel;
  if (channeltype == "heat") {
    string bolo=BoloName(channel);
    thebranch.append("_Sync_"+ChannelName(bolo,"col1"));
  }
  chain->SetBranchAddress(thebranch.c_str(),&chi2);
  chain->SetBranchStatus(thebranch.c_str(),1);

  float param0=gChi2Cut0_Heat, param1=gChi2Cut1_Heat;
  if (channeltype=="col1") {
    param0=gChi2Cut0_Col1; param1=gChi2Cut1_Col1;
  } else if (channeltype=="col2") {
    param0=gChi2Cut0_Col2; param1=gChi2Cut1_Col2;
  } else if (channeltype=="vet1") {
    param0=gChi2Cut0_Vet1; param1=gChi2Cut1_Vet1;
  } else if (channeltype=="vet2") {
    param0=gChi2Cut0_Vet2; param1=gChi2Cut1_Vet2;
  } else if (channeltype=="gar1") {
    param0=gChi2Cut0_Gar1; param1=gChi2Cut1_Gar1;
  } else if (channeltype=="gar2") {
    param0=gChi2Cut0_Gar2; param1=gChi2Cut1_Gar2;
  }

  TFile f1(ntpfile.c_str(),"READ");
  TTree* etree = (TTree*)f1.Get("energytree");
  etree->SetBranchAddress(("E"+channeltype).c_str(),&energy);
  
  Bool_t cutrun=1;
  Bool_t cutbases=1;
  TTree* cuttree;
  if (cutntpfile != "") {
    TFile f2(cutntpfile.c_str(),"READ");
    cuttree = (TTree*)f2.Get("cuttree");
    cuttree->SetBranchAddress("CutRun",&cutrun);
    cuttree->SetBranchAddress("CutBases",&cutbases);
  }

  Int_t nb = chain->GetEntries();
  if (etree->GetEntries() != nb) cout << "error tree size" << endl;

  Float_t energy_other1=0, energy_other2=0;
  Float_t base_other1=0, base_other2=0;
  Float_t cut_otherbase=5;
  TTree* e2tree;
  if (ntp2 != "" ) {
    TFile f3(ntp2.c_str(),"READ");
    e2tree = (TTree*)f3.Get("energytree");
    if (channeltype == "heat") {
      e2tree->SetBranchAddress("Ecol1",&energy_other1);
      e2tree->SetBranchAddress("Ecol2",&energy_other2);
      e2tree->SetBranchAddress("LdbCol1",&base_other1);
      e2tree->SetBranchAddress("LdbCol2",&base_other2);
    } else {
      e2tree->SetBranchAddress("Eheat",&energy_other1);
      e2tree->SetBranchAddress("LdbHeat",&base_other1);
      if (channeltype == "col1") {
	etree->SetBranchAddress("Ecol2",&energy_other2);
	etree->SetBranchAddress("LdbCol2",&base_other2);
      } else {
	etree->SetBranchAddress("Ecol1",&energy_other2);
	etree->SetBranchAddress("LdbCol1",&base_other2);
      }
    }
    if (e2tree->GetEntries() != nb) cout << "error tree size" << endl;
  }


  Float_t* Ene = new Float_t[nb];
  Float_t* Chi = new Float_t[nb];
  Float_t* ChiCut = new Float_t[nb];
  for (Int_t i=0; i<nb; i++) {
    chain->GetEntry(i); etree->GetEntry(i);
    if (cutntpfile != "") cuttree->GetEntry(i);
    if (energy > 1.e5 || cutrun==0 || cutbases == 0 ||
	chi2 < -100 || chi2 > 1.e10 || energy < -1.e4 ||
	TMath::IsNaN(chi2) || TMath::IsNaN(energy) ) { Ene[i]=0; Chi[i]=0; ChiCut[i]=0;}
    else {
      Ene[i] = energy;
      Chi[i] = chi2;
      ChiCut[i]=chi2;
      if (ntp2 != "") {
	e2tree->GetEntry(i);
	if (energy_other1 < cut_otherbase*base_other1 || energy_other2 < cut_otherbase*base_other2) ChiCut[i]=0;
      }
    }
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }
  
  chain->SetBranchStatus("*",1);

  TCanvas* c = new TCanvas("chi2","chi2");
  TGraph* gr = new TGraph(nb,Ene,Chi);
  gr->SetTitle(("Chi2 vs E - "+channel).c_str());
  gr->GetXaxis()->SetRangeUser(-50,600); // justqu'a 100 keV
  gr->GetXaxis()->SetTitle(("Energy - "+channeltype).c_str());
  gr->GetYaxis()->SetRangeUser(-1,10);
  gr->GetYaxis()->SetTitle("Chi2");
  gr->SetMarkerStyle(6);

  TF1* fitfunc = new TF1("fitfunc","[0]+[1]*x*x",0,10000);
  fitfunc->FixParameter(0,param0);
  fitfunc->FixParameter(1,param1);
  fitfunc->SetLineColor(2);
  gr->Draw("AP");
  fitfunc->Draw("same");
  gPad->Update();
  Bool_t ok=0; 
  cout << "Ok? (0/1)" << endl; cin >> ok;
  Float_t value=0; Float_t cst=0; Bool_t zoom=0;
  while (ok == 0) {
    cout << "c0="<<fitfunc->GetParameter(0)<<" c1="<<fitfunc->GetParameter(1)<<endl;
    cout << "Enter value for parabol coef[0]"<<endl; cin >> cst;
    cout << "Enter value for parabol coef[1]"<<endl; cin >> value;
    //    cout << "Zoom?(0/1)"<<endl; cin >> zoom;
    if (zoom) {
      gr->GetYaxis()->SetRangeUser(0,2);
      gr->GetXaxis()->SetRangeUser(0,10);
    } else {
      gr->GetYaxis()->SetRangeUser(0,10);
      gr->GetXaxis()->SetRangeUser(0,400);   
    }
    fitfunc->SetParameter(0,cst); fitfunc->SetParameter(1,value);
    fitfunc->SetLineStyle(2);
    fitfunc->Draw("same");
    gPad->Update();
    cout << "Ok? (0/1)" << endl; cin >> ok;
  }
  
  if (ntp2 != "") {
    TGraph* gr2 = new TGraph(nb,Ene,ChiCut);
    gr2->SetMarkerStyle(8); gr2->SetMarkerColor(4);
    gr2->Draw("sameP");
    gPad->Update();
    int count_tot=0, count_cut=0;
    int count2_tot=0, count2_cut=0;
    for (int i=0; i<nb; i++) {
      if (Ene[i] > 8 && Ene[i] < 12 && ChiCut[i] > 0.1){
	count_tot++;
	if (ChiCut[i] <= fitfunc->Eval(Ene[i])) count_cut++;
      }
      if (Ene[i] > 8 && Ene[i] < 200 && ChiCut[i] > 0.1){
	count2_tot++;
	if (ChiCut[i] <= fitfunc->Eval(Ene[i])) count2_cut++;
      }
    }
    float ratio = 100; 
    if (count_tot != 0) ratio=100.*(float)(count_cut)/(float)(count_tot);
    cout << "** Efficiency for 8<E<12 keV : "<<count_cut<<"/"<<count_tot<<"="<<ratio<<"%"<<endl;
    ratio = 100; 
    if (count2_tot != 0) ratio=100.*(float)(count2_cut)/(float)(count2_tot);
    cout << "** Efficiency for 8<E<200 keV : "<<count2_cut<<"/"<<count2_tot<<"="<<ratio<<"%"<<endl;
  }

  delete[] Ene; delete[] Chi; delete[] ChiCut;
  return 0;
}
