
#include <vector>
#include <string>

int BuildHeatEnergy(TChain* chain, string bolo, string heatgainfile, string echalntpfile, TTree* Eion) {
  
  bool heatonly_flag=0;
  if (bolo == "Gc2") heatonly_flag=1;
  
  // 1) Prepare the trees/chain
  TFile* f = new TFile(echalntpfile.c_str(),"RECREATE");
  TTree* t = new TTree("energytree","EdwHeatEnergy");
  Float_t Eheat=0, E0heat=0, Aheat=0, A0heat=0, Aheat_nosync=0;
  Float_t Chi2Heat=0, RiseTime=0, PulseWidth=0;
  Float_t lChi2=0, lRise=0, lWidth=0;
  ULong64_t date_out=0;
  
  t->Branch("Eheat",&Eheat,"Eheat/F");
  t->Branch("E0heat",&E0heat,"E0heat/F");
  t->Branch("Chi2Heat",&lChi2,"Chi2Heat/F");
  t->Branch("RiseTime",&lRise,"RiseTime/F");
  t->Branch("PulseWidth",&lWidth,"PulseWidth/F");
  if (heatonly_flag) t->Branch("DateSec",&date_out,"DateSec/l");
  
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("DateSec",1);
  string heatname=ChannelName(bolo,"heat");
  string col1=ChannelName(bolo,"col1");
  chain->SetBranchStatus(("WienerAmpl_"+heatname).c_str(),1);
  if (!heatonly_flag) chain->SetBranchStatus(("WienerAmpl_"+heatname+"_Sync_"+col1).c_str(),1);
  chain->SetBranchStatus(("WienerZeroAmpl_"+heatname).c_str(),1);
  chain->SetBranchStatus(("WienerChi2_"+heatname).c_str(),1);
  chain->SetBranchStatus(("RiseTime_"+heatname).c_str(),1);
  chain->SetBranchStatus(("FWHM_"+heatname).c_str(),1);
  if (!heatonly_flag) {
    chain->SetBranchAddress(("WienerAmpl_"+heatname).c_str(),&Aheat_nosync);
    chain->SetBranchAddress(("WienerAmpl_"+heatname+"_Sync_"+col1).c_str(),&Aheat);
  } else chain->SetBranchAddress(("WienerAmpl_"+heatname).c_str(),&Aheat);
  chain->SetBranchAddress(("WienerZeroAmpl_"+heatname).c_str(),&A0heat);
  chain->SetBranchAddress(("WienerChi2_"+heatname).c_str(),&Chi2Heat);
  chain->SetBranchAddress(("RiseTime_"+heatname).c_str(),&RiseTime);
  chain->SetBranchAddress(("FWHM_"+heatname).c_str(),&PulseWidth);
  
  ULong64_t date=0;
  chain->SetBranchAddress("DateSec",&date);
  
  Int_t vflag=0;
  if (!heatonly_flag) {
    if (Eion->GetEntries() != chain->GetEntries()) cout << "Error ntp sizes"<<endl;
    Eion->SetBranchStatus("*",0);
    Eion->SetBranchStatus("PolarFlag",1);
    Eion->SetBranchAddress("PolarFlag",&vflag);
  }
  
  // 2) Parameters : special crosstalk + calib params
  Float_t a_talk=0;
  if (gSpecialCTalked == heatname && gSpecialCTalk != "") {
    chain->SetBranchAddress(("WienerAmpl_"+gSpecialCTalk).c_str(),&a_talk);
    chain->SetBranchStatus(("WienerAmpl_"+gSpecialCTalk).c_str(),1);
  }
  
  TF1* calibfunc = new TF1("cf",HeatCalibFunc,-100,100,4);
  vector<Float_t> seuil_calcul_log;
  for (int p=0; p<gNbVoltages; p++) {
    if (gCalib_Heat1[p]!=0) {
      Float_t toto=((gCalib_Heat2[p]/gCalib_Heat0[p])-1)/gCalib_Heat1[p];
      toto-=(gCalib_Heat3[p]);
      seuil_calcul_log.push_back(pow(10.,toto));
      cout << "NL threshold: "<<seuil_calcul_log[p] << endl;  
    } else {
      cout << "No NL in calibration" << endl;
      seuil_calcul_log.push_back(10000.);
    }
  }
  
  // 3) Load gain vs time
  ULong_t ti_buf,tf_buf; Float_t g_buf;
  vector<ULong_t> ti,tf;
  vector<Float_t> gain;
  if (!heatonly_flag) {
    ifstream gainstream(heatgainfile.c_str(),ios::in);
    if (!gainstream) cout << "No heat gain file!" <<endl;
    while (gainstream >> ti_buf >> tf_buf >> g_buf) {
      ti.push_back(ti_buf);
      tf.push_back(tf_buf);
      gain.push_back(g_buf);
    }
    gainstream.close();
  }
  int nbgains=gain.size();
  Int_t j_gain=0;
  if (!heatonly_flag) g_buf=gain[0];
  else g_buf=1;
  
  // 4) Loop
  Float_t ampl, ampl0, loga, loga0;
  Int_t vflagcor=0; // si vflag = -1 on met arbitrairement a zero.
  int nbnan=0, nbnan_sync=0;
  for (Int_t i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    if (!heatonly_flag) Eion->GetEntry(i);
    lChi2=Chi2Heat; lRise=RiseTime; lWidth=PulseWidth;
    date_out=date;
    if (vflag >= gNbVoltages) cout << "error vflag " <<vflag<<" "<<gNbVoltages<< endl;
    if (vflag < 0 ) vflagcor=0; // et pas -1!!
    else vflagcor=vflag;
    calibfunc->SetParameter(0,gCalib_Heat0[vflagcor]);
    calibfunc->SetParameter(1,gCalib_Heat1[vflagcor]);
    calibfunc->SetParameter(2,gCalib_Heat2[vflagcor]);
    calibfunc->SetParameter(3,gCalib_Heat3[vflagcor]);
    if (!heatonly_flag) {
      while (date > tf[j_gain]) {
        if (j_gain < nbgains-1) {
          j_gain++;
          g_buf=gain[j_gain];
        } else break;
      }
    }
    
    ampl=Aheat;  //assign he weiner amplitude here.
    if (TMath::Finite(ampl) == 0) {
      ampl=Aheat_nosync;
      nbnan_sync++;
    }
    ampl0=A0heat;
    if (ampl != 0 && a_talk != 0 && TMath::Finite(a_talk) != 0) {
      ampl -= (a_talk*gSpecialCT[vflagcor]);
      ampl0 -= (a_talk*gSpecialCT[vflagcor]);
    }
    if (TMath::Finite(ampl) == 0 || TMath::Finite(ampl0) == 0) {
      ampl=0; ampl0=0; nbnan++;
    }
    ampl/=g_buf;  //apply a time-dependent paramter to the heat signal.
    
    ampl0/=g_buf;
    if (ampl > seuil_calcul_log[vflagcor]) loga = TMath::Log10(ampl);
    else loga=-10; // regime NL
    if (ampl0 > seuil_calcul_log[vflagcor]) loga0 = TMath::Log10(ampl0);
    else loga0=-10;
    //    Eheat = calibfunc->GetX(Aheat,-10000,10000);
    Eheat = ampl*calibfunc->Eval(loga);
    E0heat = ampl0*calibfunc->Eval(loga0);
    
    t->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  f->cd(); t->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);
  if (!heatonly_flag) Eion->SetBranchStatus("*",1);
  cout << "** Nb of infinite amplitudes: "<<nbnan << endl;
  cout << "** (with sync : "<<nbnan_sync<<" )"<<endl;
  return 0;
}
