
int CutVetos(TChain* chain, string eionfile, string bolo, Float_t sigmacut_vet, Float_t sigmacut_delta, Float_t sigmacut_etot, Float_t deltabin_col=10., Float_t deltapercentcut=5, Float_t vetopercentcut=2) {
  
    // cutparams convention:
    // ----OLD---- :
    // ID: cutcol_a, cutcol_b, cutvet1, cutvet2, cutgar1, cutgar2
    // FID: cutcol_a, cutcol_b, cutvet1, cutvet2
    // Classic ntd: cutgar1
    // sigma1, sigma2: used for FID/ID to weight the total collectrode energy estimator
  
    // vetopercentcut "x" : ne pas couper a haute energie si Evet < x% Efid.
    // Additional cut : il faut |Bin col1 - Bin col2| < deltabin_col
    // AND ADDITIONNAL CUT : si |Bin vet - Bin fid| < 2 et Evet > pas gd chose, on coupe
  Float_t atten=2; // on coupe la a sigmacut_vet/atten
  Float_t deltabin_vet=2;
  
  
  
  string simplentp=(eionfile.substr(0,eionfile.length()-5))+"_simple.root";
  system(("cp "+simplentp+" "+eionfile).c_str());
  
  TFile* f = new TFile(eionfile.c_str(),"UPDATE");
  TTree* Eion = (TTree*)f->Get("energytree");
  Int_t nb=Eion->GetEntries();
  if (chain->GetEntries() != nb) cout <<"pbl trees"<<endl;
  chain->SetBranchStatus("*",0);
  
  Bool_t vetocut=0;
  Float_t Efid=0, Ldbfid=0, Etot=0; // pas de ldbtot pour l'instant...
  Float_t EsideCut=0;
  Eion->Branch("VetoCut",&vetocut,"VetoCut/O");
  Eion->Branch("Efid",&Efid,"Efid/F");
  Eion->Branch("LdbFid",&Ldbfid,"LdbFid/F");
  Eion->Branch("Etot",&Etot,"Etot/F");
  Eion->Branch("EsideCut",&EsideCut,"EsideCut/F");
  
  Float_t Ecol1=0, Ecol2=0, Ldbcol1, Ldbcol2;
  Float_t Evet1=0, Evet2=0, Ldbvet1, Ldbvet2;
  Float_t Egar1=0, Egar2=0, Ldbgar1, Ldbgar2;
  Float_t BinCol1=0, BinCol2=0, BinVet1=0, BinVet2=0, BinGar1=0, BinGar2=0;  
  if (bolo.substr(0,2) == "ID") { // ID
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),&BinCol1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),&BinCol2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),&BinVet1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),&BinVet2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),&BinGar1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),&BinGar2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),1);
    Eion->SetBranchAddress("Ecol1",&Ecol1); 
    Eion->SetBranchAddress("Ecol2",&Ecol2);
    Eion->SetBranchAddress("Evet1",&Evet1); 
    Eion->SetBranchAddress("Evet2",&Evet2);
    Eion->SetBranchAddress("Egar1",&Egar1); 
    Eion->SetBranchAddress("Egar2",&Egar2);
    Eion->SetBranchAddress("LdbCol1",&Ldbcol1); 
    Eion->SetBranchAddress("LdbCol2",&Ldbcol2);
    Eion->SetBranchAddress("LdbVet1",&Ldbvet1); 
    Eion->SetBranchAddress("LdbVet2",&Ldbvet2);
    Eion->SetBranchAddress("LdbGar1",&Ldbgar1); 
    Eion->SetBranchAddress("LdbGar2",&Ldbgar2);
  } else if (bolo.substr(0,3) == "FID") { // FID
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),&BinCol1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),&BinCol2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),&BinVet1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),&BinVet2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),1);
    Eion->SetBranchAddress("Ecol1",&Ecol1); 
    Eion->SetBranchAddress("Ecol2",&Ecol2);
    Eion->SetBranchAddress("Evet1",&Evet1); 
    Eion->SetBranchAddress("Evet2",&Evet2);
    Eion->SetBranchAddress("LdbCol1",&Ldbcol1); 
    Eion->SetBranchAddress("LdbCol2",&Ldbcol2);
    Eion->SetBranchAddress("LdbVet1",&Ldbvet1); 
    Eion->SetBranchAddress("LdbVet2",&Ldbvet2);
  } else { // Classic GeNTD
    Eion->SetBranchAddress("Ecol1",&Ecol1); 
    Eion->SetBranchAddress("Egar1",&Egar1);
    Eion->SetBranchAddress("LdbCol1",&Ldbcol1); 
    Eion->SetBranchAddress("LdbGar1",&Ldbgar1); 
  }
  
  Float_t w1, w2, DeltaE, Ldb_DeltaE, invdenom, truc;
  Float_t Ethreshold ; // cutpercent
  
  if (bolo.substr(0,2) == "ID") {
    for (Int_t i=0; i<nb; i++) {
      Eion->GetEntry(i); chain->GetEntry(i);
      if (Ldbcol1==0 || Ldbcol2 == 0 || Ldbvet1 == 0 || Ldbvet2 == 0 ||
          Ldbgar1 ==0 || Ldbgar2 == 0) Ldbcol1=Ldbcol2=Ldbvet1=Ldbvet2=Ldbgar1=Ldbgar2=1;
      
      invdenom=1./(Ldbcol2*Ldbcol2+Ldbcol1*Ldbcol1);
      w1=Ldbcol2*Ldbcol2*invdenom;
      w2=Ldbcol1*Ldbcol1*invdenom;
      Efid = w1*Ecol1+w2*Ecol2;
      Ldbfid = Ldbcol1*Ldbcol2*sqrt(invdenom);
      
        //      Etot = (Ecol1+Ecol2+Evet1+Evet2+Egar1+Egar2)/2.;
      Etot = 0;
      if (Ecol1 > Ldbcol1*sigmacut_etot) Etot += Ecol1;
      if (Ecol2 > Ldbcol2*sigmacut_etot) Etot += Ecol2;
      if (Evet1 > Ldbvet1*sigmacut_etot) Etot += Evet1;
      if (Evet2 > Ldbvet2*sigmacut_etot) Etot += Evet2;
      if (Egar1 > Ldbgar1*sigmacut_etot) Etot += Egar1;
      if (Egar2 > Ldbgar2*sigmacut_etot) Etot += Egar2;
      Etot /= 2.;
      
      DeltaE = TMath::Abs(Ecol1-Ecol2);
      Ldb_DeltaE = sqrt(Ldbcol2*Ldbcol2+Ldbcol1*Ldbcol1);
      truc=0.01*vetopercentcut*Efid;
      EsideCut = sigmacut_delta*Ldb_DeltaE;
      if ( (Evet1 > Ldbvet1*sigmacut_vet && Evet1 > truc) || 
          (Evet2 > Ldbvet2*sigmacut_vet && Evet2 > truc) || 
          (Egar1 > Ldbgar1*sigmacut_vet && Egar1 > truc) || 
          (Egar2 > Ldbgar2*sigmacut_vet && Egar2 > truc) || 
          (DeltaE > EsideCut  && DeltaE > 0.01*deltapercentcut*Efid) )
        vetocut=0;
      else vetocut=1;
      if (TMath::Abs(BinCol1-BinCol2) > deltabin_col) vetocut=0;
      else {
        Int_t BinFid=(BinCol1+BinCol2)/2;
        if (TMath::Abs(BinFid-BinVet1) <= deltabin_vet && Evet1 > truc && 
            Evet1 > Ldbvet1*sigmacut_vet/atten) vetocut=0;
        else if (TMath::Abs(BinFid-BinVet2) <= deltabin_vet && Evet2 > truc && 
                 Evet2 > Ldbvet2*sigmacut_vet/atten) vetocut=0;
        else if (TMath::Abs(BinFid-BinGar1) <= deltabin_vet && Egar1 > truc && 
                 Egar1 > Ldbgar1*sigmacut_vet/atten) vetocut=0;
        else if (TMath::Abs(BinFid-BinGar2) <= deltabin_vet && Egar2 > truc && 
                 Egar2 > Ldbgar2*sigmacut_vet/atten) vetocut=0;
      }
      
      if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
      Eion->GetBranch("Efid")->Fill();
      Eion->GetBranch("VetoCut")->Fill();
      Eion->GetBranch("LdbFid")->Fill();
      Eion->GetBranch("Etot")->Fill();
      Eion->GetBranch("EsideCut")->Fill();
    }
  } else if (bolo.substr(0,3) == "FID") {
    for (Int_t i=0; i<nb; i++) {
      Eion->GetEntry(i); chain->GetEntry(i);
      if (Ldbcol1==0 || Ldbcol2 == 0 || Ldbvet1 == 0 || Ldbvet2 == 0)
        Ldbcol1=Ldbcol2=Ldbvet1=Ldbvet2=1;
      
      invdenom=1./(Ldbcol2*Ldbcol2+Ldbcol1*Ldbcol1);
      w1=Ldbcol2*Ldbcol2*invdenom;
      w2=Ldbcol1*Ldbcol1*invdenom;
      Efid = w1*Ecol1+w2*Ecol2;
      Ldbfid = Ldbcol1*Ldbcol2*sqrt(invdenom);
        //      Etot = (Ecol1+Ecol2+Evet1+Evet2)/2.;
      Etot = 0;
      if (Ecol1 > Ldbcol1*sigmacut_etot) Etot += Ecol1;
      if (Ecol2 > Ldbcol2*sigmacut_etot) Etot += Ecol2;
      if (Evet1 > Ldbvet1*sigmacut_etot) Etot += Evet1;
      if (Evet2 > Ldbvet2*sigmacut_etot) Etot += Evet2;
      Etot /= 2.;
      
      
      DeltaE = TMath::Abs(Ecol1-Ecol2);
      Ldb_DeltaE = sqrt(Ldbcol2*Ldbcol2+Ldbcol1*Ldbcol1);
      truc=0.01*vetopercentcut*Efid;
      EsideCut = sigmacut_delta*Ldb_DeltaE;
      if ( (Evet1 > Ldbvet1*sigmacut_vet && Evet1 > truc) || 
          (Evet2 > Ldbvet2*sigmacut_vet && Evet2 > truc) || 
          (DeltaE > EsideCut && DeltaE > 0.01*deltapercentcut*Efid) )
        vetocut=0;
      else vetocut=1;
      if (TMath::Abs(BinCol1-BinCol2) > deltabin_col) vetocut=0;
      else {
        Int_t BinFid=(BinCol1+BinCol2)/2;
        if (TMath::Abs(BinFid-BinVet1) < deltabin_vet && Evet1 > truc && 
            Evet1 > Ldbvet1*sigmacut_vet/atten) vetocut=0;
        else if (TMath::Abs(BinFid-BinVet2) < deltabin_vet && Evet2 > truc && 
                 Evet2 > Ldbvet2*sigmacut_vet/atten) vetocut=0;
      }
      
      if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
      Eion->GetBranch("Efid")->Fill();
      Eion->GetBranch("VetoCut")->Fill();
      Eion->GetBranch("LdbFid")->Fill();
      Eion->GetBranch("Etot")->Fill();
      Eion->GetBranch("EsideCut")->Fill();
    }
  } else {
    for (Int_t i=0; i<nb; i++) {
      Eion->GetEntry(i);
      Efid = Ecol1;
      Ldbfid = Ldbcol1;
      Etot = (Ecol1+Egar1)/2.;
      if (Ldbgar1 == 0) Ldbgar1=1;
      truc=0.01*vetopercentcut*Efid;
      
      if ( (Egar1 > Ldbgar1*sigmacut_vet && Egar1 > truc) )
        vetocut=0;
      else vetocut=1;
      if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
      Eion->GetBranch("Efid")->Fill();
      Eion->GetBranch("VetoCut")->Fill();
      Eion->GetBranch("LdbFid")->Fill();
      Eion->GetBranch("Etot")->Fill();
    }
  }
  
    //  Int_t optionecriture=TObject::kWriteDelete; // root
  Int_t optionecriture=TObject::kOverwrite; // root
  f->cd(); Eion->Write(0,optionecriture);
  f->Close();
  chain->SetBranchStatus("*",1);
  
  return 0;
}
