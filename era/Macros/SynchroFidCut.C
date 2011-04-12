
int SynchroFidCut(string bolo, TTree* DSTRed, float vetopercentcut=2) {

  // September 2010
  // Plot the "synchro" channels vs full ionization to optimize the cut at high energy
  // and check there is no strong efficiency loss for a 2% vetopercentcut...

  TCanvas* cc = new TCanvas("sync","sync",1000,800);

  if (IsFID(bolo)) cc->Divide(2,1);
  else cc->Divide(2,2);

  cc->cd(1);
  DSTRed->Draw("Eion:Evet1sync","EventFlag==2 && Vflag>=0 && Evet1sync<50 && Eion<500");
  TLine* l1 = new TLine(0,0,500*vetopercentcut*0.01,500);
  TLine* l2 = new TLine(0,0,-500*vetopercentcut*0.01,500);
  l1->SetLineColor(kRed); l2->SetLineColor(kRed);
  l2->SetLineStyle(2);
  l1->Draw("same"); l2->Draw("same");
  cc->cd(2);
  DSTRed->Draw("Eion:Evet2sync","EventFlag==2 && Vflag>=0 && Evet2sync<50 && Eion<500");
  l1->Draw("same"); l2->Draw("same");
  if (!IsFID(bolo)) {
    cc->cd(3);
    DSTRed->Draw("Eion:Egar1sync","EventFlag==2 && Vflag>=0 && Egar1sync<50 && Eion<500");
    l1->Draw("same"); l2->Draw("same");
    cc->cd(4);
    DSTRed->Draw("Eion:Egar2sync","EventFlag==2 && Vflag>=0 && Egar2sync<50 && Eion<500");
    l1->Draw("same"); l2->Draw("same");
  }

  return 0;
}
