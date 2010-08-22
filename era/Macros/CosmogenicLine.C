
int CosmogenicLine(string bolo, TTree* Eion, TTree* Cuts, TTree* Echal) {

  Float_t Ea=8.98, Eb=10.37;

  Float_t efid, etot; Bool_t vetocut, cutrun, cutbases, cutchi2;
  Float_t ldbfid, ldbheat, eheat;
  Eion->SetBranchAddress("Efid",&efid);
  Eion->SetBranchAddress("LdbFid",&ldbfid);
  Eion->SetBranchAddress("Etot",&etot);
  Eion->SetBranchAddress("VetoCut",&vetocut);
  Echal->SetBranchAddress("Eheat",&eheat);
  Echal->SetBranchAddress("LdbHeat",&ldbheat);
  Cuts->SetBranchAddress("CutRun",&cutrun);
  Cuts->SetBranchAddress("CutBases",&cutbases);
  Cuts->SetBranchAddress("CutChi2",&cutchi2);
  Int_t nb=Eion->GetEntries();
  if (Cuts->GetEntries() != nb) cout << "Error trees" << endl;

  TH1F* histofid = new TH1F("hfid","hfid",50,4,15);
  TH1F* histotot = new TH1F("htot","htot",50,4,15);
  TH1F* histobest = new TH1F("hbest","hbest",50,4,15);
  Float_t wi=0, wh=0, ldbi_prev=0, ldbh_prev=0;
  for (int i=0; i<nb; i++) {
    Eion->GetEntry(i); Cuts->GetEntry(i); Echal->GetEntry(i);
    if (cutrun && cutbases && cutchi2) {
      histotot->Fill(etot);
      if (vetocut) histofid->Fill(efid);
      if (ldbfid != ldbi_prev || ldbheat != ldbh_prev) {
	wi=ldbheat*ldbheat/(ldbheat*ldbheat+ldbfid*ldbfid);
      	wh=ldbfid*ldbfid/(ldbheat*ldbheat+ldbfid*ldbfid);
	ldbi_prev=ldbfid; ldbh_prev=ldbheat;
      }
      if (vetocut && efid > 4*ldbfid) histobest->Fill(wi*efid+wh*eheat);
    }
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }

  TF1* fitlines = new TF1("fitlines","[0]+[1]*exp(-((x-[2]*8.98)*(x-[2]*8.98)/(2*[3]*[3])))+[4]*exp(-((x-[2]*10.37)*(x-[2]*10.37)/(2*[3]*[3])))",6,13);
  fitlines->SetParameter(0,0);
  fitlines->SetParameter(2,1);
  fitlines->SetParLimits(2,0.5,1.05);
  fitlines->SetParameter(3,0.5);
  fitlines->SetParLimits(3,0.1,2);
  fitlines->SetLineColor(2);

  TCanvas* cosmo = new TCanvas("cosmo","cosmo");
  histotot->SetTitle(("Cosmogenic doublet - "+bolo).c_str());
  histotot->GetXaxis()->SetTitle("E[keV]");
  //  histotot->Draw();
  histofid->SetTitle(("Cosmogenic doublet - "+bolo).c_str());
  //  histofid->SetLineColor(2);
  //  histofid->Draw("same");
  histofid->Draw();
  histofid->Fit("fitlines","R","same");
  Float_t pic2_fid=fitlines->GetParameter(4);
  Float_t pic1_fid=fitlines->GetParameter(1);

  histobest->SetLineColor(4);
  //  histobest->Draw("same");
  //  histobest->Fit("fitlines","","same");

  fitlines->SetLineColor(1);
  //  histotot->Fit("fitlines","R+","same");
  Float_t pic2_tot=fitlines->GetParameter(4);
  Float_t pic1_tot=fitlines->GetParameter(1);

  Float_t emin=8., emax=12.;
  //  Float_t raie_tot=histotot->Integral(histotot->FindBin(emin),histotot->FindBin(emax));
  Float_t raie_fid=histofid->Integral(histofid->FindBin(emin),histofid->FindBin(emax));
  cout << "Counts (8<E<12 keV): "<<raie_fid<<endl;
  //  cout << "Integral estimate : "<<raie_tot<<" / "<<raie_fid<<" => fiducial fraction  "<<raie_fid/raie_tot<<endl;
  //  cout << "Fit estimate from ratio : " <<pic1_fid/pic1_tot << " (9keV) - " <<pic2_fid/pic2_tot<< " (10.4keV)" << endl;
  return 0;
}
