
#include <cmath>
#include <vector>
#include <string>

int BuildPeriodList(TChain* chain, string qualdir, UInt_t periodlength_min=100, bool with_run=1) {
  
  
  Double_t periodlength = 60*(Double_t)periodlength_min;
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("DateSec",1);
  chain->SetBranchStatus("RunName",1);
  ULong64_t date=0; char therun[9];
  chain->SetBranchAddress("DateSec",&date);
  chain->SetBranchAddress("RunName",&therun);
  
  vector<string> theruns; string currentrun;
  string listruns=qualdir+"/runlist.txt";
  ifstream buf(listruns.c_str(),ios::in);
  if (!buf) {
    cout << "No list of runs!" << endl;
    return 0;
  }
  while (buf >> currentrun) theruns.push_back(currentrun);
  buf.close();
  
  vector<ULong_t> ti, tf;
  vector<string> run_of_period;
  ULong64_t tstop_run=0, tstart_run=100000000000;
  currentrun=theruns[0]; Int_t p_run=0;
  for (UInt_t i=0; i<chain->GetEntries(); i++) {
    chain->GetEntry(i);
    if (strcmp(therun,currentrun.c_str())) {
      cout << "* Event "<<i<<" : "<<currentrun << " done"<<endl;
      if (tstop_run != 0) { // Make subdivisions in the past run
        Double_t nbdivisions=((Double_t)(tstop_run-tstart_run))/periodlength;
        if (nbdivisions < 1) nbdivisions=1;
        if (nbdivisions-floor(nbdivisions) < 0.5) nbdivisions=floor(nbdivisions);
        else nbdivisions=ceil(nbdivisions);
        Double_t localperiodlength=((Double_t)(tstop_run-tstart_run))/nbdivisions;
        cout << nbdivisions<<" divisions of length "<<localperiodlength/60.<<" mins" << endl;
        for (UInt_t p=0; p<nbdivisions; p++) {
          ti.push_back((ULong_t)tstart_run+(ULong_t)(p*localperiodlength));
          tf.push_back((ULong_t)tstart_run+(ULong_t)((p+1)*localperiodlength));
          if (with_run) run_of_period.push_back(currentrun);
        }
      }
      // Go to next run
      p_run++; currentrun=theruns[p_run]; 
      tstart_run=date; tstop_run = date;
      if (strcmp(therun,currentrun.c_str())) { 
        string dum(therun); 
        cout << "Pbl with runs "<<dum<< " "<<currentrun<<endl;
      }
    } else {
      // Update the run time borders
      if (date < tstart_run) tstart_run=date;
      if (date > tstop_run) tstop_run=date;
    }
  }
  // Finish last period:
  cout << "* LastEvent: "<<currentrun << " done"<<endl;
  Double_t nbdivisions=((Double_t)(tstop_run-tstart_run))/periodlength;
  if (nbdivisions < 1) nbdivisions=1;
  if (nbdivisions-floor(nbdivisions) < 0.5) nbdivisions=floor(nbdivisions);
  else nbdivisions=ceil(nbdivisions);
  Double_t localperiodlength=((Double_t)(tstop_run-tstart_run))/nbdivisions;
  cout << nbdivisions<<" divisions of length "<<localperiodlength/60.<<" mins" << endl;
  for (UInt_t p=0; p<nbdivisions; p++) {
    ti.push_back((ULong_t)tstart_run+(ULong_t)(p*localperiodlength));
    tf.push_back((ULong_t)tstart_run+(ULong_t)((p+1)*localperiodlength));
    if (with_run) run_of_period.push_back(currentrun);
  }
  
  string periodfile=qualdir+"/period_list.txt";
  ofstream buf2(periodfile.c_str(),ios::out);
  for (UInt_t p=0; p<ti.size(); p++)
    buf2 << ti[p] <<" "<< tf[p] << endl;
  buf2.close();
  
  periodfile=qualdir+"/period_list_withruns.txt";
  ofstream buf3(periodfile.c_str(),ios::out);
  for (UInt_t p=0; p<ti.size(); p++)
    buf3 << ti[p] <<" "<< tf[p] <<" "<<run_of_period[p] << endl;
  buf3.close();
  
  
  chain->SetBranchStatus("*",1);
  return 0;
}

