
#include "DetectTrig.C"
#include "ChannelType.C"
//#include "BuildSubPeriodFile.C"
#include <time.h>

int BaselineEvol(TChain* chain, string channel, string ntpfile, string periodfile, string outputfile="",int cuttrigger=-1,Bool_t wait=1) {
  // si cuttrigger>=0: ne prend pas les evts dt le bittrigger est 1 pour le
  // detecteur samba "cuttrigger" - discutable, a voir..

  // si "wait" fait aussi les plots

  if (channel == "") {
    cout << "Wrong channel!" << endl; return 0;
  }

  ofstream output;
  if (outputfile != "") output.open(outputfile.c_str(),ios::out);

  // First get correct channel on the ntp!
  ULong64_t date; Int_t bittrigger1, bittrigger2;
  Float_t energy;
  chain->SetBranchAddress("DateSec",&date);
  if (cuttrigger != -1) chain->SetBranchAddress("TriggerBit1",&bittrigger1);
  if (cuttrigger != -1) chain->SetBranchAddress("TriggerBit2",&bittrigger2);
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("DateSec",1);
  if (cuttrigger != -1) chain->SetBranchStatus("TriggerBit1",1);
  if (cuttrigger != -1) chain->SetBranchStatus("TriggerBit2",1);

  string channeltype=ChannelType(channel);
  TFile f1(ntpfile.c_str(),"READ");
  TTree* etree = (TTree*)f1.Get("energytree");
  etree->SetBranchAddress(("E0"+channeltype).c_str(),&energy);
  etree->SetBranchStatus("*",0);
  etree->SetBranchStatus(("E0"+channeltype).c_str(),1);

  ifstream buf(periodfile.c_str(),ios::in);
  vector<ULong_t> vti, vtf;
  ULong_t ti,tf;
  while (!buf.eof()) {
    buf >> ti >> tf;
    vti.push_back(ti); vtf.push_back(tf);
  }
  buf.close();
  int indice_period=0; int nbperiods=vti.size();
  ti=vti[indice_period]; tf=vtf[indice_period];

  Int_t nbins=400;//400 normally - 60 was used for heats ID201/ID3 run 10;
  Int_t min=-10, max=10;
  const char* thebase = ("Baseline "+channel).c_str();
  TH1F* hampl = new TH1F(thebase,thebase,nbins,min,max);
  TF1* func = new TF1("f","gaus",0.5*min,0.5*max); // param important!
  func->FixParameter(1,0);
  TCanvas* cb;
  if (wait) cb = new TCanvas("baseline","baseline");

  Bool_t long_gap=0;
  Int_t nbevts=chain->GetEntries();
  if (etree->GetEntries() != nbevts) cout << "energy tree length pbl" << endl;
  for (Int_t i=0; i<nbevts; i++) {
    chain->GetEntry(i); etree->GetEntry(i);
    //    if (date < ti-100) cerr << "Warning evt too early!" << endl; // test...
    if (date > tf || i == nbevts-1) { // Finish the histogram, and go to the next period.
      if (hampl->GetEntries() > 100) {
	if (wait) {
	  cb->Clear();
	  hampl->Draw();
	  hampl->Fit("f","RQ");
	  cb->SetLogy(1);
	  cb->Update();
	} else {
	  hampl->Fit("f","RQN");
	}
	Float_t fwhm = 2.35482*func->GetParameter(2);
	time_t ti_t = (time_t)ti, tf_t = (time_t)tf;
	cout << "From "<<string(ctime(&ti_t),16)<<" to "<<string(ctime(&tf_t),16)<<" ("<<hampl->GetEntries()<<" entries) : FWHM = "<< fwhm <<" keV";//<<endl;
	if (wait) cin.get();
	else cout <<endl;
	if (outputfile != "") output <<ti<<" "<<tf<<" "<<fwhm<<endl;
      } else {
	cerr << "not enough events in subperiod :" << hampl->GetEntries()<<endl;
	if (outputfile != "") output <<ti<<" "<<tf<<" "<<0<<endl;
      }
      long_gap=0;
      while (date > tf) {
	indice_period+=1;
	if (indice_period >= nbperiods) {cerr << "Warning going after end of period" << endl; return 0;}
	if (long_gap && (outputfile != "")) output<<ti<<" "<<tf<<" "<<0<<endl;
	ti=vti[indice_period]; tf=vtf[indice_period];
	long_gap=1;
      }
      hampl->Reset();
    }

    if (date >= ti) {
      bool trig=0;
      if (cuttrigger >=0) trig=DetectTrig(bittrigger1,bittrigger2,cuttrigger);
      if (!trig && energy != 0 && energy > min && energy < max) hampl->Fill(energy);
    } // else do NOT fill the histogram!  
  }
  if (wait) cb->Close();

  etree->SetBranchStatus("*",1);
  f1.Close();

  if (outputfile != "") output.close();
  chain->SetBranchStatus("*",1);

  return 0;
}
