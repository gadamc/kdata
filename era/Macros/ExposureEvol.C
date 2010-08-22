
#include "BuildPeriodList.C"
#include "ChannelName.C"
#include "UpdateEnergy.C"

int ExposureEvol(TChain* chain, string bolo, string qualdir, Int_t bolonum=-1, Int_t cutpolarflag=-1, Float_t samba_deadtime=0.250) {

  // temps mort: "normalement" (JG) pour le run 12 c'est 400ms x Mac rate avant une certaine date; 
  // puis 250ms x physics evt rate on single bolo

  Float_t cut_ldbheat=gLdbCut_Eheat;
  Float_t cut_ldbcol=gLdbCut_Ecol;
  Float_t cut_ldbfid=gLdbCut_Efid;
  Float_t cut_ldbgar=gLdbCut_Egar;
  Float_t cut_ldbvet=gLdbCut_Evet;

  // Build the "periodcutfile" period_list_bolo and plot exposure vs time.
  if (!IsID(bolo)) cout << "Not an ID.. pbl." <<endl; 

  string raw_listruns=qualdir+"/runlist.txt";
  string periodfile=qualdir+"/period_list_withruns.txt";
  string periodcutfile=qualdir+"/period_list_"+bolo+".txt";

  ifstream buf1(periodfile.c_str(),ios::in);
  int nbperiods=0;string ss;
  while(getline(buf1,ss)) nbperiods++;
  buf1.close();
  
  vector<string> badruns;
  string run; int polarflag;
  string runfile=qualdir+"/runlist_"+bolo+".txt";

  ifstream buf2(runfile.c_str(),ios::in);
  while (buf2 >> run >> polarflag) {
    if (cutpolarflag==-1 && polarflag==-1) badruns.push_back(run);
    if (cutpolarflag!=-1 && polarflag != cutpolarflag!=polarflag)
      badruns.push_back(run);
  }
  int nbbadruns=badruns.size();
  buf2.close();
  
  vector<Float_t> ldb_heat=GetLDB(ChannelName(bolo,"heat"),qualdir,nbperiods);
  vector<Float_t> ldb_col1=GetLDB(ChannelName(bolo,"col1"),qualdir,nbperiods);
  vector<Float_t> ldb_col2=GetLDB(ChannelName(bolo,"col2"),qualdir,nbperiods);
  vector<Float_t> ldb_vet1=GetLDB(ChannelName(bolo,"vet1"),qualdir,nbperiods);
  vector<Float_t> ldb_vet2=GetLDB(ChannelName(bolo,"vet2"),qualdir,nbperiods);
  vector<Float_t> ldb_fid, ldb_gar;
  for (int p=0; p<nbperiods; p++) {
    if (ldb_col1[p]<0.01 && ldb_col2[p]<0.01) ldb_fid.push_back(0);
    else ldb_fid.push_back(ldb_col1[p]*ldb_col2[p]/sqrt(ldb_col1[p]*ldb_col1[p]+ldb_col2[p]*ldb_col2[p]));
  }
  vector<Float_t> ldb_gar1, ldb_gar2;
  int boloflag=0;
  if (IsID(bolo)) {
    ldb_gar1=GetLDB(ChannelName(bolo,"gar1"),qualdir,nbperiods);
    ldb_gar2=GetLDB(ChannelName(bolo,"gar2"),qualdir,nbperiods);
    for (int p=0; p<nbperiods; p++) {
      if (ldb_gar1[p]<0.01) ldb_gar.push_back(ldb_gar2[p]);
      else if (ldb_gar2[p]<0.01) ldb_gar.push_back(ldb_gar1[p]);
      else ldb_gar.push_back(ldb_gar1[p]*ldb_gar2[p]/sqrt(ldb_gar1[p]*ldb_gar1[p]+ldb_gar2[p]*ldb_gar2[p]));
    }
  } else boloflag=1;
  
  ULong64_t date; Int_t bittrigger1, bittrigger2;
  Int_t j_evt=0, nbevts=0;
  if (samba_deadtime > 0) {
    chain->SetBranchAddress("DateSec",&date);
    chain->SetBranchAddress("TriggerBit1",&bittrigger1);
    chain->SetBranchAddress("TriggerBit2",&bittrigger2);
    chain->SetBranchStatus("*",0);
    chain->SetBranchStatus("DateSec",1);
    chain->SetBranchStatus("TriggerBit1",1);
    chain->SetBranchStatus("TriggerBit2",1);
    chain->GetEntry(j_evt);
    nbevts=chain->GetEntries();
  }

  vector<Float_t> IntegralExpo_raw, IntegralExpo_cut;
  vector<Double_t> TimeStamps;
  string therun; ULong_t ti, tf;
  Float_t current_rawexpo=0, current_cutexpo=0;
  ifstream buf3(periodfile.c_str(),ios::in);
  ofstream buf4(periodcutfile.c_str(),ios::out);
  int p=0;
  while (buf3 >> ti >> tf >> therun) {
    TimeStamps.push_back(Double_t(ti));
    IntegralExpo_raw.push_back(current_rawexpo);
    IntegralExpo_cut.push_back(current_cutexpo);

    Float_t deadtimecor=0; bool trig;
    if (samba_deadtime > 0) {
      while (date < ti && j_evt < nbevts) {j_evt++; chain->GetEntry(j_evt);}
      while (date < tf && j_evt < nbevts) {
	j_evt++; chain->GetEntry(j_evt);
	trig=DetectTrig(bittrigger1,bittrigger2,bolonum);
	if (trig) deadtimecor += samba_deadtime; // in seconds! 
	if (fmod((double)j_evt,(double)100000) == 0) cout << j_evt << endl;
      }
    }
    if (deadtimecor > (tf-ti)) {cout << "ho "<<endl; deadtimecor=(tf-ti);}

    current_rawexpo += (Double_t(tf-ti)-deadtimecor)/86400.; // in days
    bool period_ok=1;
    for (int k=0; k<nbbadruns; k++) {
      if (badruns[k] == therun) period_ok=0;
    }
    if (ldb_heat[p] > cut_ldbheat || ldb_heat[p] <0.01) period_ok=0;
    if (ldb_col1[p]<0.01 || ldb_col2[p]<0.01) period_ok=0;
    if (ldb_col1[p] > cut_ldbcol || ldb_col2[p] > cut_ldbcol ) {
      if (ldb_vet1[p]>cut_ldbvet || ldb_vet2[p] > cut_ldbvet || 
	  ldb_vet1[p]<0.01 || ldb_vet2[p]<0.01) period_ok=0;
    }
    if (ldb_fid[p] > cut_ldbfid) period_ok=0;
    if (IsID(bolo)) {
      if (ldb_gar[p] > cut_ldbgar || ldb_gar[p]<0.01) period_ok=0;
    }
    /*    if (boloflag == 0) { // Pour l'instant pas de contrainte ldb garde
      if ((cutbaselines[5] != 0 && (ldb_gar1[p] > cutbaselines[5] || ldb_gar1[p] == 0)) ||
	  (cutbaselines[6] != 0 && (ldb_gar2[p] > cutbaselines[6] || ldb_gar2[p] == 0)))
	period_ok=0;
	}*/

    p++;
    if (period_ok) current_cutexpo += (Double_t(tf-ti)-deadtimecor)/86400.;

    buf4 << ti <<" "<<tf<<" "<<period_ok<<endl;
    TimeStamps.push_back(Double_t(tf));
    IntegralExpo_raw.push_back(current_rawexpo);
    IntegralExpo_cut.push_back(current_cutexpo);
  }
  buf3.close();
  buf4.close();

  chain->SetBranchStatus("*",1);


  // plot
  Int_t nbpts=TimeStamps.size();
  Double_t* xx = new Double_t[nbpts];
  for (int i=0; i<nbpts; i++) xx[i]=TimeStamps[i];
  Double_t* y1 = new Double_t[nbpts];
  Double_t* y2 = new Double_t[nbpts];
  for (int i=0; i<nbpts; i++) y1[i]=IntegralExpo_raw[i];
  for (int i=0; i<nbpts; i++) y2[i]=IntegralExpo_cut[i];
  TGraph* gr1 = new TGraph(nbpts,xx,y1);
  TGraph* gr2 = new TGraph(nbpts,xx,y2);
  gr1->SetTitle(("Cumulative exposure - "+bolo).c_str());
  gr1->GetXaxis()->SetTimeDisplay(1);
  gr1->GetYaxis()->SetTitle("Exposure(days)");
  gr1->Draw("AL");
  gr2->GetXaxis()->SetTimeDisplay(1);
  gr2->SetLineColor(2);
  gr2->Draw("sameL");
  TLegend* leg = new TLegend(0.15,0.75,0.45,0.85);
  leg->SetFillColor(0);
  leg->AddEntry(gr1,"Raw Samba exposure","L");
  leg->AddEntry(gr2,"After period cuts","L");
  leg->Draw();

  cout << "Exposure after cuts: "<<y2[nbpts-1]<<" days"<<endl;

  return 0;
}
