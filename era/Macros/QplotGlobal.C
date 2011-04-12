
int QplotGlobal(vector<string> qplotfiles, Float_t erec_min=0) {

  int nbpts=0, nn=0;
  for (int p=0; p<qplotfiles.size(); p++) {
    system(("wc -l "+qplotfiles[p]+" | awk '{print $1}' > tmptoto.txt").c_str());
    ifstream buf("tmptoto.txt",ios::in);
    while (buf >> nn) nbpts += nn;
    buf.close();
    system("rm -f tmptoto.txt");
  }
  cout << nbpts<<" events"<<endl;
  int nbtot=nbpts;

  Float_t* Erec = new Float_t[nbtot];
  Float_t* Q = new Float_t[nbtot];
  Float_t* Erec_NR = new Float_t[nbtot];

  int nn=0;
  Float_t aa,bb; Bool_t cc;
  for (int p=0; p<qplotfiles.size(); p++) {
    ifstream buf((qplotfiles[p]).c_str(),ios::in);
    if (buf) {
      while (buf >> aa >>bb >> cc) { // format fichier: E, Q, flag bande wimp
	Erec[nn]=aa; Q[nn]=bb;
	if (cc) Erec_NR[nn]=aa;
	else Erec_NR[nn]=-1;
	if (aa < erec_min) {
	  Erec[nn]=-1; Erec_NR[nn]=-1;
	}
	nn++;
      }
      buf.close();
    } else cout <<"pbl qplotfile "<<qplotfiles[p]<<endl;
  }
  if (nn != nbtot) cout << "erreur!" << endl;

  TCanvas* c1 = new TCanvas("qplot","qplot");
  TGraph* gr = new TGraph(nbtot,Erec,Q);
  gr->SetTitle("Qplot - All detectors");  
  gr->GetXaxis()->SetRangeUser(0,400);
  gr->GetYaxis()->SetRangeUser(0,2);
  gr->GetXaxis()->SetTitle("Recoil energy [keV]");
  gr->GetYaxis()->SetTitle("Ionization yield");
  gr->SetMarkerStyle(7);
  gr->Draw("AP");
  TGraph* gr2 = new TGraph(nbtot,Erec_NR,Q);
  gr2->SetMarkerStyle(8); gr2->SetMarkerColor(2);
  //  gr2->SetMarkerSize(0.5);
  gr2->Draw("sameP");

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

  return 0;
}
