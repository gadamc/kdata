
#include "UpdateEnergy.C"
#include <vector>
#include <string>
#include "ChannelName.C"

int BuildCutFile(TChain* chain, string bolo, TTree* Eion, TTree* Eheat, string cutntpfile, string qualdir, Float_t* baselinelimit) {

  // Arbre cuts:
  // - mauvais runs
  // - periodes a mauvaises lignes de base
  // - evts a mauvais chi2

  // Initialisation de l'arbre
  TFile* f = new TFile(cutntpfile.c_str(),"RECREATE");
  TTree* cuts = new TTree("cuttree","EdwCuts");
  Bool_t cutrun=1, cutcol1base=1, cutcol2base=1, cutvet1base=1, cutvet2base=1;
  Bool_t cutgar1base=1, cutgar2base=1, cutbases=1;
  Bool_t cutchi2heat=1, cutchi2col1=1, cutchi2col2=1, cutchi2vet1=1, cutchi2vet2=1;
  Bool_t cutchi2gar1=1, cutchi2gar2=1, cutchi2=1, cutheatbase=1;
  cuts->Branch("CutRun",&cutrun,"CutRun/O");
  cuts->Branch("CutHeatBase",&cutheatbase,"CutHeatBase/O");
  cuts->Branch("CutCol1Base",&cutcol1base,"CutCol1Base/O");
  cuts->Branch("CutChi2Col1",&cutchi2col1,"CutChi2Col1/O");
  cuts->Branch("CutChi2Heat",&cutchi2heat,"CutChi2Heat/O");
  if (!IsID(bolo)) {
    cuts->Branch("CutGar1Base",&cutgar1base,"CutGar1Base/O");
    cuts->Branch("CutChi2Gar1",&cutchi2gar1,"CutChi2Gar1/O");
  } else {
    cuts->Branch("CutCol2Base",&cutcol2base,"CutCol2Base/O");
    cuts->Branch("CutVet1Base",&cutvet1base,"CutVet1Base/O");
    cuts->Branch("CutVet2Base",&cutvet2base,"CutVet2Base/O");

    cuts->Branch("CutChi2Col2",&cutchi2col2,"CutChi2Col2/O");
    cuts->Branch("CutChi2Vet1",&cutchi2vet1,"CutChi2Vet1/O");
    cuts->Branch("CutChi2Vet2",&cutchi2vet2,"CutChi2Vet2/O");
    if (!IsFID(bolo)) {
      cuts->Branch("CutGar1Base",&cutgar1base,"CutGar1Base/O");
      cuts->Branch("CutGar2Base",&cutgar2base,"CutGar2Base/O");
      cuts->Branch("CutChi2Gar1",&cutchi2gar1,"CutChi2Gar1/O");
      cuts->Branch("CutChi2Gar2",&cutchi2gar2,"CutChi2Gar2/O");
    }
  }
  cuts->Branch("CutChi2",&cutchi2,"CutChi2/O");
  cuts->Branch("CutBases",&cutbases,"CutBases/O");

  // Recupere chi2, energies, et ldbs des voies
  ULong64_t date=0; char therun[9];
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("RunName",&therun);
  chain->SetBranchStatus("RunName",1);
  Int_t nbevts=chain->GetEntries();
  if (Eion->GetEntries() != nbevts || Eheat->GetEntries() != nbevts)
    cout << "Pbl tree size" << endl;
  Float_t chi2col1=0,chi2col2=0,chi2heat=0, chi2heat_nosync=0;
  Float_t chi2vet1=0, chi2vet2=0, chi2gar1=0, chi2gar2=0;
  chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"col1")).c_str(),&chi2col1);
  chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"col1")).c_str(),1);
  chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"heat")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),&chi2heat);
  chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"heat")).c_str(),&chi2heat_nosync);
  chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"heat")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),1);
  chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"heat")).c_str(),1);
  if (!IsID(bolo)) {
    chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"gar1")).c_str(),&chi2gar1);
    chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"gar1")).c_str(),1);
  } else {
    chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"col2")).c_str(),&chi2col2);
    chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"vet1")).c_str(),&chi2vet1);
    chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"vet2")).c_str(),&chi2vet2);
    chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"col2")).c_str(),1);
    chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"vet1")).c_str(),1);
    chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"vet2")).c_str(),1);
    if (!IsFID(bolo)) {
      chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"gar1")).c_str(),&chi2gar1);
      chain->SetBranchAddress(("WienerChi2_"+ChannelName(bolo,"gar2")).c_str(),&chi2gar2);
      chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"gar1")).c_str(),1);
      chain->SetBranchStatus(("WienerChi2_"+ChannelName(bolo,"gar2")).c_str(),1);
    }
  }

  Float_t eheat=0, ldbheat=0;
  Float_t ldbcol1=0, ldbcol2=0, ldbvet1=0, ldbvet2=0, ldbgar1=0, ldbgar2=0;
  Float_t ecol1=0, ecol2=0, evet1=0, evet2=0, egar1=0, egar2=0;
  Eheat->SetBranchAddress("Eheat",&eheat);
  Eheat->SetBranchAddress("LdbHeat",&ldbheat);
  Eion->SetBranchAddress("Ecol1",&ecol1);
  Eion->SetBranchAddress("LdbCol1",&ldbcol1);
  if (!IsID(bolo)) {
    Eion->SetBranchAddress("Egar1",&egar1);
    Eion->SetBranchAddress("LdbGar1",&ldbgar1);
  } else {
    Eion->SetBranchAddress("Ecol2",&ecol2);
    Eion->SetBranchAddress("Evet1",&evet1);
    Eion->SetBranchAddress("Evet2",&evet2);
    Eion->SetBranchAddress("LdbCol2",&ldbcol2);
    Eion->SetBranchAddress("LdbVet1",&ldbvet1);
    Eion->SetBranchAddress("LdbVet2",&ldbvet2);
    if (!IsFID(bolo)) {
      Eion->SetBranchAddress("Egar1",&egar1);
      Eion->SetBranchAddress("Egar2",&egar2);
      Eion->SetBranchAddress("LdbGar1",&ldbgar1);
      Eion->SetBranchAddress("LdbGar2",&ldbgar2);
    }
  }
  Int_t boloflag=0;
  if (!IsID(bolo)) boloflag=2;
  if (IsFID(bolo)) boloflag=1;

  // Recupere mauvais runs
  vector<string> badruns;
  string run; int polar, flag;
  string runfile=qualdir+"/runlist_"+bolo+".txt";
  ifstream buf2(runfile.c_str(),ios::in);
  while (buf2 >> run >> polar >> flag) {
    if (!flag) badruns.push_back(run);
  }
  int nbbadruns=badruns.size();

  for (Int_t i=0; i<nbevts; i++) {
    chain->GetEntry(i); Eion->GetEntry(i);
    Eheat->GetEntry(i);

    // CutRun
    cutrun=1;
    for (int p=0; p<nbbadruns; p++) {
      if (!strcmp(therun,(badruns[p]).c_str()))	cutrun=0;
    }
    
    // Chi2
    cutchi2heat=1;
    if (chi2heat > gChi2Cut0_Heat+gChi2Cut1_Heat*eheat*eheat) cutchi2heat=0;
    if (TMath::Finite(chi2heat) == 0) {
      if (chi2heat_nosync > gChi2Cut0_Heat+gChi2Cut1_Heat*eheat*eheat) cutchi2heat=0;
    }
    cutchi2col1=1; cutchi2col2=1; cutchi2vet1=1; cutchi2vet2=1;
    cutchi2gar1=1; cutchi2gar2=1;
    if (chi2col1 > gChi2Cut0_Col1+gChi2Cut1_Col1*ecol1*ecol1) cutchi2col1=0;
    if (boloflag == 2) {
      if (chi2gar1 > gChi2Cut0_Gar1+gChi2Cut1_Gar1*egar1*egar1) cutchi2gar1=0;
    } else {
      if (chi2col2 > gChi2Cut0_Col2+gChi2Cut1_Col2*ecol2*ecol2) cutchi2col2=0;
      if (chi2vet1 > gChi2Cut0_Vet1+gChi2Cut1_Vet1*evet1*evet1) cutchi2vet1=0;
      if (chi2vet2 > gChi2Cut0_Vet2+gChi2Cut1_Vet2*evet2*evet2) cutchi2vet2=0;
      if (boloflag == 0) {
	if (chi2gar1 > gChi2Cut0_Gar1+gChi2Cut1_Gar1*egar1*egar1) cutchi2gar1=0;
	if (chi2gar2 > gChi2Cut0_Gar2+gChi2Cut1_Gar2*egar2*egar2) cutchi2gar2=0;
      }
    }
    cutchi2=0;
    if (cutchi2heat && cutchi2col1 && cutchi2col2) cutchi2=1;

    // Ldb (ldb=0 : ldb pas calculee car periode trop courte ==> on rejete)
    // CONVENTION : Si baselinelimit=0 pour une veto ou garde on cherche pas a rejeter de periode
    //   (sert pour les voies mortes)
    cutheatbase=1; cutcol1base=1; cutcol2base=1; cutvet1base=1;
    cutvet2base=1; cutgar1base=1; cutgar2base=1;
    if (ldbheat > baselinelimit[0] || ldbheat == 0) cutheatbase=0;
    if (ldbcol1 > baselinelimit[1] || ldbcol1 == 0) cutcol1base=0;
    if (boloflag == 2) {
      if (baselinelimit[5] != 0 && (ldbgar1 > baselinelimit[5] || ldbgar1 == 0)) cutgar1base=0;
    } else {
      if (ldbcol2 > baselinelimit[2] || ldbcol2 == 0) cutcol2base=0;
      if (baselinelimit[3] != 0 && (ldbvet1 > baselinelimit[3] || ldbvet1 == 0)) cutvet1base=0;
      if (baselinelimit[4] != 0 && (ldbvet2 > baselinelimit[4] || ldbvet2 == 0)) cutvet2base=0;
      if (boloflag == 0) {
	if (baselinelimit[5] != 0 && (ldbgar1 > baselinelimit[5] || ldbgar1 == 0)) cutgar1base=0;
	if (baselinelimit[6] != 0 && (ldbgar2 > baselinelimit[6] || ldbgar2 == 0)) cutgar2base=0;
      }
    }
    cutbases=0;
    if (cutheatbase && cutcol1base && cutcol2base && cutvet1base &&
	cutvet2base && cutgar1base && cutgar2base) cutbases=1;

    cuts->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }

  f->cd(); cuts->Write();
  f->Close();
  chain->SetBranchStatus("*",1);
  return 0;
}
