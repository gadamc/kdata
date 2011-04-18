
int BuildDST(TChain* chain, string bolo, TTree* Eion, TTree* Eheat, TTree* Cuts, string dstfile, string asciifile="",int modulation=200) {
  
  // Format DST au niveau zero (avant de passer a l'etude des coincidences)
  // run - nevt(samba) - flag_evttype - echal - eion - toffset - chi2flag
  
  // NB: chi2flag: pour l'instant c'est juste le chi2 de la chaleur!!
  // A ameliorer pour gerer le chi2 des ions en fct du type d'evt...
  
  if (!IsID(bolo)) { // Cas des GG..
    BuildDST_GeNTD(chain, bolo, Eion, Eheat, Cuts, dstfile, asciifile, modulation);
    return 0;
  }
  
  
  string datadir=DATAPATH;
  string macrodir=MACROPATH;
  Float_t sigmacut_heat=3; // variable differente de gSigmaCut_Heat! (moins strict que pr qplot)
  
  TFile* f = new TFile(dstfile.c_str(),"RECREATE");
  TTree* dst = new TTree("dsttree","EdwDST");
  char runname[9];
  Int_t evtnum=0, evtflag=0, toffset=0, chi2flag=0;
  Float_t eion=0, eheat=0;
  Int_t ionflags[6];
  for (int k=0; k<6; k++) ionflags[k]=0;
  ULong64_t date;
  dst->Branch("RunName",&runname,"RunName/C");
  dst->Branch("EventNum",&evtnum,"EventNum/I");
  dst->Branch("DateSec",&date,"DateSec/l");
  dst->Branch("EventFlag",&evtflag,"EventFlag/I");
  dst->Branch("Eheat",&eheat,"Eheat/F");
  dst->Branch("Eion",&eion,"Eion/F");
  dst->Branch("Toffset",&toffset,"Toffset/I");
  dst->Branch("Chi2Flag",&chi2flag,"Chi2Flag/I");
  dst->Branch("IonFlags",ionflags,"IonFlags[6]/I");
  
  Int_t nbevts = chain->GetEntries();
  char therun[9]; UInt_t theevtnum=0; ULong64_t thedate=0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("RunName",&therun);
  chain->SetBranchAddress("EventNum",&theevtnum);
  chain->SetBranchStatus("RunName",1);
  chain->SetBranchStatus("EventNum",1);
  chain->SetBranchAddress("DateSec",&thedate);
  chain->SetBranchStatus("DateSec",1);
  
  Float_t Ecol1=0, Ecol2=0, Ldbcol1, Ldbcol2;
  Float_t Evet1=0, Evet2=0, Ldbvet1, Ldbvet2;
  Float_t Egar1=0, Egar2=0, Ldbgar1, Ldbgar2;
  Float_t BinCol1=0, BinCol2=0, BinVet1=0, BinVet2=0, BinGar1=0, BinGar2=0;
  Float_t EsyncVet1=0, EsyncVet2=0, EsyncGar1=0, EsyncGar2=0;
  if (Eion->GetEntries() != nbevts) cout << "Error nb evts eion" << endl;
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
  Eion->SetBranchAddress("Evet1sync",&EsyncVet1); 
  Eion->SetBranchAddress("Evet2sync",&EsyncVet2);
  if (bolo.substr(0,2) == "ID") {
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),&BinGar1);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),1);
    chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),&BinGar2);
    chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar2")).c_str(),1);
    Eion->SetBranchAddress("Egar1",&Egar1); 
    Eion->SetBranchAddress("Egar2",&Egar2);
    Eion->SetBranchAddress("LdbGar1",&Ldbgar1); 
    Eion->SetBranchAddress("LdbGar2",&Ldbgar2);
    Eion->SetBranchAddress("Egar1sync",&EsyncGar1); 
    Eion->SetBranchAddress("Egar2sync",&EsyncGar2);
  }
  
  Float_t theEheat=0, Ldbheat=0, BinHeat=0;
  if (Eheat->GetEntries() != nbevts) cout << "Error nb evts eheat" << endl;
  chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"heat")).c_str(),&BinHeat);
  chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"heat")).c_str(),1);
  Eheat->SetBranchAddress("Eheat",&theEheat);
  Eheat->SetBranchAddress("LdbHeat",&Ldbheat); 
  
  Bool_t cutchi2heat=1, cutchi2col1=1, cutchi2col2=1, cutchi2vet1=1, cutchi2vet2=1,
  cutchi2gar1=1, cutchi2gar2=1, cutchi2ion=1;
  if (Cuts->GetEntries() != nbevts) cout << "Error nb evts cuts" << endl;
  Cuts->SetBranchAddress("CutChi2Heat",&cutchi2heat);
  Cuts->SetBranchAddress("CutChi2Col1",&cutchi2col1);
  Cuts->SetBranchAddress("CutChi2Col2",&cutchi2col2);
  Cuts->SetBranchAddress("CutChi2Vet1",&cutchi2vet1);
  Cuts->SetBranchAddress("CutChi2Vet2",&cutchi2vet2);
  if (!IsFID(bolo)) {
    Cuts->SetBranchAddress("CutChi2Gar1",&cutchi2gar1);
    Cuts->SetBranchAddress("CutChi2Gar2",&cutchi2gar2);
  }
  
  // 1er passage : on remplit un (gros) fichier ascii pour l'envoyer au code compile.
  string compile_entree=datadir+bolo+"_tmptoto.txt";
  string compile_sortie=datadir+bolo+"_tmptiti.txt";
  int nbchannels=4;
  if (bolo.substr(0,2) == "ID") nbchannels=6;
  ofstream entree(compile_entree.c_str(),ios::out);
  for (int i=0; i<nbevts; i++) { // nbevts
    chain->GetEntry(i);
    Eion->GetEntry(i);
    Eheat->GetEntry(i);
    if (theEheat >= sigmacut_heat*Ldbheat && theEheat > 0.05) {
      if (nbchannels == 6) {
        entree <<i<<" "<<Ecol1<<" "<<Ecol2<<" "<<Evet1<<" " <<Evet2<<" "<<Egar1<<" "<<Egar2<<" "<<
        Ldbcol1<<" "<<Ldbcol2<<" "<<Ldbvet1<<" " <<Ldbvet2<<" "<<Ldbgar1<<" "<<Ldbgar2<<" "<<
        (int)BinCol1<<" "<<(int)BinCol2<<" "<<(int)BinVet1<<" " <<(int)BinVet2<<" "<<
        (int)BinGar1<<" "<<(int)BinGar2<<" "<<
        EsyncVet1<<" "<<EsyncVet2<<" "<<EsyncGar1<<" "<<EsyncGar2<<endl;
      } else {
        entree<<i<<" "<<Ecol1<<" "<<Ecol2<<" "<<Evet1<<" " <<Evet2<<" "<<
        Ldbcol1<<" "<<Ldbcol2<<" "<<Ldbvet1<<" " <<Ldbvet2<<" "<<
        (int)BinCol1<<" "<<(int)BinCol2<<" "<<(int)BinVet1<<" " <<(int)BinVet2<<" "<<
        EsyncGar1<<" "<<EsyncGar2<<endl;
      }
    }
  }
  entree.close();
  system(("wc -l "+compile_entree).c_str());
  
  // Code compile
  //  system(("g++ -o "+macrodir+"run_evtclass.exe "+macrodir+"run_evtclass.C").c_str());
  system((macrodir+"run_evtclass.exe "+int2str(nbchannels)+" "+
          compile_entree+" "+compile_sortie).c_str());
  system(("wc -l "+compile_sortie).c_str());
  //  system(("rm -f "+macrodir+"run_evtclass.exe").c_str());
  
  // 2e passage : on lit l'output du code compile et on remplit le ntuple.
  ofstream asciiout;
  if (asciifile != "") {
    asciiout.open(asciifile.c_str(),ios::out);
    asciiout << "# Run Event Bolometer TimeOffset Chi2Flag EventClass Eheat Eion" << endl;
  }
  ifstream sortie(compile_sortie.c_str(),ios::in);
  string line; int nbsortie=0; int j=1;
  while (getline(sortie,line)) nbsortie++;
  sortie.close(); sortie.open(compile_sortie.c_str(),ios::in);
  int i_sortie,flag_sortie,toffset_sortie; float eion_sortie; int ionflags_sortie[6];
  sortie >> i_sortie >> flag_sortie >> toffset_sortie >> eion_sortie 
  >>ionflags_sortie[0]>>ionflags_sortie[1]>>ionflags_sortie[2]>>ionflags_sortie[3]>>ionflags_sortie[4]>>ionflags_sortie[5];
  for (int i=0; i<nbevts; i++) { // nbevts
    if (i > i_sortie && j < nbsortie) { // A verifier!!!!
      sortie >> i_sortie >> flag_sortie >> toffset_sortie >> eion_sortie
      >>ionflags_sortie[0]>>ionflags_sortie[1]>>ionflags_sortie[2]>>ionflags_sortie[3]>>ionflags_sortie[4]>>ionflags_sortie[5];
      j++;
      if (i > i_sortie) cout << "error isortie...?" << j<<" "<<i<<" "<<i_sortie<<endl;
    }
    chain->GetEntry(i);
    Eion->GetEntry(i);
    Eheat->GetEntry(i);
    Cuts->GetEntry(i);
    strcpy(runname,therun);
    evtnum=(Int_t)theevtnum;
    date=thedate;
    eheat=theEheat;
    toffset=(Int_t)((BinHeat-256)*modulation); // EN DUR A S'ASSURER... RUN10...?
                                               // Il faut gerer le pattern...: ca peut etre 100,200,400.
    if (eheat < sigmacut_heat*Ldbheat) {
      evtflag=0; // a modifier: checker aussi ion ici!
      for (int k=0; k<6; k++) ionflags[k]=0;
      eion=0;
    } else {
      if (i == i_sortie) {
        evtflag = flag_sortie;
        toffset = toffset_sortie;
        eion = eion_sortie;
        for (int k=0; k<6; k++) ionflags[k]=ionflags_sortie[k];
      } else {
        evtflag = 1;
        eion=0;
        for (int k=0; k<6; k++) ionflags[k]=0;
      }
    }
    if (evtflag <= 1) cutchi2ion=1;
    else if (evtflag == 2) cutchi2ion=(cutchi2col1 && cutchi2col2);
    else if (evtflag == 3) cutchi2ion=(cutchi2col1 && cutchi2col2 && cutchi2vet1 && cutchi2vet2);
    else if (evtflag == 4) cutchi2ion=(cutchi2col1 && cutchi2col2 && cutchi2gar1 && cutchi2gar2);
    else if (evtflag == 5) cutchi2ion=(cutchi2col1 && cutchi2col2 && cutchi2vet1 && cutchi2vet2 && 
                                       cutchi2gar1 && cutchi2gar2);
    chi2flag=(Int_t)(cutchi2heat && cutchi2ion);
    dst->Fill();
    if (asciifile != "" && evtflag >= 2) asciiout<<runname<<" "<<evtnum<<" "<<bolo<<" "<<toffset<<" "
      <<chi2flag<<" "<<evtflag<<" "<<eheat<<" "<<eion<<endl;
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  if (asciifile!= "") asciiout.close();
  f->cd(); dst->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);
  system(("rm -f "+compile_entree).c_str());
  system(("rm -f "+compile_sortie).c_str());
  // create a "simple" file for later purpose
  string simplentp=(dstfile.substr(0,dstfile.length()-5))+"_simple.root";
  system(("cp "+dstfile+" "+simplentp).c_str());
  
  return 0;
}

