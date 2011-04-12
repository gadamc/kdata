
int PulseBin(TTree* t, string channel, Float_t amplmin=0.05, Float_t amplmax=3, string ampltype="Raw", Int_t nbinsampl=100, Int_t nbins=100, string* runs=NULL, int nbruns=0) {

  Int_t nb = t->GetEntries();
  Float_t bin, ampl;
  char therun[9];
  t->SetBranchAddress((ampltype+"Bin_"+channel).c_str(),&bin);
  t->SetBranchAddress((ampltype+"Ampl_"+channel).c_str(),&ampl);
  t->SetBranchAddress("RunName",&therun);
  t->SetBranchStatus("*",0);
  t->SetBranchStatus((ampltype+"Bin_"+channel).c_str(),1);
  Int_t maxbin=0, minbin=10000;
  for (Int_t i=0; i<nb; i++) {
    t->GetEntry(i);
    if (bin >= maxbin) maxbin = bin+1;
    if (bin <= minbin && bin != 0) minbin = bin-1;
  }

  t->SetBranchStatus((ampltype+"Ampl_"+channel).c_str(),1);
  t->SetBranchStatus("RunName",1);

  const char* title=channel.c_str();
  TCanvas* cp = new TCanvas("pb","pb");
  gStyle->SetPalette(1);
  TH2F* hist = new TH2F(title,title,nbinsampl,amplmin,amplmax,nbins,minbin,maxbin);
  for (Int_t i=0; i<nb; i++) {
    t->GetEntry(i);
    if (runs == NULL)
      hist->Fill(ampl,bin);
    else {
      for (int p=0; p<nbruns; p++) {
	if (!strcmp(therun,(runs[p]).c_str()))
	  hist->Fill(ampl,bin);
      }
    }
  }
  hist->GetXaxis()->SetTitle("Amplitude");
  hist->GetYaxis()->SetTitle("Pulse bin");
  hist->Draw("COLZ");
  t->SetBranchStatus("*",1);

  return 0;
}
