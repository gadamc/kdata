//
// KAmpKounselor.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KAmpKounselor.h"
#include "KDataReader.h"
#include "KDataWriter.h"
#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KRawMuonVetoSysRecord.h"
#include "KRawBolometerRecord.h"
#include "KAmpBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KAmpSite.h"
#include "KDataProcessingInfo.h"
#include <iostream>

using namespace std;

KAmpKounselor::KAmpKounselor(void)
{

}

KAmpKounselor::~KAmpKounselor(void)
{

}

void KAmpKounselor::AddKAmpSite(KAmpSite *aSite)
{
  fKampSites.push_back(aSite);
  
}

Bool_t KAmpKounselor::RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile)
{
  KDataReader f(inputRawKDataFile);
  KDataWriter ff(outputAmpKDataFile,"KAmpEvent");
  KAmpEvent *ee = (KAmpEvent *)ff.GetEvent();
  KRawEvent *e = (KRawEvent *)f.GetEvent();
  
  f.cd();
  int numEvents = f.GetEntries();
  /*
  KDataProcessingInfo *mNewInfo = (KDataProcessingInfo *)(ff.GetTTree()->GetUserInfo()->Last());
  mNewInfo->AddModule("KAmpKounselor");
  */
  
  //check to see if any of the KAmpSites that are managed by this KAmpKounselor need
  //to "scout" their kampsite. For sure, a KAmpSite that employs an Optimal Filter
  //will need to scout out the data in order to estimate the noise power for each channel.
  vector<KAmpSite *>::iterator it;
  Bool_t bNeedScouting = false;
  for( it = fKampSites.begin(); it < fKampSites.end(); it++){
    //mNewInfo->AddCommand( (*it)->GetName());
    if ( (*it)->NeedScout()){
      bNeedScouting = true;
      break;
    }
  }
  
  //loop through the raw data and pass it to the 
  //kampsites so they may KAmpSite::ScoutKampSite()
  //will need to loop through the data here in order to build up the noise power spectrum
  //to be used in the optimal filter
  if(bNeedScouting){
    for(int i = 0; i < numEvents; i++){
      f.GetEntry(i);
      for(int j = 0; j < e->GetNumBoloPulses(); j++){
        KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
        for( it = fKampSites.begin(); it < fKampSites.end(); it++){
          if ( (*it)->NeedScout() )
            (*it)->ScoutKampSite(pRaw, e);
        }
      }
    }
  }
  
  
  //loop through the data, passing data to the KAmpSites in order to calculate
  //pulse amplitudes.
  for(int ii = 0; ii < numEvents; ii++){
    f.GetEntry(ii);
    
    ee->Clear("C");
    *(KEvent *)ee = *(KEvent *)e; //copy the base-class stuff
        
    //copy the muon veto system record information
    KRawMuonVetoSysRecord *muonRaw = (KRawMuonVetoSysRecord *)e->GetMuonVetoSystemRecord();
    KRawMuonVetoSysRecord *muonAmp = (KRawMuonVetoSysRecord *)ee->GetMuonVetoSystemRecord();
    *muonAmp = *muonRaw;

    for(int j = 0; j < e->GetNumBolos(); j++){
      cout << j << endl;
      KRawBolometerRecord *boloRaw = (KRawBolometerRecord *)e->GetBolo(j);      
      KAmpBolometerRecord *boloAmp = ee->AddBolo(boloRaw);
      
      for( it = fKampSites.begin(); it < fKampSites.end(); it++){
        (*it)->RunKampSite(boloRaw, boloAmp, ee);
      }
      
    }
    
    ff.Fill();
  }
  
  int writeReturn = ff.Write();
  ff.Close();
  f.Close();
  return writeReturn;

}
