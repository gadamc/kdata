
#include <vector>
#include "ChannelName.C"
#include <string>
#include "DumpEvent.C"

string int2str(Int_t aNum,Int_t aWidth) {
  std::ostringstream lStream; 
  lStream.width(aWidth); 
  lStream.fill('0'); 
  lStream << aNum;
  return lStream.str();
}

Int_t PlotEvent(string sambarun, int sambanum, string bolo, string basedir="/Users/armengau/Data/Edelweiss/TestRootAna/", string libdir="/Users/armengau/Edelweiss/RootAnalysis/EdwRootAna/lib",bool fitpulse=1, long chainnum=-1,bool remove_pattern=1) {

  string evtdir=basedir+"/rootevts/";
  string tmpltdir=basedir+"/templates/";
  string ntpdir=basedir+"/ntps/";

  gSystem->Load((libdir+"/EraLib.so").c_str());
  gEnv->SetValue("Canvas.ShowToolBar",0);
  gEnv->SetValue("Canvas.ShowEditor",0);

  string ss;
  TChain* chain = new TChain("EdwTree","rawevts");
  system(("ls "+evtdir+" | grep "+sambarun+" > tmptoto.txt").c_str());
  ifstream buf2("tmptoto.txt",ios::in);
  while (buf2 >> ss) chain->AddFile((evtdir+"/"+ss).c_str());
  buf2.close();
  system("rm -f tmptoto.txt");
  Int_t nbevts=chain->GetEntries();
  cout << nbevts<<" events in run "<<sambarun<<endl;

  EdwEvent* evt = NULL;
  EdwEventHeader* hd = NULL;
  chain->SetBranchAddress("Event",&evt);
  Int_t numevt=-1, i=0;
  chain->GetEntry(sambanum-1);
  hd = evt->Header();
  numevt=hd->Num();
  while (numevt != sambanum) {
    if (i != 0) evt->Clear();
    chain->GetEntry(i);
    hd = evt->Header();
    numevt=hd->Num();
    i++;
    if (i == nbevts) break;
  }
  
  TCanvas* cheat = new TCanvas("c_heat","c_heat",500,400);
  TCanvas* cion = new TCanvas("c_ion","c_ion",800,600);
  if (bolo.find("FID")!= string::npos) cion->Divide(2,2);
  else cion->Divide(2,3);
  int padnum=1;
  
  TChain* ntpchain;
  Float_t* ampl= new Float_t[evt->NbPulses()];
  Float_t* toffset=new Float_t[evt->NbPulses()];
  Float_t* chi2=new Float_t[evt->NbPulses()];
  if (fitpulse) {
    ntpchain = new TChain("EdwRootNtp","calib evts");
    system(("ls "+ntpdir+" | wc -l > tmptoto.txt").c_str());
    ifstream buf3("tmptoto.txt",ios::in);
    int nbntuples=0; buf3 >> nbntuples; buf3.close();
    system("rm -f tmptoto.txt");
    for (int k=1; k<=nbntuples; k++) { std::ostringstream truc; truc<<k; ntpchain->AddFile((ntpdir+"/ntp_"+truc.str()+".root").c_str()); }
    if (chainnum == -1) {
      cout <<"Searching evt in chain.." << endl;
      chainnum=DumpEvent(ntpchain,sambarun,sambanum,0);
    }
    UInt_t evtnum=0; char therun[9];
    ntpchain->SetBranchAddress("RunName",&therun);
    ntpchain->SetBranchAddress("EventNum",&evtnum);
    for (int j=0; j<evt->NbPulses(); j++) {
      string ntpchannel=(evt->Pulse(j))->Channel();
      if (ntpchannel.find("Gc2") == string::npos) {
	ntpchannel.replace(ntpchannel.find_first_of(" "),1,"_");
	ntpchain->SetBranchAddress(("WienerAmpl_"+ntpchannel).c_str(),&(ampl[j]));
	ntpchain->SetBranchAddress(("WienerBin_"+ntpchannel).c_str(),&(toffset[j]));
	ntpchain->SetBranchAddress(("WienerChi2_"+ntpchannel).c_str(),&(chi2[j]));
      }
    }
    ntpchain->GetEntry(chainnum);
    if (sambanum != evtnum || strcmp(therun,sambarun.c_str()))
      cout << "Evt not matching. " <<sambarun<<" "<<sambanum<<endl;
    cout << therun <<" "<<evtnum<<" -- "<<chainnum<<endl;
  }

  cout <<evt->NbPulses()<<" pulses trouves"<<endl;
  for (int j=0; j<evt->NbPulses(); j++) {
    EdwPulse* pulse=evt->Pulse(j);
    string channel=pulse->Channel();
    if (BoloName(channel," ") == bolo && pulse != NULL) {
      cout << "trace pr "<<channel<<endl;
      vector<Short_t> ttrace = pulse->Trace();
      int nbpts=ttrace.size();
      vector<Float_t> trace(nbpts,0);
      for (int k=0; k<nbpts; k++) trace[k]=(Float_t)ttrace[k];
      // substract base
      float mean=0;
      int basestart=nbpts/4;
      int basestop=(nbpts/2)-20;
      if (channel.find("chaleur") == string::npos) {
	basestart=3300; // 3800
	basestop=3400; // 3900
      }
      if (channel.find("chaleur") == string::npos && remove_pattern==1) { // depattern
	Int_t patlength=200; Int_t nbpatt = 12; // PARAMS EN DUR
	if (channel.find("FID401") != string::npos || channel.find(" ID402")!= string::npos) {patlength=400; nbpatt=7;}
	vector<Float_t> pattern(patlength,0);
	for (int pp=0; pp<nbpatt; pp++) {
	  for (int kk=0; kk<patlength; kk++) 
	    (pattern[kk]) += (trace.at(kk+patlength*pp));
	}
	for (int kk=0; kk<patlength; kk++) pattern[kk] /= nbpatt;
	for (int pp=0; pp<nbpts; pp++) {
	  (trace[pp]) -= (pattern[fmod((double)pp,(double)patlength)]);
	}
	pattern.clear();
      } else {
	//	for (int pp=0; pp<nbpts; pp++) cout << trace[pp]<<endl;
      }
      for (int pp=basestart; pp<basestop; pp++) mean+= (trace[pp]); // PARAM EN DUR
      mean/=((Float_t)(basestop-basestart));
      for (int pp=0; pp<nbpts; pp++) trace[pp] -= mean;

      string name=channel;
      name+=(" - event "+int2str(numevt,5)+" - run "+sambarun);
      TH1F* hh = new TH1F(name.c_str(),name.c_str(),nbpts,0,nbpts);
      for (int k=0; k<nbpts; k++) hh->SetBinContent(k+1,trace[k]);
      gStyle->SetOptStat(0);
      if (channel.find("chaleur") != string::npos) cheat->cd();
      else {cion->cd(padnum); padnum++;
	//	hh->GetYaxis()->SetRangeUser(-60,60); // custom!
      }
      hh->Draw();
      //     gPad->Update();
      ttrace.clear(); trace.clear();

      // Add the obtained fit
      if (fitpulse) {
	string tmpltfile=tmpltdir+"/template_"+channel+".txt";
	TH1F* fit = new TH1F(("pulse_"+int2str(j)).c_str(),"fit",nbpts,0,nbpts);
	Float_t pt_tmplt;
	ifstream buf4(tmpltfile.c_str(),ios::in);
	if (!buf4) cout << "Not found "<<tmpltfile<<endl;
	for (int l=0; l<nbpts; l++) {
	  buf4 >> pt_tmplt; fit->SetBinContent(l+1,pt_tmplt);
	}
	buf4.close();
	cout << "A="<<ampl[j]<<"  t="<<toffset[j]<<"  chi2="<<chi2[j]<<endl;
	fit->Scale(ampl[j]);
	Float_t tmpltbin=fit->GetMaximumBin();
	if (fit->GetMaximum() < -fit->GetMinimum()) tmpltbin=fit->GetMinimumBin();
	(toffset[j])-=tmpltbin;
	if (toffset[j] >= 1) {
	  for (int l=nbpts; l>toffset[j]+1; l--) fit->SetBinContent(l,fit->GetBinContent(l-toffset[j]-1));
	} else if (toffset[j] <= -1) {
	  for (int l=1; l<nbpts+toffset[j]-1; l++) fit->SetBinContent(l,fit->GetBinContent(l-toffset[j]-1));
	}
	
	fit->SetLineWidth(2); fit->SetLineColor(2);
	fit->Draw("same");
      }
    }
    if (pulse != NULL) pulse->Clear();
  }
  //  evt->Clear();
  
  //  }
  
  return 0;
}
