
// Atension doublon avec PlotEvent.C
string int2str(Int_t aNum,Int_t aWidth=2) {
  std::ostringstream lStream; 
  lStream.width(aWidth); 
  lStream.fill('0'); 
  lStream << aNum;
  return lStream.str();
}

int DeltaPlot(TChain* chain, string bolo, TTree* Eion, TTree* Echal, TTree* Cuts, Float_t sigmacut_vet, Float_t sigmacut_ion, Float_t sigmacut_chal) {

  Int_t nbevts=Eion->GetEntries();
  if (Cuts->GetEntries() != nbevts) cout <<"pbl taille ntp" << endl;
  Bool_t isfid=0;
  if (bolo.find("FID") != string::npos) isfid=1;

  Float_t ecol1,ecol2,ldbcol1,ldbcol2,efid,ldbfid;
  Eion->SetBranchAddress("Ecol1",&ecol1);
  Eion->SetBranchAddress("Ecol2",&ecol2);
  Eion->SetBranchAddress("Efid",&efid);
  Eion->SetBranchAddress("LdbCol1",&ldbcol1);
  Eion->SetBranchAddress("LdbCol2",&ldbcol2);
  Eion->SetBranchAddress("LdbFid",&ldbfid);
  Bool_t cutbases,cutrun,cutchi2;
  Cuts->SetBranchAddress("CutBases",&cutbases);
  Cuts->SetBranchAddress("CutRun",&cutrun);
  Cuts->SetBranchAddress("CutChi2",&cutchi2);
  Float_t eheat,ldbheat;
  Echal->SetBranchAddress("Eheat",&eheat);
  Echal->SetBranchAddress("LdbHeat",&ldbheat);


  // AATTENTION : SEULEMENT ID COMPLET!!!!! A generaliser...
  Float_t evet1,evet2,egar1,egar2,ldbvet1,ldbvet2,ldbgar1,ldbgar2;
  Eion->SetBranchAddress("Evet1",&evet1);
  Eion->SetBranchAddress("Evet2",&evet2);
  Eion->SetBranchAddress("LdbVet1",&ldbvet1);
  Eion->SetBranchAddress("LdbVet2",&ldbvet2);
  if (!isfid) {
    Eion->SetBranchAddress("Egar1",&egar1);
    Eion->SetBranchAddress("Egar2",&egar2);
    Eion->SetBranchAddress("LdbGar1",&ldbgar1);
    Eion->SetBranchAddress("LdbGar2",&ldbgar2);
  }

  Float_t* Energy = new Float_t[nbevts];
  Float_t* Delta = new Float_t[nbevts];
  Float_t* Sigma = new Float_t[nbevts];

  Float_t Efidmin1=5, Efidmax1=15;
  Float_t Efidmin2=15, Efidmax2=40;
  Float_t* x_DeltaSigma1 = new Float_t[nbevts];
  Float_t* y_DeltaSigma1 = new Float_t[nbevts];
  Float_t* x_DeltaSigma2 = new Float_t[nbevts];
  Float_t* y_DeltaSigma2 = new Float_t[nbevts];


  for (int i=0; i<nbevts; i++) {
    Cuts->GetEntry(i); Eion->GetEntry(i); Echal->GetEntry(i);

    if (cutbases && cutrun && cutchi2 && ecol1 && ecol2 &&
	efid > sigmacut_ion*ldbfid && eheat > sigmacut_chal*ldbheat) {
      Energy[i]=efid;
      Delta[i]=TMath::Abs(ecol2-ecol1)/(ecol2+ecol1);

      Sigma[i]=0;
      if (evet1 > sigmacut_vet*ldbvet1) Sigma[i]+=evet1/2;
      if (evet2 > sigmacut_vet*ldbvet2) Sigma[i]+=evet2/2;
      if (!isfid) {
	if (egar1 > sigmacut_vet*ldbgar1) Sigma[i]+=egar1/2;
	if (egar2 > sigmacut_vet*ldbgar2) Sigma[i]+=egar2/2;
      }
      // Estimateur (2) possible
      //      Sigma[i] = (evet1/ldbvet1)+(evet2/ldbvet2)+(egar1/ldbgar1)+(egar2/ldbgar2);
      Sigma[i] /= efid; // Estimateur (1bis)

    } else {
      Energy[i]=0; Delta[i]=0; Sigma[i]=0;
    }

    // Plots DeltaSigma
    if (Energy[i] > Efidmin1 && Energy[i] < Efidmax1) {
      x_DeltaSigma1[i]=Delta[i]; y_DeltaSigma1[i]=Sigma[i];
      x_DeltaSigma2[i]=0; y_DeltaSigma2[i]=0;
    } else if (Energy[i] > Efidmin2 && Energy[i] < Efidmax2) {
      x_DeltaSigma2[i]=Delta[i]; y_DeltaSigma2[i]=Sigma[i];
      x_DeltaSigma1[i]=0; y_DeltaSigma1[i]=0;
    } else {
      x_DeltaSigma1[i]=0; y_DeltaSigma1[i]=0;
      x_DeltaSigma2[i]=0; y_DeltaSigma2[i]=0;
    }

    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }

  TCanvas* cdelta = new TCanvas("cd","delta plot");
  TGraph* gr = new TGraph(nbevts,Energy,Delta);
  gr->SetTitle(("DeltaPlot - "+bolo).c_str());  
  gr->GetXaxis()->SetRangeUser(0,400);
  gr->GetYaxis()->SetRangeUser(0,1);
  gr->GetXaxis()->SetTitle("Efid [keV]");
  gr->GetYaxis()->SetTitle("Delta");
  gr->SetMarkerStyle(6);
  //  TLine* ll = new TLine(0,flim,400,flim);
  gr->Draw("AP");
  //  ll->SetLineWidth(2);
  //  ll->Draw("same");
  //  TGraph* grc = new TGraph(nbevts,Energy,Delta_cut);
  //  grc->SetMarkerStyle(6); grc->SetMarkerColor(2);
  //  grc->Draw("sameP");

  TCanvas* csigma = new TCanvas("cs","sigma plot");
  TGraph* grs = new TGraph(nbevts,Energy,Sigma);
  grs->SetTitle(("SigmaPlot - "+bolo).c_str());  
  grs->GetXaxis()->SetRangeUser(0,400);
  grs->GetYaxis()->SetRangeUser(0,5);
  grs->GetXaxis()->SetTitle("Efid [keV]");
  grs->GetYaxis()->SetTitle("Sigma");
  grs->SetMarkerStyle(6);
  //  TLine* ls = new TLine(0,flim,400,flim);
  grs->Draw("AP");
  //  ls->SetLineWidth(2);
  //  ls->Draw("same");

  TCanvas* cds1 = new TCanvas("cds1","low E delta-sigma plot");
  TGraph* grds1 = new TGraph(nbevts,x_DeltaSigma1,y_DeltaSigma1);
  grds1->SetTitle(("Delta vs Sigma - "+bolo+" - "+int2str(Efidmin1)+"<E/keV<"+int2str(Efidmax1)).c_str());  
  grds1->GetXaxis()->SetRangeUser(0,1);
  grds1->GetYaxis()->SetRangeUser(0,5);
  grds1->GetXaxis()->SetTitle("Delta");
  grds1->GetYaxis()->SetTitle("Sigma");
  grds1->SetMarkerStyle(6);
  //  TLine* l1 = new TLine(0,flim,flim,flim);
  //  TLine* l2 = new TLine(flim,0,flim,flim);
  grds1->Draw("AP");
  //  l1->SetLineWidth(2); l2->SetLineWidth(2);
  //  l1->Draw("same"); l2->Draw("same");

  TCanvas* cds2 = new TCanvas("cds2","high E delta-sigma plot");
  TGraph* grds2= new TGraph(nbevts,x_DeltaSigma2,y_DeltaSigma2);
  grds2->SetTitle(("Delta vs Sigma - "+bolo+" - "+int2str(Efidmin2)+"<E/keV<"+int2str(Efidmax2)).c_str());  
  grds2->GetXaxis()->SetRangeUser(0,1);
  grds2->GetYaxis()->SetRangeUser(0,5);
  grds2->GetXaxis()->SetTitle("Delta");
  grds2->GetYaxis()->SetTitle("Sigma");
  grds2->SetMarkerStyle(6);
  grds2->Draw("AP");

  delete[] Energy; delete[] Delta; delete[] Sigma;
  delete[] x_DeltaSigma1; delete[] y_DeltaSigma1;
  delete[] x_DeltaSigma2; delete[] y_DeltaSigma2;
  return 0;
}
