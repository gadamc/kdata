
int SelectEvents(TTree* t, string channel, int nbselect, Float_t amplmin, Float_t amplmax, Float_t binmin, Float_t binmax, string* runs=NULL, int nbruns=0) {

  Int_t nbevts = t->GetEntries();
  Float_t bin, ampl; UInt_t jevt;
  char therun[9];
  t->SetBranchAddress(("RawBin_"+channel).c_str(),&bin);
  t->SetBranchAddress(("RawAmpl_"+channel).c_str(),&ampl);
  t->SetBranchAddress("RunName",&therun);
  t->SetBranchAddress("EventNum",&jevt);
  t->SetBranchStatus("*",0);
  t->SetBranchStatus(("RawBin_"+channel).c_str(),1);
  t->SetBranchStatus(("RawAmpl_"+channel).c_str(),1);
  t->SetBranchStatus("RunName",1);
  t->SetBranchStatus("EventNum",1);

  Int_t isel=0, compteur=0;
  while (isel <nbselect) {
    compteur++;
    Float_t r = gRandom->Rndm();
    Int_t p=(Int_t)(r*(Float_t)nbevts);
    t->GetEntry(p);
    if (ampl >= amplmin && ampl <= amplmax && bin >= binmin && bin <= binmax) {
      if (runs == NULL) {
	isel+=1;
	cout << therun << " " << jevt << " \""<<channel<<"\"";
	cout << " **** "<<ampl<<" "<<bin<<endl;
      } else {
	for (int p=0; p<nbruns; p++) {
	  if (!strcmp(therun,(runs[p]).c_str())) {
	    isel+=1;
	    cout << therun << " " << jevt << " \""<<channel<<"\"";
	    cout << " **** "<<ampl<<" "<<bin<<endl;
	  }
	}
      }
    }
  }

  t->SetBranchStatus("*",1);
  return 0;
}
