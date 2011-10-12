#include "KDataReader.h"
#include "KDataWriter.h"
#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KEvent.h"
#include "KRawMuonVetoSysRecord.h"
#include "KRawBolometerRecord.h"
#include "KAmpBolometerRecord.h"
#include "KRawSambaRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KSambaRecord.h"

#include "KBackyardKampSite.h"

#include <iostream>

using namespace std;

int main(int /*argc*/, char* argv[]){
  KDataReader f(argv[1]);
  KDataWriter ff(argv[2],"KAmpEvent");
  KAmpEvent *ee = (KAmpEvent *)ff.GetEvent();
  KRawEvent *e = (KRawEvent *)f.GetEvent();
 
  KBackyardKampSite mBackYard;  
  //add KAmpSites HERE!
  
  
  int numEvents = f.GetEntries();
  
  //loop through the raw data and pass it to the 
  //kampsites so they may KAmpSite::ScoutKampSite()
  
  //will need to loop through the data here in order to build up the noise power spectrum
  //to be used in the optimal filter
  //for(int i = 0; i < numEvents; i++){
  //  f.GetEntry(i);
  //  for(int j = 0; j < e->GetNumBoloPulses(); j++){
  //    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
  //    mBackYard.ScoutKampSite(pRaw, e);
  //    }
  //  }
  //}
  
  
  //loop through the data, passing data to the KAmpSites in order to calculate
  //pulse amplitudes.
  
  for(int i = 0; i < numEvents; i++){
    f.GetEntry(i);
    
    ee->Clear("C");
    *(KEvent *)ee = *e; //copy the base-class stuff
    
    if(i % 100 == 0) cout << "entry " << i << endl;
    
    //copy the muon veto system record information
    KRawMuonVetoSysRecord *muonRaw = (KRawMuonVetoSysRecord *)e->GetMuonVetoSystemRecord();
    KRawMuonVetoSysRecord *muonAmp = (KRawMuonVetoSysRecord *)ee->GetMuonVetoSystemRecord();
    *muonAmp = *muonRaw;

    for(int j = 0; j < e->GetNumBolos(); j++){
      if(i % 100 == 0) cout << "     bolo " << j << endl;
      KRawBolometerRecord *boloRaw = (KRawBolometerRecord *)e->GetBolo(j);
      KAmpBolometerRecord *boloAmp = ee->AddBolo(boloRaw);
      

      mBackYard.RunKampSite(boloRaw, boloAmp, ee);

      
      if(i %100 == 0) cout << endl;
    }
    
    ff.Fill();
  }
  
  int writeReturn = ff.Write();
  ff.Close();
  f.Close();
  return writeReturn;
}
