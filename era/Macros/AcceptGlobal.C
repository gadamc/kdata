
#include <ctime>

int AcceptGlobal(vector<string> exposurefiles, vector<string> acceptancefiles) {
  // basedir2/3 = slt pour l'exposition pas pour l'acceptance pour l'instant

  // Masse fiducielle des bolos: en dur pr l'instant, pour IDs 400g.
  Float_t mass=0.16;
  // Idem l'efficacite globale (cut chi2)
  Float_t chi2_efficiency=0.97;

  // Jours d'integration: en dur aussi.
  // A changer en fct des besoins!
  struct tm start_time, stop_time;
  (start_time.tm_year)=(2008-1900); // ! faire -1900
  (start_time.tm_mon)=6; // ! 0-11
  (start_time.tm_mday)=0;
  (start_time.tm_hour)=0;
  (start_time.tm_min)=0;
  (start_time.tm_sec)=0;
  (stop_time.tm_year)=(2010-1900); // ! faire -1900
  (stop_time.tm_mon)=6; // ! 0-11
  (stop_time.tm_mday)=0;
  (stop_time.tm_hour)=0;
  (stop_time.tm_min)=0;
  (stop_time.tm_sec)=0;

  long start_ts=mktime(&start_time);
  long stop_ts=mktime(&stop_time);

  long bin_ts=24*3600;
  int nbins=1+((stop_ts-start_ts)/bin_ts);
  Double_t* Time = new Double_t[nbins];
  for (int j=0; j<nbins; j++) Time[j]=(Double_t)(start_ts+j*bin_ts);
  Double_t* Expo_raw = new Double_t[nbins];
  Double_t* Expo_cut = new Double_t[nbins];
  int nbfiles=exposurefiles.size();
  vector<Float_t> exposure_per_file(nbfiles,0);

  // puis remplissage exposition bining=jour par jour a partir des period_list_..
  // (en affectant chaque periode d'un bolo a un jour unique)
  // au passage kg.j total pour chaque bolo
  Double_t a,b; Bool_t c;
  Double_t dayunit=3600.*24.;
  for (int p=0; p<nbfiles; p++) {
    ifstream buf((exposurefiles[p]).c_str(),ios::in);
    if (buf) {
      while (buf >> a >> b >> c) {
	if (a < start_ts || b > stop_ts) cout << "Time bounds are too strict" << endl;
	int j_bin = (a-(Double_t)start_ts)/bin_ts;
	Double_t expo=chi2_efficiency*mass*(b-a)/(dayunit);
	Expo_raw[j_bin] += expo;
	if (c) {
	  Expo_cut[j_bin] += expo;
	  exposure_per_file[p] += expo;
	}
      }
      buf.close();
    } else cout << "not found "<<exposurefiles[p]<<endl;
  }
  for (int j=1; j<nbins; j++) {
    Expo_cut[j] += Expo_cut[j-1];
    Expo_raw[j] += Expo_raw[j-1];
  }

  TCanvas* c1 = new TCanvas("exposure","exposure");
  TGraph* gr1 = new TGraph(nbins,Time,Expo_raw);
  TGraph* gr2 = new TGraph(nbins,Time,Expo_cut);
  gr1->SetTitle("Cumulative exposure - All detectors");
  gr1->GetXaxis()->SetTimeDisplay(1);
  gr1->GetYaxis()->SetTitle("Exposure (kg.days)");
  gr1->Draw("AL");
  gr2->GetXaxis()->SetTimeDisplay(1);
  gr2->SetLineColor(2);
  gr2->Draw("sameL");
  TLegend* leg = new TLegend(0.15,0.75,0.45,0.85);
  leg->SetFillColor(0);
  leg->AddEntry(gr1,"Raw exposure","L");
  leg->AddEntry(gr2,"After period cuts","L");
  leg->Draw();

  // Acceptance
  vector<Double_t> toto;
  if (acceptancefiles.size() != nbfiles) 
    cout << "warning not same nb of accept and expo files"<<endl;
  ifstream buf0((acceptancefiles[0]).c_str(),ios::in);
  while (buf0 >> a >> b) toto.push_back(a);
  buf0.close();
  int nbins_acc=toto.size();
  Double_t* Energy = new Double_t[nbins_acc];
  for (int j=0; j<nbins_acc; j++) Energy[j]=toto[j];
  Double_t* Accept = new Double_t[nbins_acc];
  for (int j=0; j<nbins_acc; j++) Accept[j]=0;
  for (int p=0; p<nbfiles; p++) {
    ifstream buf((acceptancefiles[p]).c_str(),ios::in);
    if (!buf) cout << "error opening file "<<acceptancefiles[p]<<endl;
    int j=0;
    while (buf >> a >> b) {
      if ((a - Energy[j]) > 0.01) cout << "Error energy bin" << endl;
      if ( b<0 || b>1) cout << "Error acceptance read" << endl;
      Accept[j]+=(Double_t)(b*exposure_per_file[p]);
      j++;
    }
    if (j != nbins_acc) cout << "Error energy bins" <<endl;
    buf.close();
  }

  for (int k=0; k<nbins_acc; k++) Accept[k]/=(Accept[nbins_acc-1]);

  TCanvas* c2 = new TCanvas("acceptance","acceptance");
  TGraph* gr3 = new TGraph(nbins_acc,Energy,Accept);
  gr3->SetTitle("Acceptance - All detectors");
  gr3->GetXaxis()->SetTitle("Er [keV]");
  gr3->GetYaxis()->SetTitle("Relative acceptance");
  gr3->Draw("AL");

  cout << "Total exposure: " << Expo_cut[nbins-1]<<endl;
  int j20=0,j50=0; // j20 = bin of 20 keV, j50=bin of 50% acceptance
  while (Energy[j20] < 20) j20++;
  while (Accept[j50] < 0.5) j50++;
  cout << "Acceptance at 20 keV: "<<Accept[j20]<<" - 50% acceptance at: " <<Energy[j50]<<endl;

  // a la main...
  ofstream ff("accept_global.txt",ios::out);
  for (int i=0; i<nbins_acc; i++) ff << Energy[i]<<" "<<Accept[i]<<endl;
  ff.close();

  //  delete[] Time; delete[] Expo_raw; delete[] Expo_cut;
  //  delete[] Energy; delete[] Accept;
  return 0;
}
