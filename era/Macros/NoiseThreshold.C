
int NoiseThreshold(string bolo, TTree* Eion, TTree* Eheat, TTree* Cuts, Float_t sigmacut_h, Float_t sigmacut_i) {

  // coupures : on met aussi le cut chi2 pour faire la chose!
  bool cutrun, cutbases, cutchi2;
  Cuts->SetBranchAddress("CutRun",&cutrun);
  Cuts->SetBranchAddress("CutBases",&cutbases);
  Cuts->SetBranchAddress("CutChi2",&cutchi2);

  Float_t eheat,ldbheat,efid,ldbfid;
  Eheat->SetBranchAddress("Eheat",&eheat);
  Eheat->SetBranchAddress("LdbHeat",&ldbheat);
  Eion->SetBranchAddress("Efid",&efid);
  Eion->SetBranchAddress("LdbFid",&ldbfid);

  vector<Float_t> enorm_i, enorm_h;
  for (int i=0; i<Eion->GetEntries(); i++) {
    Eion->GetEntry(i); Eheat->GetEntry(i); Cuts->GetEntry(i);
    if (cutrun && cutbases && cutchi2 && ldbfid !=0 && ldbheat != 0) {
      enorm_i.push_back(efid/ldbfid);
      enorm_h.push_back(eheat/ldbheat);
    }
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }

  int nb=enorm_i.size();
  Float_t* xx = new Float_t[nb];
  for (int i=0; i<nb; i++) xx[i]=enorm_i[i];
  Float_t* yy = new Float_t[nb];
  for (int i=0; i<nb; i++) yy[i]=enorm_h[i];
  //  TGraph* gr = new TGraph(nb,xx,yy);
  gStyle->SetPalette(1);
  TH2F* h2 = new TH2F("h2","noise",340,-2,100,500,-8,100);
  h2->GetXaxis()->SetTitle("Efid/FWHM");
  h2->GetYaxis()->SetTitle("Eheat/FWHM");
  for (int i=0; i<nb; i++) { if (xx[i] !=0 && yy[i] != 0) h2->Fill(xx[i],yy[i]); }
  h2->Draw("COLZ");
  //  gr->Draw("sameP");
  TLine* l1 = new TLine(sigmacut_i,sigmacut_h,sigmacut_i,0);
  TLine* l2 = new TLine(sigmacut_i,sigmacut_h,0,sigmacut_h);
  l1->SetLineWidth(2); l2->SetLineWidth(2);
  l1->Draw("same"); l2->Draw("same");


  return 0;
}
