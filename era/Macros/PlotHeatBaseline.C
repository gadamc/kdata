
#include "PlotIonBaselines.C"

int PlotHeatBaseline(string bolo, string baselinefile, bool continuous=0) {

  TCanvas* cc = new TCanvas("cc","baseline evol");

  ldbgraphe* ptsheat = MakeLdbGraph(baselinefile,continuous);
  Int_t nbheat = ptsheat->size;
  Double_t* t_heat = new Double_t[nbheat];
  Double_t* base_heat = new Double_t[nbheat];
  for (Int_t i=0; i<nbheat; i++) {
    t_heat[i]=(ptsheat->time)[i]; base_heat[i]=(ptsheat->ldb)[i];
  }
  TGraph* gr = new TGraph(nbheat,t_heat,base_heat);
  gr->SetTitle((bolo+" heat baseline").c_str());
  gr->SetLineColor(2);
  gr->GetXaxis()->SetTimeDisplay(1);
  gr->GetYaxis()->SetRangeUser(0,10);
  gr->GetYaxis()->SetTitle("FWHM [keV]");
  gr->Draw("AL");

}

int PlotDistriBaselines(string bolo, int p, string qualdir, Float_t* cutbaselines) {

  vector<string> thechannels;
  thechannels.push_back("heat");
  thechannels.push_back("col1"); thechannels.push_back("col2");
  thechannels.push_back("vet1"); thechannels.push_back("vet2");
  if (bolo.find("FID") == string::npos) {
    thechannels.push_back("gar1"); thechannels.push_back("gar2");
  }

  // Connerie de cint de merde:  pas de loop propre...
  // on doit demander p en argument!

  string thechannel=thechannels[p];
  string filename=qualdir+"/baseline_"+ChannelName(bolo,thechannel)+".txt";
  Float_t thecut=cutbaselines[p];
  string ll; long a,b; Double_t c; int nbpts=0;
  int nbpts_nocut=0;
  ifstream buf(filename.c_str(),ios::in);
  while (getline(buf,ll)) nbpts++;
  buf.close();
  ifstream buf2(filename.c_str(),ios::in);
  Double_t* basedistri = new Double_t[nbpts];
  for (int i=0; i<nbpts; i++) {
    buf2 >> a >> b >> c;
    basedistri[i]=c;
    if (c < thecut) nbpts_nocut++;
  }
  buf2.close();
  Double_t* basedistri_nocut = new Double_t[nbpts_nocut];
  int j_nocut=0;
  for (int i=0; i<nbpts; i++) {
    if (basedistri[i] < thecut && basedistri[i] > 0.01) {
      basedistri_nocut[j_nocut]=basedistri[i];
      j_nocut++;
    }
  }
  //    Double_t median_full = TMath::Median(nbpts,&basedistri[0]); // bugguee car
  // avec les pts a base=0!
  Double_t median_cut = TMath::Median(nbpts_nocut,&basedistri_nocut[0]);
  TCanvas* cc = new TCanvas(thechannel.c_str(),thechannel.c_str());
  TH1F* histo = new TH1F(("Baselines "+thechannel).c_str(),(thechannel+" baselines - "+bolo).c_str(),100,0.01,1.5*thecut);
  for (int i=0; i<nbpts; i++) histo->Fill(basedistri[i]);
  histo->GetXaxis()->SetTitle("FWHM baseline [keV]");
  gStyle->SetOptStat(0);
  histo->Draw();
  TLine* line = new TLine(thecut,0,thecut,histo->GetMaximum());
  line->SetLineColor(2);
  line->Draw("same");
  cout <<"Median baseline after cut for "<<thechannel<<" : "<<median_cut<<endl;

  delete[] basedistri; delete[] basedistri_nocut;

  return 1;
}
