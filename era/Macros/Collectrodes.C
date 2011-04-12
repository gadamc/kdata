
int Collectrodes(string ntpfile, string bolo, Float_t sigmacut_delta, Float_t deltapercentcut=5) {
  
  if (!IsID(bolo)) cout << "Not an ID.. " <<endl;

  TFile f1(ntpfile.c_str(),"READ");
  Eion = (TTree*)f1.Get("energytree");
  
  Int_t nb = Eion->GetEntries();
  Float_t ecol1=0, ecol2=0, efid=0;
  Float_t ldbcol1=0, ldbcol2=0;
  Eion->SetBranchAddress("Ecol1",&ecol1); Eion->SetBranchAddress("Ecol2",&ecol2);
  Eion->SetBranchAddress("LdbCol1",&ldbcol1); Eion->SetBranchAddress("LdbCol2",&ldbcol2);
  Eion->SetBranchAddress("Efid",&efid);
  
  Float_t* DeltaE = new Float_t[nb];
  Float_t* Efid = new Float_t[nb];
  Float_t* DeltaE_cutfid = new Float_t[nb];

  Float_t ldbcol1_prev=0, ldbcol2_prev=0, Ldb_DeltaE=0;
  for (Int_t i=0; i<nb; i++) {
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
    Eion->GetEntry(i);

    if (ldbcol1 != ldbcol1_prev || ldbcol2 != ldbcol2_prev) {
      Ldb_DeltaE = sqrt(ldbcol2*ldbcol2+ldbcol1*ldbcol1);
      ldbcol1_prev=ldbcol1; ldbcol2_prev=ldbcol2;
    }
    
    DeltaE[i]=ecol2-ecol1; // NOT ABSOLUTE VALUE HERE
    Efid[i]=efid;
    if (TMath::Abs(DeltaE[i]) > Ldb_DeltaE*sigmacut_delta && TMath::Abs(DeltaE[i]) > 0.01*deltapercentcut*efid) DeltaE_cutfid[i]=-1000000;
    else DeltaE_cutfid[i]=DeltaE[i];
  }
  f1.Close();

  TCanvas* c = new TCanvas("collectrodes","collectrodes");
  TGraph* gr = new TGraph(nb,Efid,DeltaE);
  gr->SetTitle(("Collectrodes - "+bolo).c_str());
  gr->GetXaxis()->SetRangeUser(-20,400);
  gr->GetXaxis()->SetTitle("Efid");
  gr->GetYaxis()->SetRangeUser(-400,400);
  gr->GetYaxis()->SetTitle("E2-E1");
  gr->SetMarkerStyle(6);
  gr->Draw("AP");

  /* Old business
  Float_t kappa = 0.001;
  TF1* ff = new TF1("ff","sqrt([0]*[0]+[1]*x*x)",0,1000);
  ff->SetParameter(0,3*sigma1*sigma2/sqrt(sigma1*sigma1+sigma2*sigma2)); // 3*sigma_total
  ff->SetParameter(1,kappa);
  TF1* fg = new TF1("fg","ff*(-1)",0,1000);
  ff->SetLineColor(2); fg->SetLineColor(2);
  ff->Draw("same"); fg->Draw("same");
  gPad->Update();

  Bool_t ok=0; Float_t AA; Bool_t zoom=0;
  while (ok == 0) {
    cout << "cut2 = s0^2 + kappa*E^2" << endl;
    cout << "kappa="<<ff->GetParameter(1)<<" s0="<<ff->GetParameter(0)<<endl;
    cout << "Enter value for s0"<<endl; cin >> AA; ff->SetParameter(0,AA);
    fg->SetParameter(0,AA);
    cout << "Enter value for parabol kappa"<<endl; cin >> kappa; ff->SetParameter(1,kappa);
    fg->SetParameter(1,kappa);
    cout << "Zoom?(0/1)"<<endl; cin >> zoom;
    if (zoom) {
      gr->GetYaxis()->SetRangeUser(-3,3);
      gr->GetXaxis()->SetRangeUser(-1,50);
    } else {
      gr->GetYaxis()->SetRangeUser(-max/10.,max);
      gr->GetYaxis()->SetRangeUser(-max/10.,max/10.);
    }
    ff->Draw("same"); fg->Draw("same");
    gPad->Update();
    cout << "Ok? (0/1)" << endl; cin >> ok;
    } */

  TGraph* gr2 = new TGraph(nb,Efid,DeltaE_cutfid);
  gr2->SetMarkerStyle(6); gr2->SetMarkerColor(2);
  gr2->Draw("sameP");

  delete[] DeltaE; delete[] Efid; delete[] DeltaE_cutfid;
  return 0;
}
