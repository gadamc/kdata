

int BuildDSTRed(TChain* chain, string bolo, TTree* EionTree, TTree* EheatTree, TTree* Cuts, TTree* DSTFull, TTree* CoinTree, TTree* TstTree, string dstredfile) {

  Int_t nbevts = chain->GetEntries();
  if (EionTree->GetEntries() != nbevts || EheatTree->GetEntries() != nbevts || 
      Cuts->GetEntries() != nbevts || DSTFull->GetEntries() != nbevts )
    cout << "Pbl nb of entries in input trees" << endl;
  chain->SetBranchStatus("*",0);
  bool use_tst_coin=1;
  if (TstTree == NULL || CoinTree == NULL) use_tst_coin=0;
  else {
    if (CoinTree->GetEntries()!=nbevts || TstTree->GetEntries()!=nbevts) 
      cout << "Pbl nb of entries in coin/tst trees" << endl;
  }

  Int_t evtflag_in=0, evtnum_in=0, Vflag_in=0, chi2flag_in=0, toffset=0;
  char runname_in[9]; ULong64_t date_in;
  Float_t Erec_in=0, Q_in=0, Eion_in=0;
  Int_t ionflags_in[6];
  DSTFull->SetBranchAddress("EventFlag",&evtflag_in);
  DSTFull->SetBranchAddress("RunName",&runname_in);
  DSTFull->SetBranchAddress("EventNum",&evtnum_in);
  DSTFull->SetBranchAddress("DateSec",&date_in);
  DSTFull->SetBranchAddress("Vflag",&Vflag_in);
  DSTFull->SetBranchAddress("Erec",&Erec_in);
  DSTFull->SetBranchAddress("Q",&Q_in);
  DSTFull->SetBranchAddress("IonFlags",ionflags_in);
  DSTFull->SetBranchAddress("Eion",&Eion_in);
  DSTFull->SetBranchAddress("Chi2Flag",&chi2flag_in);
  DSTFull->SetBranchAddress("Toffset",&toffset);

  Float_t Ecol1_in=0, Ecol2_in=0, Ldbcol1_in=0, Ldbcol2_in=0;
  Float_t Evet1_in=0, Evet2_in=0, Ldbvet1_in=0, Ldbvet2_in=0;
  Float_t Egar1_in=0, Egar2_in=0, Ldbgar1_in=0, Ldbgar2_in=0;
  Float_t BinCol1_in=0, BinCol2_in=0, BinVet1_in=0;
  Float_t BinVet2_in=0, BinGar1_in=0, BinGar2_in=0;  
  Bool_t CutChi2Col1_in=0, CutChi2Col2_in=0, CutChi2Vet1_in=0;
  Bool_t CutChi2Vet2_in=0, CutChi2Gar1_in=0, CutChi2Gar2_in=0;
  Float_t Egar1sync_in=0, Egar2sync_in=0, Evet1sync_in=0, Evet2sync_in=0;
  Float_t Ldbfid_in=0;
  chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),&BinCol1_in);
  chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),1);
  Cuts->SetBranchAddress("CutChi2Col1",&CutChi2Col1_in);
  EionTree->SetBranchAddress("Ecol1",&Ecol1_in); 
  EionTree->SetBranchAddress("LdbFid",&Ldbfid_in);
  EionTree->SetBranchAddress("LdbCol1",&Ldbcol1_in);
  if (!IsID(bolo)) {
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),&BinGar1_in);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),1);
    Cuts->SetBranchAddress("CutChi2Gar1",&CutChi2Gar1_in);
    EionTree->SetBranchAddress("Egar1",&Egar1_in); 
    EionTree->SetBranchAddress("LdbGar1",&Ldbgar1_in);
    EionTree->SetBranchAddress("Egar1sync",&Egar1sync_in); 
  } else {
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),&BinCol2_in);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col2")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),&BinVet1_in);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),&BinVet2_in);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"vet2")).c_str(),1);
    EionTree->SetBranchAddress("Ecol2",&Ecol2_in);
    EionTree->SetBranchAddress("Evet1",&Evet1_in); 
    EionTree->SetBranchAddress("Evet2",&Evet2_in);
    EionTree->SetBranchAddress("LdbCol2",&Ldbcol2_in);
    EionTree->SetBranchAddress("LdbVet1",&Ldbvet1_in); 
    EionTree->SetBranchAddress("LdbVet2",&Ldbvet2_in);
    EionTree->SetBranchAddress("Evet1sync",&Evet1sync_in); 
    EionTree->SetBranchAddress("Evet2sync",&Evet2sync_in);
    Cuts->SetBranchAddress("CutChi2Vet1",&CutChi2Vet1_in);
    Cuts->SetBranchAddress("CutChi2Vet2",&CutChi2Vet2_in);
    Cuts->SetBranchAddress("CutChi2Col2",&CutChi2Col2_in);
    if (bolo.substr(0,2) == "ID") {
      chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),&BinGar1_in);
      chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),1);
      chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),&BinGar2_in);
      chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),1);
      EionTree->SetBranchAddress("Egar1",&Egar1_in); 
      EionTree->SetBranchAddress("Egar2",&Egar2_in);
      EionTree->SetBranchAddress("LdbGar1",&Ldbgar1_in); 
      EionTree->SetBranchAddress("LdbGar2",&Ldbgar2_in);
      EionTree->SetBranchAddress("Egar1sync",&Egar1sync_in); 
      EionTree->SetBranchAddress("Egar2sync",&Egar2sync_in); 
      Cuts->SetBranchAddress("CutChi2Gar1",&CutChi2Gar1_in);
      Cuts->SetBranchAddress("CutChi2Gar2",&CutChi2Gar2_in);
    }
  }
  Float_t Eheat_in=0, Ldbheat_in=0; Bool_t CutChi2Heat_in=0;
  EheatTree->SetBranchAddress("Eheat",&Eheat_in);
  EheatTree->SetBranchAddress("LdbHeat",&Ldbheat_in);
  Cuts->SetBranchAddress("CutChi2Heat",&CutChi2Heat_in);
  Int_t CoinTot=0;
  if (use_tst_coin)
    CoinTree->SetBranchAddress("CoinTot",&CoinTot);

  TFile* f = new TFile(dstredfile.c_str(),"RECREATE");
  TTree* dstred = new TTree("dstred","EdwDSTRed");
  Int_t evtflag=0, evtnum=0, dstnum=0, Vflag=0, chi2flag=0;
  char runname[9]; ULong64_t date, TimeStamp;
  Float_t Erec=0, Q=0, Eion=0;
  Int_t ionflags[6]; Bool_t IsBoloCoin=0, IsVetoCoin=0;
  dstred->Branch("EventFlag",&evtflag,"EventFlag/I");
  dstred->Branch("RunName",&runname,"RunName/C");
  dstred->Branch("EventNum",&evtnum,"EventNum/I");
  dstred->Branch("DSTNum",&dstnum,"DSTNum/I");
  dstred->Branch("DateSec",&date,"DateSec/l");
  dstred->Branch("TimeStamp",&TimeStamp,"TimeStamp/l");
  dstred->Branch("IsBoloCoinc",&IsBoloCoin,"IsBoloCoin/O");
  dstred->Branch("IsVetoCoinc",&IsVetoCoin,"IsVetoCoin/O");
  dstred->Branch("Vflag",&Vflag,"Vflag/I");
  dstred->Branch("Erec",&Erec,"Erec/F");
  dstred->Branch("Q",&Q,"Q/F");
  dstred->Branch("IonFlags",ionflags,"IonFlags[6]/I");
  dstred->Branch("Eion",&Eion,"Eion/F");
  dstred->Branch("Chi2Flag",&chi2flag,"Chi2Flag/I");
  Float_t Ecol1=0, Ecol2=0, Ldbcol1=0, Ldbcol2=0;
  Float_t Evet1=0, Evet2=0, Ldbvet1=0, Ldbvet2=0;
  Float_t Egar1=0, Egar2=0, Ldbgar1=0, Ldbgar2=0;
  Float_t BinCol1=0, BinCol2=0, BinVet1=0;
  Float_t BinVet2=0, BinGar1=0, BinGar2=0, Ldbfid=0;
  Bool_t CutChi2Col1=0, CutChi2Col2=0, CutChi2Vet1=0;
  Bool_t CutChi2Vet2=0, CutChi2Gar1=0, CutChi2Gar2=0;
  Float_t Egar1sync=0, Egar2sync=0, Evet1sync=0, Evet2sync=0;
  
  dstred->Branch("Ecol1",&Ecol1,"Ecol1/F");
  dstred->Branch("BinCol1",&BinCol1,"BinCol1/F");
  dstred->Branch("LdbCol1",&Ldbcol1,"LdbCol1/F");
  dstred->Branch("CutChi2Col1",&CutChi2Col1,"CutChi2Col1/O");
  dstred->Branch("LdbFid",&Ldbfid,"LdbFid/F");
  if (!IsID(bolo)) {
    dstred->Branch("Egar1",&Egar1,"Egar1/F");
    dstred->Branch("BinGar1",&BinGar1,"BinGar1/F");
    dstred->Branch("LdbGar1",&Ldbgar1,"LdbGar1/F");
    dstred->Branch("CutChi2Gar1",&CutChi2Gar1,"CutChi2Gar1/O");
    dstred->Branch("Egar1sync",&Egar1sync,"Egar1sync/F");
  } else {
    dstred->Branch("Ecol2",&Ecol2,"Ecol2/F");
    dstred->Branch("BinCol2",&BinCol2,"BinCol2/F");
    dstred->Branch("LdbCol2",&Ldbcol2,"LdbCol2/F");
    dstred->Branch("CutChi2Col2",&CutChi2Col2,"CutChi2Col2/O");
    dstred->Branch("Evet1",&Evet1,"Evet1/F");
    dstred->Branch("BinVet1",&BinVet1,"BinVet1/F");
    dstred->Branch("LdbVet1",&Ldbvet1,"LdbVet1/F");
    dstred->Branch("CutChi2Vet1",&CutChi2Vet1,"CutChi2Vet1/O");
    dstred->Branch("Evet2",&Evet2,"Evet2/F");
    dstred->Branch("BinVet2",&BinVet2,"BinVet2/F");
    dstred->Branch("LdbVet2",&Ldbvet2,"LdbVet2/F");
    dstred->Branch("CutChi2Vet2",&CutChi2Vet2,"CutChi2Vet2/O");
    dstred->Branch("Evet1sync",&Evet1sync,"Evet1sync/F");
    dstred->Branch("Evet2sync",&Evet2sync,"Evet2sync/F");
    if (bolo.substr(0,2) == "ID") {
      dstred->Branch("Egar1",&Egar1,"Egar1/F");
      dstred->Branch("BinGar1",&BinGar1,"BinGar1/F");
      dstred->Branch("LdbGar1",&Ldbgar1,"LdbGar1/F");
      dstred->Branch("CutChi2Gar1",&CutChi2Gar1,"CutChi2Gar1/O");
      dstred->Branch("Egar2",&Egar2,"Egar2/F");
      dstred->Branch("BinGar2",&BinGar2,"BinGar2/F");
      dstred->Branch("LdbGar2",&Ldbgar2,"LdbGar2/F");
      dstred->Branch("CutChi2Gar2",&CutChi2Gar2,"CutChi2Gar2/O");
      dstred->Branch("Egar1sync",&Egar1sync,"Egar1sync/F");
      dstred->Branch("Egar2sync",&Egar2sync,"Egar2sync/F");
    }
  }
  Float_t Eheat=0, Ldbheat=0; Bool_t CutChi2Heat=0;
  dstred->Branch("Eheat",&Eheat,"Eheat/F");
  dstred->Branch("LdbHeat",&Ldbheat,"LdbHeat/F");
  dstred->Branch("CutChi2Heat",&CutChi2Heat,"CutChi2Heat/O");
  
  ULong64_t timestamp_in=0;
  if (use_tst_coin) TstTree->SetBranchAddress("TimeStamp",&timestamp_in);
  
  for (Int_t i=0; i<nbevts; i++) {
    DSTFull->GetEntry(i);
    if (evtflag_in >= 2) {
      chain->GetEntry(i);
      EionTree->GetEntry(i); EheatTree->GetEntry(i);
      Cuts->GetEntry(i); 
      if (use_tst_coin) {
	CoinTree->GetEntry(i);
	TstTree->GetEntry(i);
      }
      evtflag = evtflag_in; evtnum=evtnum_in;
      strcpy(runname,runname_in);
      dstnum=i; date=date_in; Vflag = Vflag_in;
      Erec = Erec_in; Q = Q_in; Eion = Eion_in; chi2flag = chi2flag_in;
      for (int k=0; k<6; k++) ionflags[k]=ionflags_in[k];
      Ecol1=Ecol1_in; BinCol1=BinCol1_in; Ldbcol1=Ldbcol1_in; CutChi2Col1=CutChi2Col1_in;
      Ecol2=Ecol2_in; BinCol2=BinCol2_in; Ldbcol2=Ldbcol2_in; CutChi2Col2=CutChi2Col2_in;
      Evet1=Evet1_in; BinVet1=BinVet1_in; Ldbvet1=Ldbvet1_in; CutChi2Vet1=CutChi2Vet1_in;
      Evet2=Evet2_in; BinVet2=BinVet2_in; Ldbvet2=Ldbvet2_in; CutChi2Vet2=CutChi2Vet2_in;
      Egar1=Egar1_in; BinGar1=BinGar1_in; Ldbgar1=Ldbgar1_in; CutChi2Gar1=CutChi2Gar1_in;
      Egar2=Egar2_in; BinGar2=BinGar2_in; Ldbgar2=Ldbgar2_in; CutChi2Gar2=CutChi2Gar2_in;
      Egar1sync=Egar1sync_in; Egar2sync=Egar2sync_in;
      Evet1sync=Evet1sync_in; Evet2sync=Evet2sync_in;
      Eheat=Eheat_in; Ldbheat=Ldbheat_in; CutChi2Heat=CutChi2Heat_in;
      Ldbfid=Ldbfid_in;
      IsBoloCoin=0; if (CoinTot >= 1) IsBoloCoin=1;
      // tstnew
      /*      if (strcmp(runname,currentrun)) {
	current_tstfile.close();
	strcpy(currentrun,runname);
	cout << "new run "<<currentrun<<endl;
	string yoyo(currentrun);
	current_tstfile.open((tstnew_dir+"/"+yoyo+"_tstnew_v1").c_str(),ios::in);
	if (!current_tstfile) cout << "Aie! no tstnew file" << endl;
	getline(current_tstfile,yoyo); getline(current_tstfile,yoyo);
	getline(current_tstfile,yoyo);
      }
      do {
	if (current_tstfile.eof()) cout << "fin du tstred file.." << endl;
	current_tstfile >> a >> b;
      } while (a != evtnum);
      TimeStamp = (ULong64_t)(b+toffset); */
      TimeStamp = (ULong64_t)(((Long64_t)timestamp_in)+toffset);
      // fin tstnew

      dstred->Fill();
    }
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  cout << "Fin de la boucle" << endl;
  //  if (!current_tstfile) cout << "pbl?" <<endl;
  //  current_tstfile.close();
  cout << "Fermeture fichier.."<<endl;

  f->cd(); dstred->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);

  return 0;
}
