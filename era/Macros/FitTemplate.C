
#include <vector>

Double_t FitIon(Double_t* x, Double_t* par) {
  Float_t xx=x[0];
  Double_t f = 0;

  if (xx < par[0])
    f = (1-par[3])*par[1]*(1+TMath::Erf((xx-par[0])/par[4]))/2.0;
  else
    f = (par[1]*(1+TMath::Erf((xx-par[0])/par[4]))/2.0)
      *(TMath::Exp(-(xx-par[0])/par[2])-par[3]);
  
  return f;
}

Double_t FitChal(Double_t* x, Double_t* par) {
  Float_t xx=x[0];
  Double_t f = 0;
  Float_t offset = xx - par[0];

  if (xx > par[0])
    f = par[1] * (1-exp(-offset/par[2]))
      * (exp(-offset/par[3]) + par[4]*exp(-offset/par[5])
	 + par[6]*exp(-offset/par[7])) ;
  
  return f;
}

Int_t FitTemplate(string channel, string file, string outfile="None") {

  ifstream buf(file.c_str(),ios::in);
  string line;
  getline(buf,line);
  getline(buf,line);
  getline(buf,line);
  getline(buf,line);

  Float_t tmp; Int_t nb=0;
  while (buf >> tmp ) nb++;

  Float_t* data = new Float_t[nb];
  buf.close(); buf.open(file.c_str(),ios::in); Int_t i=0;
  getline(buf,line); getline(buf,line);
  getline(buf,line); getline(buf,line);
  while (buf >> tmp) {
    data[i]=tmp; 
    i++;
  }
  buf.close();

  TH1F* h = new TH1F("Template",channel.c_str(),nb,0,nb);
  Float_t average=0;
  for (Int_t i=0; i<200; i++) average+=data[i];
  average/=200.;
  for (Int_t i=0; i<nb; i++) h->SetBinContent(i,data[i]-average);   

  // Zoom
  Int_t nb_zoom = nb / 20;
  Float_t min = h->GetMinimum(), max = h->GetMaximum();
  Int_t peakbin = h->GetMaximumBin();
  if (TMath::Abs(min) > max) peakbin = h->GetMinimumBin();

  /*  Int_t zoom_start = peakbin - (Int_t)(Float_t(nb_zoom)/4);
  Int_t zoom_stop = peakbin + (Int_t)(3*Float_t(nb_zoom)/4);
  TH1F* hzoom = new TH1F("zoom","zoom",nb_zoom,0,nb_zoom);
  for (Int_t i=0; i<nb_zoom; i++) hzoom->SetBinContent(i,data[i+zoom_start]);
  */

  // Fit ion
  string::size_type loc = file.find("chaleur",0);
  if (loc == string::npos) {

  // remove pattern
    int length=200; // ATTNETION  
    vector<float> pattern(length,0);
    for (int p=0; p<10*length; p++) {
      int k=fmod(p,length);
      pattern[k]+=h->GetBinContent(p+1);
    }
    for (int p=0; p<length; p++) pattern[p]/=10;
    for (int p=0; p<nb; p++) {
      int k=fmod(p,length);
      h->SetBinContent(p+1,h->GetBinContent(p+1)-pattern[k]);
    }

    TF1* fit = new TF1("fit",FitIon,0,nb,5);
    fit->SetParameter(1,max);
    if (TMath::Abs(min) > max) fit->SetParameter(1,min);
    fit->SetParameter(3,TMath::Abs(h->GetBinContent(nb-2)/fit->GetParameter(1)));
    Int_t offset = 0; Float_t val_half = TMath::Abs(fit->GetParameter(1))*(0.5-fit->GetParameter(3));
    do {
      offset+=1;
    } while (TMath::Abs(h->GetBinContent(peakbin+offset)) >= val_half && peakbin+offset<nb );
    fit->SetParameter(2,offset*TMath::Log(2.0));
    offset = 0;
    do {
      offset-=1;
    } while (TMath::Abs(h->GetBinContent(peakbin+offset)) >= val_half && peakbin+offset > 0);
    fit->SetParameter(0,peakbin+offset);
    fit->SetParameter(4,1);
    cout << "Fit Guess: "<<endl;
    cout << "t0="<<fit->GetParameter(0)<<" - a0="<< fit->GetParameter(1)<<endl;
    cout <<" tfall="<< fit->GetParameter(2)<<" offset="<< fit->GetParameter(3)<<" - smooth="<<fit->GetParameter(4)<<endl;
    
  /*  TF1* fitzoom = new TF1("fitzoom",FitIon,0,nb_zoom,5);
  fitzoom->SetParameter(1,max);
  if (TMath::Abs(min) > max) fitzoom->SetParameter(1,min);
  fitzoom->FixParameter(3,0);
  offset = 0; 
  val_half = TMath::Abs(fitzoom->GetParameter(1))*(0.5-fitzoom->GetParameter(3));
  do {
    offset+=1;
  } while (TMath::Abs(hzoom->GetBinContent((Int_t)(Float_t(nb_zoom)/4)+offset)) >= val_half);
  fitzoom->SetParameter(2,offset*TMath::Log(2.0));
  offset = 0;
  do {
    offset-=1;
  } while (TMath::Abs(hzoom->GetBinContent((Int_t)(Float_t(nb_zoom)/4)+offset)) >= val_half);
  fitzoom->SetParameter(0,(Int_t)(Float_t(nb_zoom)/4)+offset);
  fitzoom->SetParameter(4,1);
  cout << "Fitzoom Guess: "<< fitzoom->GetParameter(0)<<" "<< fitzoom->GetParameter(1)<<" "<< fitzoom->GetParameter(2)<<" "<< fitzoom->GetParameter(3)<<" "<<fitzoom->GetParameter(4)<<endl;
  fitzoom->SetLineColor(2);*/
    fit->SetLineColor(4); fit->SetLineWidth(1);

  //  TCanvas* c = new TCanvas("c","c",700,800);
  //  c->Divide(1,2);
  //  c->cd(1);
    gPad->Clear();
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0002);
    gStyle->SetStatFontSize(0.03);
    h->SetMarkerStyle(7);
    h->Draw("P");
    h->Fit("fit","WM0"); fit->Draw("same");
  //  c->cd(2);
  //  hzoom->SetMarkerStyle(7);
  //  hzoom->Draw("P");
  //  hzoom->Fit("fitzoom","WM0"); fitzoom->Draw("same");

  } else { // chaleur

    TF1* fit = new TF1("fit",FitChal,0,nb,8);
    fit->SetParameter(0,peakbin-5);
    fit->SetParameter(1,1);
    fit->SetParameter(2,5);
    fit->SetParLimits(2,0.2,30);
    fit->SetParameter(3,30);
    fit->SetParLimits(3,5,100);
    fit->SetParameter(4,0.1);
    fit->SetParLimits(4,0,2);
    fit->SetParameter(5,150);
    fit->SetParLimits(5,50,700);
    fit->SetParameter(6,0.05);
    fit->SetParLimits(6,0,2);
    fit->SetParameter(7,400);
    fit->SetParLimits(7,150,2000);
    Float_t normfunc = fit->GetMaximum();
    fit->SetParameter(1,max/normfunc);
    if (TMath::Abs(min) > max) fit->SetParameter(1,min/normfunc);
    cout << "Fit Guess: "<< endl;
    cout << "t0="<<fit->GetParameter(0)<<" - ampl="<< fit->GetParameter(1)<<endl;
    cout << "trise="<< fit->GetParameter(2)<<" -tfal1="<< fit->GetParameter(3)<<endl;
    cout<<"eps="<< fit->GetParameter(4)<<" -tfal2"<< fit->GetParameter(5)<<endl;//" "<< fit->GetParameter(6)<<endl;
    
    fit->SetLineColor(2); fit->SetLineWidth(1);
    h->SetMarkerStyle(7);
    h->Draw("P");
    h->Fit("fit","WM0"); fit->Draw("same");
  }

  Float_t m1=fit->GetMinimum(), m2=fit->GetMaximum(); Float_t pt;
  if (TMath::Abs(m1) > m2) pt=fit->GetMinimumX();
  else pt=fit->GetMaximumX();
  if (outfile != "None") { // print the analytic template in ascii file
    ofstream outbuf(outfile.c_str(),ios::out);
    for (int i=0; i<nb; i++) outbuf << fit->Eval(i) << endl;
    outbuf.close();
  }

  return 0;
}
