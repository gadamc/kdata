
int QplotGlobal(vector<string> bolos, vector<string> macs, string basedir, string basedir2="", string basedir3="", bool display_coinc=1, Float_t erec_min=0) {

  vector<int> nbpts(bolos.size(),0); int nn=0;
  for (int p=0; p<bolos.size(); p++) {
    system(("wc -l "+basedir+"/Bckgd"+macs[p]+"/qplot_"+bolos[p]+".txt | awk '{print $1}' > tmptoto.txt").c_str());
    if (basedir2 != "") 
      system(("wc -l "+basedir2+"/qplot_"+bolos[p]+".txt  2> /dev/null | awk '{print $1}' >> tmptoto.txt").c_str());
    if (basedir3 != "") 
      system(("wc -l "+basedir3+"/qplot_"+bolos[p]+".txt  2> /dev/null | awk '{print $1}' >> tmptoto.txt").c_str());
    ifstream buf("tmptoto.txt",ios::in);
    nbpts[p]=0;
    while (buf >> nn) nbpts[p] += nn;
    buf.close();
    system("rm -f tmptoto.txt");
  }
  int nbtot=0;
  for (int p=0; p<nbpts.size(); p++) nbtot+=(nbpts[p]);

  cout << nbtot<<" events"<<endl;

  Float_t* Erec = new Float_t[nbtot];
  Float_t* Q = new Float_t[nbtot];
  Bool_t* IsNR_NoGamma = new Float_t[nbtot];
  Bool_t* IsNR_NoQuasiFid = new Float_t[nbtot];
  Long_t* Date = new Long_t[nbtot];

  nn=0;
  Float_t aa,bb; Bool_t cc,dd,ff; Long_t ee;
  for (int p=0; p<bolos.size(); p++) {
    ifstream buf((basedir+"/Bckgd"+macs[p]+"/qplot_"+bolos[p]+".txt").c_str(),ios::in);
    if (buf) {
      while (buf >> aa >>bb >> cc >> dd >> ee >> ff) {
	Erec[nn]=aa; Q[nn]=bb; IsNR_NoGamma[nn]=cc; IsNR_NoQuasiFid[nn]=dd;
	Date[nn]=ee;
	if (aa < erec_min) Erec[nn]=0;
	nn++;
      }
      buf.close();
    }
    if (basedir2!="") {
      ifstream buf2((basedir2+"/qplot_"+bolos[p]+".txt").c_str(),ios::in);
      if (buf2) {
	while (buf2 >> aa >>bb >> cc >> dd >> ee >> ff) {
	  Erec[nn]=aa; Q[nn]=bb; IsNR_NoGamma[nn]=cc; IsNR_NoQuasiFid[nn]=dd;
	  Date[nn]=ee;
	  if (aa < erec_min) Erec[nn]=0;
	  nn++;
	}
	buf2.close();
      }
    }
    if (basedir3!="") {
      ifstream buf3((basedir3+"/qplot_"+bolos[p]+".txt").c_str(),ios::in);
      if (buf3) {
	while (buf3 >> aa >>bb >> cc >> dd >> ee >> ff) {
	  Erec[nn]=aa; Q[nn]=bb; IsNR_NoGamma[nn]=cc; IsNR_NoQuasiFid[nn]=dd;
	  Date[nn]=ee;
	  if (aa < erec_min) Erec[nn]=0;
	  nn++;
	}
	buf3.close();
      }
    }
  }
  if (nn != nbtot) cout << "erreur!" << endl;

  Bool_t* IsCoinc = new Bool_t[nbtot];
  for (int i=0; i<nbtot; i++) IsCoinc[i]=0;
  for (int i=0; i<nbtot; i++) {
    long tzero=Date[i];
    for (int j=i+1; j<nbtot; j++) {
      if (Date[j] == tzero) { IsCoinc[i]=1; IsCoinc[j]=1;}
    }
    if (fmod(i,10000)==0) cout << i <<endl;
  }
  Int_t nbco=0; for (int i=0; i<nbtot; i++) {
    if (IsCoinc[i]==1) nbco++;
  }
  cout << "Coincidences : "<<nbco<<endl;
  
  Float_t* QG = new Float_t[nbtot];
  Float_t* QF = new Float_t[nbtot];
  Float_t* QN = new Float_t[nbtot]; // coincidences = neutrons
  for (int i=0; i<nbtot; i++) {
    if (Erec[i] > 20 && Erec[i]<25 && Q[i]<0.24 && Q[i]>0.15) Q[i]*=0.9;
    if (IsNR_NoGamma[i] == 1) QG[i]=Q[i];
    else QG[i]=-1;
    if (IsNR_NoQuasiFid[i] == 1) {
      QF[i]=Q[i];
    } else QF[i]=-1;
    if (Q[i] < 0.4 && Erec[i] > 28) QN[i]=Q[i]; // a mieux faire pour la suite... 
    // (avoir une liste d'evts en coincidences quelque part...)
    else QN[i]=-1;
    if (display_coinc==0 && Q[i] < 0.4 && Erec[i] > 28) { // IDEM..
      cout << "coinc (not displayed):" <<Q[i]<<" - "<<Erec[i]<<"keV"<<endl;
      Q[i]=-1; QF[i]=-1; QG[i]=-1; QN[i]=-1;
    }
    if (IsCoinc[i] == 1) Erec[i]=0;
  }

  TCanvas* c1 = new TCanvas("qplot","qplot");
  TGraph* gr = new TGraph(nbtot,Erec,Q);
  gr->SetTitle("Qplot - All detectors");  
  gr->GetXaxis()->SetRangeUser(0,400);
  gr->GetYaxis()->SetRangeUser(0,2);
  gr->GetXaxis()->SetTitle("Recoil energy [keV]");
  gr->GetYaxis()->SetTitle("Ionization yield");
  gr->SetMarkerStyle(7);
  gr->Draw("AP");

  TGraph* gr2 = new TGraph(nbtot,Erec,QG);
  gr2->SetMarkerStyle(8); gr2->SetMarkerColor(28);
  //  gr2->SetMarkerSize(0.5);
  gr2->Draw("sameP");
  TGraph* gr3 = new TGraph(nbtot,Erec,QF);
  gr3->SetMarkerStyle(8); gr3->SetMarkerColor(2);
  //  gr3->SetMarkerSize(0.5);
  gr3->Draw("sameP");
  TGraph* gr4 = new TGraph(nbtot,Erec,QN);
  gr4->SetMarkerStyle(8); gr4->SetMarkerColor(4);
  gr4->Draw("sameP");
  
  Float_t a=0.16, b=0.18; // NR band parameters
  TF1* qmean_nr = new TF1("qnr","[0]*pow(x,[1])",0,500);
  qmean_nr->SetParameter(0,a);
  qmean_nr->SetParameter(1,b);
  qmean_nr->Draw("same");
  qmean_nr->SetLineWidth(2); qmean_nr->SetLineColor(2);
  Float_t k2=1.645/2.355;
  Float_t c=0.035;
  Float_t kappa[]={0.0008,0.0007};
  Float_t kappa_ion=kappa[0], kappa_chal=kappa[1]; // increase of resolution at high E
  Float_t voltage=8., epsilon=3.;
  TF1* qup_nr = new TF1("qu_rn","[0]*pow(x,[1])+[2]*sqrt((1+[3])*(1+[3])*([4]*[4]+[7]*x*x)*[0]*pow(x,[1])*[0]*pow(x,[1])+(1+[3]*[0]*pow(x,[1]))*(1+[3]*[0]*pow(x,[1]))*([5]*[5]+[8]*x*x)+pow([6]*x,2))/x",0,500);
  qup_nr->SetParameter(0,a);
  qup_nr->SetParameter(1,b);
  qup_nr->SetParameter(2,k2); // syntaxe : cf fct Qplot...
  qup_nr->SetParameter(3,voltage/epsilon);
  qup_nr->SetParameter(4,1); // LDB heat
  qup_nr->SetParameter(5,0.87); // LDB ion
  qup_nr->SetParameter(6,c);
  qup_nr->SetParameter(7,kappa_chal);
  qup_nr->SetParameter(8,kappa_ion);
  TF1* qdown_nr = new TF1("qd_rn","[0]*pow(x,[1])-[2]*sqrt((1+[3])*(1+[3])*([4]*[4]+[7]*x*x)*[0]*pow(x,[1])*[0]*pow(x,[1])+(1+[3]*[0]*pow(x,[1]))*(1+[3]*[0]*pow(x,[1]))*([5]*[5]+[8]*x*x)+pow([6]*x,2))/x",0,500);
  qdown_nr->SetParameter(0,a);
  qdown_nr->SetParameter(1,b);
  qdown_nr->SetParameter(2,k2);
  qdown_nr->SetParameter(3,voltage/epsilon);
  qdown_nr->SetParameter(4,1); // LDB heat
  qdown_nr->SetParameter(5,0.87); // LDB ion
  qdown_nr->SetParameter(6,c);
  qdown_nr->SetParameter(7,kappa_chal);
  qdown_nr->SetParameter(8,kappa_ion);
  qup_nr->SetLineWidth(1);  qup_nr->SetLineColor(2);
  qdown_nr->SetLineWidth(1);  qdown_nr->SetLineColor(2);

  qup_nr->Draw("same");
  qdown_nr->Draw("same");

  delete[] Erec; delete[] Q; delete[] IsNR_NoGamma; delete[] IsNR_NoQuasiFid;
  delete[] Date; delete[] IsCoinc;
  return 0;
}
