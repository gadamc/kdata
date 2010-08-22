
int GetSigma356keV(string bolo, string basedir, Float_t cutldbion=2, Float_t cutldbheat=3) {

  TFile f((basedir+"/eion_"+bolo+".root").c_str(),"READ");
  TTree* Eion = (TTree*)f.Get("energytree");
  Int_t nbevts=Eion->GetEntries();
  Float_t efid, ldbfid; Bool_t vetocut;
  Eion->SetBranchAddress("Efid",&efid);
  Eion->SetBranchAddress("LdbFid",&ldbfid);
  Eion->SetBranchAddress("VetoCut",&vetocut);

  TFile f2((basedir+"/eheat_"+bolo+".root").c_str(),"READ");
  TTree* Echal = (TTree*)f2.Get("energytree");
  if (Echal->GetEntries() != nbevts) cout << "pbl tree size" << endl;
  Float_t eheat, ldbheat;
  Echal->SetBranchAddress("Eheat",&eheat);
  Echal->SetBranchAddress("LdbHeat",&ldbheat);
  
  TH1F* hion = new TH1F("hion","hion",50,320,370);
  hion->SetDirectory(0);
  TH1F* hchal = new TH1F("hchal","hchal",40,320,370);
  hchal->SetDirectory(0);
  for (int i=0; i<nbevts; i++) {
      Eion->GetEntry(i); Echal->GetEntry(i);
      if (ldbfid < cutldbion && vetocut == 1) hion->Fill(efid);
      if (ldbheat < cutldbheat && vetocut == 1) hchal->Fill(eheat); // choix fiduciel meme pr chal..
  }
  f2.Close();
  f.Close();

  TF1* fitfunc = new TF1("fitfunc","[0]*(1+TMath::Erf(([1]-x)/(sqrt(2)*[2])))+[3]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))");
  fitfunc->SetParLimits(2,1,10);
  fitfunc->SetParameter(0,hion->GetBinContent(2));
  fitfunc->SetParLimits(1,354,358);
  TF1* fit2 = new TF1("fit2","[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))",350,362);
  //  fit2->FixParameter(1,356.);
  fit2->SetParLimits(1,354,358);
  fit2->SetParLimits(2,1,10);
  fit2->SetLineColor(2);

  TCanvas* cion = new TCanvas("cion","cion");
  hion->Draw();
  hion->Fit("fitfunc","M","same");
  cout << "Sigma_Ion(356keV)="<<fitfunc->GetParameter(2)<<endl;
  cout << "Kappa = "<<pow(fitfunc->GetParameter(2)*2.355/356.,2)<<endl;
  hion->Fit("fit2","MR+","same");
  cout << "Sigma_Ion(356keV)="<<fit2->GetParameter(2)<<endl;
  cout << "Kappa = "<<pow(fit2->GetParameter(2)*2.355/356.,2)<<endl;

  fitfunc->SetParLimits(2,1,10);
  fitfunc->SetParameter(0,hchal->GetBinContent(2));
  TCanvas* cheat = new TCanvas("cheat","cheat");
  hchal->Draw();
  hchal->Fit("fitfunc","M","same");
  cout << "Sigma_Heat(356keV)="<<fitfunc->GetParameter(2)<<endl;
  cout << "Kappa = "<<pow(fitfunc->GetParameter(2)*2.355/356.,2)<<endl;
  hchal->Fit("fit2","MR+","same");
  cout << "Sigma_Heat(356keV)="<<fit2->GetParameter(2)<<endl;
  cout << "Kappa = "<<pow(fit2->GetParameter(2)*2.355/356.,2)<<endl;


  return 0;
}
