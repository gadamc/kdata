
int IonCrossTalk(TTree* Eion, string bolo, string filename="", Float_t c1v1, Float_t v1c1, Float_t c2v2, Float_t v2c2, Float_t g1g2, Float_t g2g1 ) {
  // filename: a new Eion tree is built with CT corrections included
  
  Int_t nb = Eion->GetEntries();
  Float_t ecol1_raw=0, ecol2_raw=0, evet1_raw=0, evet2_raw=0, egar1_raw=0, egar2_raw=0; 
  if (bolo.substr(0,2) == "ID") {
    Eion->SetBranchAddress("Ecol1",&ecol1_raw); Eion->SetBranchAddress("Ecol2",&ecol2_raw);
    Eion->SetBranchAddress("Evet1",&evet1_raw); Eion->SetBranchAddress("Evet2",&evet2_raw);
    Eion->SetBranchAddress("Egar1",&egar1_raw); Eion->SetBranchAddress("Egar2",&egar2_raw);
  } else if (bolo.substr(0,3) == "FID") {
    Eion->SetBranchAddress("Ecol1",&ecol1_raw); Eion->SetBranchAddress("Ecol2",&ecol2_raw);
    Eion->SetBranchAddress("Evet1",&evet1_raw); Eion->SetBranchAddress("Evet2",&evet2_raw);
  } else {
    Eion->SetBranchAddress("Ecol1",&ecol1_raw); Eion->SetBranchAddress("Egar1",&egar1_raw);
  }
  Float_t* ecol1 = new Float_t[nb];  Float_t* ecol2 = new Float_t[nb];
  Float_t* evet1 = new Float_t[nb];  Float_t* evet2 = new Float_t[nb];
  Float_t* egar1 = new Float_t[nb];  Float_t* egar2 = new Float_t[nb];
  for (int i=0; i<nb; i++) {
    Eion->GetEntry(i);
    ecol1[i]=ecol1_raw; ecol2[i]=ecol1_raw;
    evet1[i]=evet1_raw; evet2[i]=evet2_raw;
    egar1[i]=egar1_raw; egar2[i]=egar2_raw;
  }

  if (bolo.substr(0,2) == "ID" || bolo.substr(0,3) == "FID") { // collect-veto CT
    Float_t ct_vet1col1, ct_col1vet1, ct_vet2col2, ct_col2vet2;

    TGraph* gr = new TGraph(nb,ecol1,evet1);
    gr->SetTitle(("Col1 vs Vet1 - "+bolo).c_str());
    gr->GetXaxis()->SetRangeUser(-50,400);
    gr->GetXaxis()->SetTitle("Col1");
    gr->GetYaxis()->SetRangeUser(-20,20);
    gr->GetYaxis()->SetTitle("Vet1");
    gr->SetMarkerStyle(6);
    gr->Draw("AP");
    Bool_t ok=0;
    TLine* line = new TLine(-50,0,400,0);
    line->SetLineColor(2); line->SetLineWidth(2);
    gPad->Update();
    while (ok == 0) {
      cout << "Enter value for CT: col1->vet1 " << endl; cin >> ct_col1vet1;
      line->SetY2(ct_col1vet1*400);
      line->Draw("same");
      gPad->Update();
      cout << "Ok? (0/1)" << endl; cin >> ok;
    }
    
    gr = new TGraph(nb,evet1,ecol1);
    gr->SetTitle(("Vet1 vs Col1 - "+bolo).c_str());
    gr->GetXaxis()->SetRangeUser(-50,400);
    gr->GetXaxis()->SetTitle("Vet1");
    gr->GetYaxis()->SetRangeUser(-20,20);
    gr->GetYaxis()->SetTitle("Col1");
    gr->SetMarkerStyle(6);
    gr->Draw("AP");
    Bool_t ok=0;
    TLine* line = new TLine(-50,0,400,0);
    line->SetLineColor(2); line->SetLineWidth(2);
    gPad->Update();
    while (ok == 0) {
      cout << "Enter value for CT: vet1->col1 " << endl; cin >> ct_vet1col1;
      line->SetY2(ct_vet1col1*400);
      line->Draw("same");
      gPad->Update();
      cout << "Ok? (0/1)" << endl; cin >> ok;
    }
    

  }



  return 0;
}
