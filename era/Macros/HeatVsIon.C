

#include "HeatCalibFunc.C"

int HeatVsIon(TTree* t, string bolo, TTree* Eion, string heatgainfile, TTree* t2=NULL, TTree* Eion2=NULL, string heatgain2="", Int_t usevoltflag=-1) {

  // t2 et Eion2: pour les runs bckgd pour avoir le 10keV dans la calib

  Int_t nb = t->GetEntries();
  if (Eion->GetEntries() != nb) cerr << "trees not matching!" << endl;
  if (!Eion->GetBranch("Efid")) cerr <<"incomplete ion tree" <<endl;

  Int_t nbtot=nb;
  if (t2 != NULL) {
    Int_t nb2 = t2->GetEntries();
    if (Eion2->GetEntries() != nb2) cerr <<"trees2 not matching" <<endl;
    if (!Eion2->GetBranch("Efid")) cerr <<"incomplete ion tree2" <<endl;
    nbtot+=nb2;
  }
  
  Bool_t vetocut=0; Float_t efid=0;
  Float_t chal=0;
  Float_t* Energy = new Float_t[nbtot];
  Float_t* Achal = new Float_t[nbtot];
  Bool_t* VetoIon = new Bool_t[nbtot];

  string voiechal=ChannelName(bolo,"heat");
  string voiecol1=ChannelName(bolo,"col1");
  t->SetBranchStatus("*",0);
  t->SetBranchAddress(("WienerAmpl_"+voiechal+"_Sync_"+voiecol1).c_str(),&chal);
  t->SetBranchStatus(("WienerAmpl_"+voiechal+"_Sync_"+voiecol1).c_str(),1);
  t->SetBranchStatus("DateSec",1);
  ULong64_t date=0;
  t->SetBranchAddress("DateSec",&date);

  Eion->SetBranchAddress("VetoCut",&vetocut);
  Eion->SetBranchAddress("Efid",&efid);
  Int_t vflag=1;
  if (usevoltflag >= 0) Eion->SetBranchAddress("PolarFlag",&vflag);

  ULong_t ti_buf,tf_buf; Float_t g_buf;
  vector<ULong_t> ti,tf;
  vector<Float_t> gain;
  ifstream gainstream(heatgainfile.c_str(),ios::in);
  while (gainstream >> ti_buf >> tf_buf >> g_buf) {
    ti.push_back(ti_buf);
    tf.push_back(tf_buf);
    gain.push_back(g_buf);
  }
  gainstream.close();
  int nbgains=gain.size();
  Int_t j_gain=0;
  g_buf=gain[0];

  for (Int_t i=0; i<nb; i++) {
    t->GetEntry(i);
    Eion->GetEntry(i);
    if (date > tf[j_gain] ){ // Bricolo pour accelerer CINT...
      while (date > tf[j_gain] ){
	if (j_gain < nbgains-1) {
	  j_gain++;
	  g_buf=gain[j_gain];
	} else break;
      }
    }
    Achal[i]=chal/g_buf;
    if (vetocut == 0) {
      Energy[i]=0;
    } else if (usevoltflag >= 0 && vflag != usevoltflag) {
      Energy[i]=0;
    } else Energy[i] = efid;
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  // Case of second tree
  if (t2 != NULL) {
    t2->SetBranchStatus("*",0);
    t2->SetBranchAddress(("WienerAmpl_"+voiechal+"_Sync_"+voiecol1).c_str(),&chal);
    t2->SetBranchStatus(("WienerAmpl_"+voiechal+"_Sync_"+voiecol1).c_str(),1);
    t2->SetBranchStatus("DateSec",1);
    t2->SetBranchAddress("DateSec",&date);
    Eion2->SetBranchAddress("VetoCut",&vetocut);
    Eion2->SetBranchAddress("Efid",&efid);
    ifstream gainstream2(heatgain2.c_str(),ios::in);
    vector<ULong_t> ti2,tf2; vector<Float_t> gain2;
    while (gainstream2 >> ti_buf >> tf_buf >> g_buf) {
      ti2.push_back(ti_buf);
      tf2.push_back(tf_buf);
      gain2.push_back(g_buf);
    }
    gainstream2.close();
    nbgains=gain2.size(); j_gain=0; g_buf=gain2[0];

    for (Int_t i=0; i<nb2; i++) {
      t2->GetEntry(i);
      Eion2->GetEntry(i);
      if (date > tf2[j_gain] ){ // Bricolo pour accelerer CINT...
	while (date > tf2[j_gain] ){
	  if (j_gain < nbgains-1) {
	    j_gain++;
	    g_buf=gain2[j_gain];
	  } else break;
	}
      }
      Achal[i+nb]=chal/g_buf;
      if (vetocut == 0) Energy[i+nb]=0;
      else Energy[i+nb] = efid;
      if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
    }

    t2->SetBranchStatus("*",1);
  }

  Float_t min_ion=-5, max_ion=500;
  Float_t min_chal=-5, max_chal=600;
  TGraph* gr = new TGraph(nbtot,Energy,Achal);
  gr->SetTitle(("Chal vs ion - "+bolo).c_str());
  gr->GetXaxis()->SetRangeUser(min_ion,max_ion);
  gr->GetXaxis()->SetTitle("Ionisation");
  gr->GetYaxis()->SetRangeUser(min_chal,max_chal);
  gr->GetYaxis()->SetTitle("Heat");
  gr->SetMarkerStyle(6);
  gr->Draw("AP");
  gPad->Update();

  cout << "Go to A/E vs log(E) plot? (0/1)" << endl;
  Bool_t yes = 0; cin >> yes;
  if (yes) {
    gPad->Clear();
    Float_t* Rapport = new Float_t[nbtot]; // rapport=E/A
    Float_t* LogA = new Float_t[nbtot];
    Float_t sum = 0; Int_t nbeff=0;
    for (Int_t i=0; i<nbtot; i++) {
      if (Achal[i] > 7 && Energy[i] > 8) {
	Rapport[i] = Energy[i]/Achal[i];
	LogA[i] = TMath::Log10(Achal[i]);
	sum+=Rapport[i]; nbeff+=1;
      } else {Rapport[i]=0; LogA[i]=0; }
    }
    sum/=nbeff;
    cout << "Average ratio for Achal/Energy: " << sum << endl;

    Float_t ymin=0.7, ymax=1.5;
    TGraph* gr1 = new TGraph(nbtot,LogA,Rapport);
    gr1->SetTitle(("Heat calibration - "+bolo).c_str());
    gr1->GetXaxis()->SetRangeUser(0.5,3.5);
    gr1->GetXaxis()->SetTitle("Log10(Aheat)");
    gr1->GetYaxis()->SetRangeUser(ymin,ymax);
    gr1->GetYaxis()->SetTitle("Energy_ion/Aheat");
    gr1->SetMarkerStyle(6);
    gr1->Draw("AP");
    
    // Et en bonus, un TH2F et son profile + le fit du profile
    TF1* calibfunc = new TF1("cf",HeatCalibFunc,1.,2.8,4);
    calibfunc->SetParameter(0,1); calibfunc->SetParameter(1,-0.1);
    calibfunc->SetParameter(2,1.2); calibfunc->SetParameter(3,0.2);
    calibfunc->SetParLimits(3,0.1,0.6);
    calibfunc->SetParLimits(1,-0.9,-0.04);
    calibfunc->SetParLimits(2,1.0,1.15);

    TCanvas* jojo = new TCanvas("heat calib","heat calib");
    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);
    TH2F* h2 = new TH2F("h2",("Heat calibration - "+bolo).c_str(),60,0.8,3.5,200,ymin,ymax);
    for (Int_t p=0; p<nbtot; p++) h2->Fill(LogA[p],Rapport[p]);
    h2->Draw("COLZ");
    TProfile* hp = h2->ProfileX("heat calib prof");
    hp->SetLineColor(2);
    hp->Draw("same");
    hp->Fit(calibfunc,"MR","same");

  }

  t->SetBranchStatus("*",1);

  delete[] Energy; delete[] Achal; delete[] VetoIon;
  return 0;
}
