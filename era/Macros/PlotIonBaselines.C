
#include <vector>

class ldbgraphe {
public:
  ldbgraphe() {vector<Double_t> dum(0); time=dum; ldb=dum;};
  ~ldbgraphe() {time.clear(); ldb.clear();};
  vector<Double_t> time;
  vector<Double_t> ldb;
  UInt_t size;
};

ldbgraphe* MakeLdbGraph(string ldbfile, bool continuous=0) {

  ifstream ldbstr(ldbfile.c_str(),ios::in);
  ldbgraphe* points = new ldbgraphe();
  ULong64_t ti,tf,tf_last; Float_t base;
  tf_last=0;
  ULong64_t timelag_lim=100;
  if (continuous) timelag_lim=24*3600; // just plot 1-day interruptions
  while (ldbstr >> ti >> tf >> base) {
    if (ti > tf_last+timelag_lim && tf_last !=0) {
      (points->time).push_back((Double_t)tf_last);
      (points->time).push_back((Double_t)ti);
      (points->ldb).push_back(0);
      (points->ldb).push_back(0);
    }
    if (tf_last == 0) {
      (points->time).push_back((Double_t)ti);
      (points->ldb).push_back(0);
    }
    (points->time).push_back((Double_t)ti);
    (points->time).push_back((Double_t)tf);
    (points->ldb).push_back(base);
    (points->ldb).push_back(base);
    tf_last = tf;
  }

  points->size = (points->ldb).size();
  ldbstr.close();
  return points;
}

int PlotIonBaselines(string bolo, string qualdir, string col1, string col2="", string vet1="", string vet2="", string gar1="", string gar2="", bool continuous=0) {

  gStyle->SetFillColor(0);
  TCanvas* cc = new TCanvas("cc","Baseline evolution");
  ldbgraphe* ptscol1 = MakeLdbGraph(qualdir+"/baseline_"+col1+".txt",continuous);
  Int_t nbcol1 = ptscol1->size;
  Double_t* t_col1 = new Double_t[nbcol1];
  Double_t* base_col1 = new Double_t[nbcol1];
  for (Int_t i=0; i<nbcol1; i++) {
    t_col1[i]=(ptscol1->time)[i]; base_col1[i]=(ptscol1->ldb)[i];
  }
  TGraph* grcol1 = new TGraph(nbcol1,t_col1,base_col1);
  grcol1->SetTitle((bolo+" ionization baselines").c_str());
  grcol1->SetLineColor(2);
  grcol1->GetXaxis()->SetTimeDisplay(1);
  grcol1->GetYaxis()->SetRangeUser(0,25);
  grcol1->GetYaxis()->SetTitle("FWHM [keV]");
  grcol1->Draw("AL");
  
  TLegend* leg = new TLegend(0.7,0.7,0.95,0.95);
  leg->AddEntry(grcol1,"Collectrode 1","L");

  if (col2 != "") {
    ldbgraphe* ptscol2 = MakeLdbGraph(qualdir+"/baseline_"+col2+".txt",continuous);
    Int_t nbcol2 = ptscol2->size;
    Double_t* t_col2 = new Double_t[nbcol2];
    Double_t* base_col2 = new Double_t[nbcol2];
    for (Int_t i=0; i<nbcol2; i++) {
      t_col2[i]=(ptscol2->time)[i]; base_col2[i]=(ptscol2->ldb)[i];
    }
    TGraph* grcol2 = new TGraph(nbcol2,t_col2,base_col2);
    grcol2->SetTitle(bolo.c_str());
    grcol2->SetLineColor(4);
    grcol2->GetXaxis()->SetTimeDisplay(1);
    grcol2->Draw("sameL");
    leg->AddEntry(grcol2,"Collectrode 2","L");
  }

  if (vet1 != "") {
    ldbgraphe* ptsvet1 = MakeLdbGraph(qualdir+"/baseline_"+vet1+".txt",continuous);
    Int_t nbvet1 = ptsvet1->size;
    Double_t* t_vet1 = new Double_t[nbvet1];
    Double_t* base_vet1 = new Double_t[nbvet1];
    for (Int_t i=0; i<nbvet1; i++) {
      t_vet1[i]=(ptsvet1->time)[i]; base_vet1[i]=(ptsvet1->ldb)[i];
    }
    TGraph* grvet1 = new TGraph(nbvet1,t_vet1,base_vet1);
    grvet1->SetTitle(bolo.c_str());
    grvet1->SetLineColor(2); grvet1->SetLineStyle(2);
    grvet1->GetXaxis()->SetTimeDisplay(1);
    grvet1->Draw("sameL");
    leg->AddEntry(grvet1,"Veto 1","L");
  }

  if (vet2 != "") {
    ldbgraphe* ptsvet2 = MakeLdbGraph(qualdir+"/baseline_"+vet2+".txt",continuous);
    Int_t nbvet2 = ptsvet2->size;
    Double_t* t_vet2 = new Double_t[nbvet2];
    Double_t* base_vet2 = new Double_t[nbvet2];
    for (Int_t i=0; i<nbvet2; i++) {
      t_vet2[i]=(ptsvet2->time)[i]; base_vet2[i]=(ptsvet2->ldb)[i];
    }
    TGraph* grvet2 = new TGraph(nbvet2,t_vet2,base_vet2);
    grvet2->SetTitle(bolo.c_str());
    grvet2->SetLineColor(4); grvet2->SetLineStyle(2);
    grvet2->GetXaxis()->SetTimeDisplay(1);
    grvet2->Draw("sameL");
    leg->AddEntry(grvet2,"Veto 2","L");
  }

  if (gar1 != "") {
    ldbgraphe* ptsgar1 = MakeLdbGraph(qualdir+"/baseline_"+gar1+".txt",continuous);
    Int_t nbgar1 = ptsgar1->size;
    Double_t* t_gar1 = new Double_t[nbgar1];
    Double_t* base_gar1 = new Double_t[nbgar1];
    for (Int_t i=0; i<nbgar1; i++) {
      t_gar1[i]=(ptsgar1->time)[i]; base_gar1[i]=(ptsgar1->ldb)[i];
    }
    TGraph* grgar1 = new TGraph(nbgar1,t_gar1,base_gar1);
    grgar1->SetTitle(bolo.c_str());
    grgar1->GetXaxis()->SetTimeDisplay(1);
    grgar1->Draw("sameL");
    leg->AddEntry(grgar1,"Guard 1","L");
  }

  if (gar2 != "") {
    ldbgraphe* ptsgar2 = MakeLdbGraph(qualdir+"/baseline_"+gar2+".txt",continuous);
    Int_t nbgar2 = ptsgar2->size;
    Double_t* t_gar2 = new Double_t[nbgar2];
    Double_t* base_gar2 = new Double_t[nbgar2];
    for (Int_t i=0; i<nbgar2; i++) {
      t_gar2[i]=(ptsgar2->time)[i]; base_gar2[i]=(ptsgar2->ldb)[i];
    }
    TGraph* grgar2 = new TGraph(nbgar2,t_gar2,base_gar2);
    grgar2->SetTitle(bolo.c_str());
    grgar2->SetLineStyle(5);
    grgar2->GetXaxis()->SetTimeDisplay(1);
    grgar2->Draw("sameL");
    leg->AddEntry(grgar2,"Guard 2","L");
  }
  leg->Draw();

  return 0;
}