int BuildDST_GeNTD(TChain* chain, string bolo, TTree* Eion, TTree* Eheat, TTree* Cuts, string dstfile, string asciifile="",int modulation=200) {
  
  Float_t sigmacut_heat=3;
  Float_t sigmacut_ion=2;
  
  TFile* f = new TFile(dstfile.c_str(),"RECREATE");
  TTree* dst = new TTree("dsttree","EdwDST");
  char runname[9];
  Int_t evtnum=0, evtflag=0, toffset=0, chi2flag=0;
  Float_t eion=0, eheat=0;
  Int_t ionflags[6];
  ULong64_t date;
  dst->Branch("RunName",&runname,"RunName/C");
  dst->Branch("EventNum",&evtnum,"EventNum/I");
  dst->Branch("DateSec",&date,"DateSec/l");
  dst->Branch("EventFlag",&evtflag,"EventFlag/I");
  dst->Branch("Eheat",&eheat,"Eheat/F");
  dst->Branch("Eion",&eion,"Eion/F");
  dst->Branch("Toffset",&toffset,"Toffset/I");
  dst->Branch("Chi2Flag",&chi2flag,"Chi2Flag/I");
  dst->Branch("IonFlags",ionflags,"IonFlags[6]/I");
  
  Int_t nbevts = chain->GetEntries();
  char therun[9]; UInt_t theevtnum=0; ULong64_t thedate=0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("RunName",&therun);
  chain->SetBranchAddress("EventNum",&theevtnum);
  chain->SetBranchStatus("RunName",1);
  chain->SetBranchStatus("EventNum",1);
  chain->SetBranchAddress("DateSec",&thedate);
  chain->SetBranchStatus("DateSec",1);
  
  Float_t Ecol1=0, Egar1=0, BinCol1=0, BinGar1=0, LdbCol1=0, LdbGar1=0;
  if (Eion->GetEntries() != nbevts) cout << "Error nb evts eion" << endl;
  chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),&BinCol1);
  chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"col1")).c_str(),1);
  chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),&BinGar1);
  chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"gar1")).c_str(),1);
  Eion->SetBranchAddress("Ecol1",&Ecol1);
  Eion->SetBranchAddress("LdbCol1",&LdbCol1);
  Eion->SetBranchAddress("Egar1",&Egar1);
  Eion->SetBranchAddress("LdbGar1",&LdbGar1);
  
  Float_t theEheat=0, Ldbheat=0, BinHeat=0;
  if (Eheat->GetEntries() != nbevts) cout << "Error nb evts eheat" << endl;
  chain->SetBranchAddress(("WienerBin_"+ChannelName(bolo,"heat")).c_str(),&BinHeat);
  chain->SetBranchStatus(("WienerBin_"+ChannelName(bolo,"heat")).c_str(),1);
  Eheat->SetBranchAddress("Eheat",&theEheat);
  Eheat->SetBranchAddress("LdbHeat",&Ldbheat); 
  
  Bool_t cutchi2heat=1,cutchi2col1=1,cutchi2gar1=1,cutchi2ion=1;
  if (Cuts->GetEntries() != nbevts) cout << "Error nb evts cuts" << endl;
  Cuts->SetBranchAddress("CutChi2Heat",&cutchi2heat);
  Cuts->SetBranchAddress("CutChi2Col1",&cutchi2col1);
  Cuts->SetBranchAddress("CutChi2Gar1",&cutchi2gar1);
  
  ofstream asciiout;
  if (asciifile != "") {
    asciiout.open(asciifile.c_str(),ios::out);
    asciiout << "# Run Event Bolometer TimeOffset Chi2Flag EventClass Eheat Eion" << endl;
  }
  
  for (int i=0; i<nbevts; i++) { // nbevts
    chain->GetEntry(i);
    Eion->GetEntry(i);
    Eheat->GetEntry(i);
    Cuts->GetEntry(i);
    strcpy(runname,therun);
    evtnum=(Int_t)theevtnum;
    date=thedate;
    eheat=theEheat;
    toffset=(Int_t)((BinHeat-256)*modulation);
    if (eheat < sigmacut_heat*Ldbheat) {
      evtflag=0;
      eion=0;
      for (int k=0; k<6; k++) ionflags[k]=0;
      chi2flag=1;
      toffset=0;
    } else {
      if (Ecol1 < sigmacut_ion*LdbCol1 && Egar1 < sigmacut_ion*LdbGar1) {
        evtflag=1;
        eion=0;
        chi2flag=cutchi2heat;
        for (int k=0; k<6; k++) ionflags[k]=0;
      } else if ( Egar1 < sigmacut_ion*LdbGar1 ) {
        evtflag=2;
        eion=Ecol1;
        ionflags[0]=1;
        chi2flag=(cutchi2heat && cutchi2col1);
        toffset = BinCol1-4000;
        for (int k=1; k<6; k++) ionflags[k]=0;
      } else if ( Ecol1 < sigmacut_ion*LdbCol1 ){
        evtflag=4;
        eion=Egar1;
        for (int k=0; k<6; k++) ionflags[k]=0;
        ionflags[4]=1;
        chi2flag=(cutchi2heat && cutchi2gar1);
        toffset = BinGar1-4000;
      } else {
        evtflag=5;
        eion=Egar1+Ecol1;
        ionflags[0]=1;
        for (int k=1; k<6; k++) ionflags[k]=0;
        ionflags[4]=1;
        chi2flag=(cutchi2heat && cutchi2gar1 && cutchi2col1);
        toffset=BinCol1-4000; // on fait simple...
      }
    }
    dst->Fill();
    if (asciifile != "" && evtflag >= 2) asciiout<<runname<<" "<<evtnum<<" "<<bolo<<" "<<toffset<<" "
      <<chi2flag<<" "<<evtflag<<" "<<eheat<<" "<<eion<<endl;
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }
  
  if (asciifile!= "") asciiout.close();
  f->cd(); dst->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);
  // create a "simple" file for later purpose
  string simplentp=(dstfile.substr(0,dstfile.length()-5))+"_simple.root";
  system(("cp "+dstfile+" "+simplentp).c_str());
  
  return 0;
}
