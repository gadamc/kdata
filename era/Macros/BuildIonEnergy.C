

Int_t BuildIonEnergy(TChain* chain, string bolo, string* runs=NULL, int* sgn_ion=NULL, int nbruns, string filename="tmpfile.root", bool special_flag_id3=0) {
  
  // Convention for vector of calibration coefficients, by order:
  // ID: col1,col2,vet1,vet2,gar1,gar2
  // FID: col1,col2,vet1,vet2
  // Standard GeNtd: col1,gar1
  
  // Convention for vector of crosstalk coefficients, by order:
  // FID: v1->c1,c1->v1,v2->c2,c2->v2
  // ID: same+g2->g1,g1->g2,heat->g1(lin and quad),heat->g2(lin and quad)
  // Standard GeNTD: g->c,c->g
  
  // special_flag_id3 : change in the voltage for ID3 col1
  // the gain of that electrode is a bit different for a given period in March 2010.
  
  Float_t special_gain_id3=1./0.552;
  ULong64_t special_ti_id3=1267106000, special_tf_id3=1270159616;
  ULong64_t date=0;
  
  TFile* f = new TFile(filename.c_str(),"RECREATE");
  TTree* t = new TTree("energytree","EdwIonEnergy");
  Float_t Ecol1=0, Ecol2=0, E0col1=0, E0col2=0;
  Float_t Evet1=0, Evet2=0, E0vet1=0, E0vet2=0;
  Float_t Egar1=0, Egar2=0, E0gar1=0, E0gar2=0;
  Float_t Evet1sync=0, Evet2sync=0, Egar1sync=0, Egar2sync=0;
  
  bool isntd=0;
  if (bolo.substr(0,2) == "ID") { // ID
    t->Branch("Ecol1",&Ecol1,"Ecol1/F"); t->Branch("Ecol2",&Ecol2,"Ecol2/F");
    t->Branch("Evet1",&Evet1,"Evet1/F"); t->Branch("Evet2",&Evet2,"Evet2/F");
    t->Branch("Egar1",&Egar1,"Egar1/F"); t->Branch("Egar2",&Egar2,"Egar2/F");
    t->Branch("E0col1",&E0col1,"E0col1/F"); t->Branch("E0col2",&E0col2,"E0col2/F");
    t->Branch("E0vet1",&E0vet1,"E0vet1/F"); t->Branch("E0vet2",&E0vet2,"E0vet2/F");
    t->Branch("E0gar1",&E0gar1,"E0gar1/F"); t->Branch("E0gar2",&E0gar2,"E0gar2/F");
    t->Branch("Evet1sync",&Evet1sync,"Evet1sync/F"); t->Branch("Evet2sync",&Evet2sync,"Evet2sync/F");
    t->Branch("Egar1sync",&Egar1sync,"Egar1sync/F"); t->Branch("Egar2sync",&Egar2sync,"Egar2sync/F");
  } else if (bolo.substr(0,3) == "FID") { // FID
    t->Branch("Ecol1",&Ecol1,"Ecol1/F"); t->Branch("Ecol2",&Ecol2,"Ecol2/F");
    t->Branch("Evet1",&Evet1,"Evet1/F"); t->Branch("Evet2",&Evet2,"Evet2/F");
    t->Branch("E0col1",&E0col1,"E0col1/F"); t->Branch("E0col2",&E0col2,"E0col2/F");
    t->Branch("E0vet1",&E0vet1,"E0vet1/F"); t->Branch("E0vet2",&E0vet2,"E0vet2/F");
    t->Branch("Evet1sync",&Evet1sync,"Evet1sync/F"); t->Branch("Evet2sync",&Evet2sync,"Evet2sync/F");
  } else { // Classic GeNTD
    isntd=1;
    t->Branch("Ecol1",&Ecol1,"Ecol1/F"); t->Branch("Egar1",&Egar1,"Egar1/F");
    t->Branch("E0col1",&E0col1,"E0col1/F"); t->Branch("E0gar1",&E0gar1,"E0gar1/F");
    t->Branch("Egar1sync",&Egar1sync,"Egar1sync/F");
  }
  Int_t Vflag=1;
  t->Branch("PolarFlag",&Vflag,"PolarFlag/I");
  
  Int_t nb = chain->GetEntries();
  char therun[9];
  string ampltype = "Wiener";
  Float_t Acol1=0, Acol2=0, A0col1=0, A0col2=0;
  Float_t Avet1=0, Avet2=0, A0vet1=0, A0vet2=0;
  Float_t Agar1=0, Agar2=0, A0gar1=0, A0gar2=0;
  Float_t Av1syn=0, Av2syn=0, Ag1syn=0, Ag2syn=0;
  Float_t Aheat=0;
  chain->SetBranchAddress("RunName",&therun);
  if (special_flag_id3) chain->SetBranchAddress("DateSec",&date);
  
  chain->SetBranchAddress(("WienerAmpl_"+ChannelName(bolo,"heat")).c_str(),&Aheat);
  if (bolo.substr(0,2) == "ID") { // ID
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"col1")).c_str(),&Acol1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"col2")).c_str(),&Acol2);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet1")).c_str(),&Avet1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet2")).c_str(),&Avet2);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar1")).c_str(),&Agar1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar2")).c_str(),&Agar2);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"col1")).c_str(),&A0col1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"col2")).c_str(),&A0col2);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"vet1")).c_str(),&A0vet1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"vet2")).c_str(),&A0vet2);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"gar1")).c_str(),&A0gar1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"gar2")).c_str(),&A0gar2);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet1")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),&Av1syn);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet2")+"_Sync_"+ChannelName(bolo,"col2")).c_str(),&Av2syn);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar1")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),&Ag1syn);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar2")+"_Sync_"+ChannelName(bolo,"col2")).c_str(),&Ag2syn);
  } else if (bolo.substr(0,3) == "FID") { // FID - collecting electrode = B
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"col1")).c_str(),&Acol1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"col2")).c_str(),&Acol2);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet1")).c_str(),&Avet1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet2")).c_str(),&Avet2);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"col1")).c_str(),&A0col1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"col2")).c_str(),&A0col2);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"vet1")).c_str(),&A0vet1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"vet2")).c_str(),&A0vet2);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet1")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),&Av1syn);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"vet2")+"_Sync_"+ChannelName(bolo,"col2")).c_str(),&Av2syn);
  } else { // Classic GeNTD
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"col1")).c_str(),&Acol1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar1")).c_str(),&Agar1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"col1")).c_str(),&A0col1);
    chain->SetBranchAddress((ampltype+"ZeroAmpl_"+ChannelName(bolo,"gar1")).c_str(),&A0gar1);
    chain->SetBranchAddress((ampltype+"Ampl_"+ChannelName(bolo,"gar1")+"_Sync_"+ChannelName(bolo,"col1")).c_str(),&Ag1syn);
  }
  
  int nbnan=0;
  for (Int_t i=0; i<nb; i++) {
    chain->GetEntry(i);
    Int_t runflag=0;
    if (runs != NULL) {
      for (int p=0; p<nbruns; p++) {
        if (!strcmp(therun,(runs[p]).c_str()))
          runflag = sgn_ion[p];
      }
    }
    Vflag=runflag;
    // Bad voltage : use the '0' polar calibration... (but keep Vflag=-1!)
    if (runflag == -1) runflag=0;
    if (runflag < 0 || runflag >= gNbVoltages) cout << "Error run flag" << endl;
    
    if (TMath::Finite(Acol1) == 0 || TMath::Finite(Acol2) == 0 
        || TMath::Finite(Avet1) == 0 || TMath::Finite(Avet2) == 0
        || TMath::Finite(Agar1) == 0 || TMath::Finite(Agar2) == 0 ) {
      nbnan++;
      Ecol1=0; Ecol2=0; Evet1=0; Evet2=0; Egar1=0; Egar2=0;
      E0col1=0; E0col2=0; E0vet1=0; E0vet2=0; E0gar1=0; E0gar2=0;
    } else {
      
      // Plus de distingo selon le signe du run
      // Crosstalk chaleur: Achal reste tjs positive donc le signe du coefficient
      // de cross talk doit logiquement changer en fct du signe polar (doit
      // etre pris en compte dans le fichier params...)
      
      if (isntd) { // definition differente des ceofs de crosstalk..
        Ecol1 = (Acol1-gCT_V1C1[runflag]*Agar1)*gCalib_Col1[runflag];
        E0col1 = (A0col1-gCT_V1C1[runflag]*Agar1)*gCalib_Col1[runflag];
        Egar1 = (Agar1-gCT_C1V1[runflag]*Acol1-gCTlin_HG1[runflag]*Aheat-
                 gCTquad_HG1[runflag]*Aheat*Aheat)*gCalib_Gar1[runflag];
        E0gar1 = (A0gar1-gCT_C1V1[runflag]*Acol1-gCTlin_HG1[runflag]*Aheat-
                  gCTquad_HG1[runflag]*Aheat*Aheat)*gCalib_Gar1[runflag];
      } else {
        Ecol1 = (Acol1-gCT_V1C1[runflag]*Avet1)*gCalib_Col1[runflag];
        Ecol2 = (Acol2-gCT_V2C2[runflag]*Avet2)*gCalib_Col2[runflag];
        E0col1 = (A0col1-gCT_V1C1[runflag]*Avet1)*gCalib_Col1[runflag]; // Choix correction CT pour les a0
        E0col2 = (A0col2-gCT_V2C2[runflag]*Avet2)*gCalib_Col2[runflag];
        Evet1 = (Avet1-gCT_C1V1[runflag]*Acol1)*gCalib_Vet1[runflag]; 
        Evet2 = (Avet2-gCT_C2V2[runflag]*Acol2)*gCalib_Vet2[runflag];
        E0vet1 = (A0vet1-gCT_C1V1[runflag]*Acol1)*gCalib_Vet1[runflag];
        E0vet2 = (A0vet2-gCT_C2V2[runflag]*Acol2)*gCalib_Vet2[runflag];
        Egar1 = (Agar1-gCT_G2G1[runflag]*Agar2-gCTlin_HG1[runflag]*Aheat-
                 gCTquad_HG1[runflag]*Aheat*Aheat)*gCalib_Gar1[runflag];
        Egar2 = (Agar2-gCT_G1G2[runflag]*Agar1-gCTlin_HG2[runflag]*Aheat-
                 gCTquad_HG2[runflag]*Aheat*Aheat)*gCalib_Gar2[runflag];
        E0gar1 = (A0gar1-gCT_G2G1[runflag]*Agar2-gCTlin_HG1[runflag]*Aheat-
                  gCTquad_HG1[runflag]*Aheat*Aheat)*gCalib_Gar1[runflag]; 
        E0gar2 = (A0gar2-gCT_G1G2[runflag]*Agar1-gCTlin_HG2[runflag]*Aheat-
                  gCTquad_HG2[runflag]*Aheat*Aheat)*gCalib_Gar2[runflag];
      }
      
      if (special_flag_id3) {
        if (date > special_ti_id3 && date < special_tf_id3) {
          Ecol1 *= special_gain_id3; E0col1 *= special_gain_id3;
        }
      }
      
      if (TMath::Finite(Av1syn) == 0 || TMath::Finite(Av2syn) == 0 
          || TMath::Finite(Ag1syn) == 0 || TMath::Finite(Ag2syn) == 0) {
        nbnan++;
        Evet1sync=0; Evet2sync=0; Egar1sync=0; Egar2sync=0;
      } else {
        Evet1sync = (Av1syn-gCT_V1sync[runflag]*Acol1)*gCalib_Vet1[runflag]; 
        Evet2sync = (Av2syn-gCT_V2sync[runflag]*Acol2)*gCalib_Vet2[runflag]; 
        Egar1sync = (Ag1syn-gCT_G1sync[runflag]*Acol1)*gCalib_Gar1[runflag]; 
        Egar2sync = (Ag2syn-gCT_G2sync[runflag]*Acol2)*gCalib_Gar2[runflag]; 
      }
    }
    
    t->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  cout << "** Nb of infinite amplitudes: "<<nbnan << endl;
  
  f->cd(); t->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);
  return 0;
}
