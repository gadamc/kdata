
int BuildTimestamp(TChain* chain, string tstfile, string tstnew_dir="/sps/edelweis/torrento/tstnew/") {

  // lit les fichiers d'ana et fait un "friend" des fichiers standard avec

  Int_t nbevts = chain->GetEntries();
  chain->SetBranchStatus("*",0);
  char runname[9]; Int_t evtnum=0;
  chain->SetBranchAddress("RunName",&runname);
  chain->SetBranchAddress("EventNum",&evtnum);
  chain->SetBranchStatus("RunName",1);
  chain->SetBranchStatus("EventNum",1);

  TFile* f = new TFile(tstfile.c_str(),"RECREATE");
  TTree* tsttree = new TTree("tst_new","EdwTstNew");
  ULong64_t TimeStamp=0;
  tsttree->Branch("TimeStamp",&TimeStamp,"TimeStamp/l");

  char currentrun[9];
  ifstream current_tstfile; int a; Long64_t b;

  for (Int_t i=0; i<nbevts; i++) {
    chain->GetEntry(i);
    if (strcmp(runname,currentrun)) {
      current_tstfile.close();
      strcpy(currentrun,runname);
      cout << "new run "<<currentrun<<endl;
      string yoyo(currentrun);
      current_tstfile.open((tstnew_dir+"/"+yoyo+"_tstnew_v1").c_str(),ios::in);
      if (!current_tstfile) {
	cout << "Aie! no tstnew file" << endl;
	cout << currentrun<<" "<<evtnum<<" "<<i<<endl;
	return 0;
      }
      getline(current_tstfile,yoyo); getline(current_tstfile,yoyo);
      getline(current_tstfile,yoyo);
    }
    do {
      if (current_tstfile.eof()) {
	cout << "fin du tstred file.." << endl;
	cout << currentrun<<" "<<evtnum<<" "<<i<<endl;
	b=0;
	break;
      }
      current_tstfile >> a >> b;
    } while (a != evtnum);
    TimeStamp = (ULong64_t)(b);
    tsttree->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  f->cd(); tsttree->Write(0,TObject::kOverwrite);
  f->Close();
  chain->SetBranchStatus("*",1);
  return 0;
}
