
// Debut Septembre 2010
// analyse rapide des ntps de fid804 en gamma

{

  string ntpdir="/sps/edelweis/EdwRootAna/FID_Run13/Test/ntps/";
  TChain* chain = new TChain("EdwRootNtp","EdwRootNtp");
  for (int p=1; p<=5; p++) { std::ostringstream truc; truc<<p; chain->AddFile((ntpdir+"ntp_"+truc.str()+".root").c_str()); }
  
  //*********************************************
  // Calib ion approximative
  float calib_ionb=356./1.02;
  float calib_iond=356./0.91;
  float calib_iona=356./1.14;
  float calib_ionc=356./1.51;
  float calib_iong=356./0.89;
  float calib_ionh=356./0.89;

// Avec ca on mesure les lignes de base (FWHM):
// b=1.35keV; d=1.59keV
// a=4keV; c=2.9keV (queue non gaussienne marquee sur ces deux voies)
// g=1.43keV; h=1.91keV

/*
  Float_t Eia,Eib,Eic,Eid,Eig,Eih=0;
  Float_t Aa,Ab,Ac,Ad,Ag,Ah=0;
  TFile* f = new TFile((ntpdir+"eion.root").c_str(),"RECREATE");
  TTree* t = new TTree("Eion","Eion");
  t->Branch("Eia",&Eia,"Eia/F");
  t->Branch("Eib",&Eib,"Eib/F");
  t->Branch("Eic",&Eic,"Eic/F");
  t->Branch("Eid",&Eid,"Eid/F");
  t->Branch("Eig",&Eig,"Eig/F");
  t->Branch("Eih",&Eih,"Eih/F");
  chain->SetBranchAddress("WienerAmpl_centre_FID804AB",&Aa);
  chain->SetBranchAddress("WienerAmpl_garde_FID804AB",&Ab);
  chain->SetBranchAddress("WienerAmpl_centre_FID804CD",&Ac);
  chain->SetBranchAddress("WienerAmpl_garde_FID804CD",&Ad);
  chain->SetBranchAddress("WienerAmpl_centre_FID804GH",&Ag);
  chain->SetBranchAddress("WienerAmpl_garde_FID804GH",&Ah);

  for (int i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    Eia=calib_iona*Aa; Eib=calib_ionb*Ab;
    Eic=calib_ionc*Ac; Eid=calib_iond*Ad;
    Eig=calib_iong*Ag; Eih=calib_ionh*Ah;
    t->Fill();
  }
  f->cd(); t->Write(0,TObject::kOverwrite);
  f->Close();
*/

//*********************************
// Chaleur
  //  TFile* f = new TFile((ntpdir+"eion.root").c_str(),"READ");
  //  TTree* t = (TTree*)f->Get("Eion");
  //  chain->AddFriend(Eion);
  //chain->Draw("(Eion.Eib+Eion.Eid)/(2.*WienerAmpl_chaleur_FID804CD):DateSec","Eion.Eia<10 && Eion.Eic<10 && Eion.Eig<10 && Eion.Eih<10 && Eion.Eid > 10 && Eion.Eib>10");
  // Pour l'instant on s'embete pas : calib lineaire et surtout constante au cours du temps.
  float calib_chal1=300./0.875;
  float calib_chal2=355.34;

  /*
  TFile* f2 = new TFile((ntpdir+"eheat.root").c_str(),"RECREATE");
  TTree* t2 = new TTree("Eheat","Eheat");
  Float_t Eh1,Eh2,Chi2heat1,Chi2heat2=0;
  t2->Branch("Eh1",&Eh1,"Eh1/F");
  t2->Branch("Eh2",&Eh2,"Eh2/F");
  t2->Branch("Chi2heat1",&Chi2heat1,"Chi2heat1/F");
  t2->Branch("Chi2heat2",&Chi2heat2,"Chi2heat2/F");
  Float_t Achal1, Achal2=0, Chi1, Chi2=0;   
  chain->SetBranchAddress("WienerAmpl_chaleur_FID804AB",&Achal1);
  chain->SetBranchAddress("WienerAmpl_chaleur_FID804CD",&Achal2);
  chain->SetBranchAddress("WienerChi2_chaleur_FID804AB",&Chi1);
  chain->SetBranchAddress("WienerChi2_chaleur_FID804CD",&Chi2);

  for (int i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    Eh1=Achal1*calib_chal1; Eh2=Achal2*calib_chal2;
    Chi2heat1=Chi1; Chi2heat2=Chi2;
    t2->Fill();
  }
  f2->cd(); t2->Write(0,TObject::kOverwrite);
  f2->Close();
  */

  // Coupure chi2 sur la chal
  float chi2cut_chal1_a=1.3, chi2cut_chal1_b=0.00007;
  float chi2cut_chal2_a=1.3, chi2cut_chal2_b=0.00014;

  // Analyse globale..
  TFile* f = new TFile((ntpdir+"eion.root").c_str(),"READ");
  TTree* t = (TTree*)f->Get("Eion");
  TFile* f2 = new TFile((ntpdir+"eheat.root").c_str(),"READ");
  TTree* t2 = (TTree*)f2->Get("Eheat");
  t->AddFriend(t2);
  Float_t Eh1, Eh2, Eia, Eib, Eic, Eid, Eig, Eih, Ch1, Ch2;
  t->SetBranchAddress("Eh1",&Eh1);
  t->SetBranchAddress("Eh2",&Eh2);
  t->SetBranchAddress("Eia",&Eia);
  t->SetBranchAddress("Eib",&Eib);
  t->SetBranchAddress("Eic",&Eic);
  t->SetBranchAddress("Eid",&Eid);
  t->SetBranchAddress("Eig",&Eig);
  t->SetBranchAddress("Eih",&Eih);
  t->SetBranchAddress("Chi2heat1",&Ch1);
  t->SetBranchAddress("Chi2heat2",&Ch2);

  //  t->Draw("Eh1:Eh2");
  //  t->Draw("Eh1:Eh2","Chi2heat1<1.3+0.00007*Eh1*Eh1 && Chi2heat2<1.3+0.00014*Eh2*Eh2","same");

  // A la hache:
  // Coupure sur ion:  0.8<Ebottom/Etop<1.1
  // sur heat: 0.95<Eh2/Eh1<1.1

  // Qplot
  float volt=4, eps=3;
  int nb=t->GetEntries();
  Float_t* Er = new Float_t[nb];
  Float_t* Q = new Float_t[nb];
  Float_t* Qcut = new Float_t[nb];
  for (int i=0; i<nb; i++) {
    t->GetEntry(i);
    bool ok_glob=1, ok_cut=1;
    if (Eh2 > 1.1*Eh1 || Eh2<0.95*Eh1) ok_cut=0;
    if (Ch1 > chi2cut_chal1_a+chi2cut_chal1_a*Eh1*Eh1) ok_glob=0;
    if (Ch2 > chi2cut_chal2_a+chi2cut_chal2_a*Eh2*Eh2) ok_glob=0;
    if (Eia > 10 || Eic > 10 ) ok_glob=0;
    float Etop=Eib+Eig, Ebot=Eid+Eih;
    if (Ebot> 1.1*Etop || Ebot<0.8*Etop) ok_glob=0;
    if (!ok_glob) {
      Er[i]=0; Q[i]=0; Qcut[i]=0;
    } else {
      float Eheat_av=(Eh1+Eh2)/2.;
      float Eion_av=(Etop+Ebot)/2.;
      Er[i]=Eheat_av*(1+volt/eps)-volt*Eion_av/eps;
      if (Er[i] > 1) Q[i]=Eion_av/Er[i];
      else Q[i]=0;
      if (ok_cut) Qcut[i]=Q[i];
      else Qcut[i]=0;
    }
  }

  TGraph* gr = new TGraph(nb,Er,Q);
  TGraph* grc = new TGraph(nb,Er,Qcut);
  grc->SetMarkerColor(kRed);
  gr->SetMarkerStyle(7);
  grc->SetMarkerStyle(7);
  gr->Draw("AP");
  grc->Draw("sameP");

  f->Close(); f2->Close();

}
