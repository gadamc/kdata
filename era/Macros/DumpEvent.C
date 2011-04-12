
int DumpEvent(TTree* chain, string selectrun, UInt_t selectevt, Bool_t verbose=0) {
  
  UInt_t evtnum=0;
  char therun[9];
  chain->SetBranchAddress("RunName",&therun);
  chain->SetBranchAddress("EventNum",&evtnum);
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("RunName",1);
  chain->SetBranchStatus("EventNum",1);
  int isel=-1;
  for (UInt_t i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    if (selectevt == evtnum) {
      if (!strcmp(therun,selectrun.c_str())) {
	cout << "Chain entry "<<i<<endl;
	isel=i;
      }
    }
  }

  chain->SetBranchStatus("*",1);
  if ((isel != 0) && verbose) chain->Show(isel);
  return isel;
}
