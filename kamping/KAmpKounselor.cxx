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
  fInput = 0;
  fOutput = 0;
}

KAmpKounselor::~KAmpKounselor(void)
{
  if(fInput) delete fInput;
  if(fOutput) delete fOutput;
}

void KAmpKounselor::AddKAmpSite(KAmpSite *aSite)
{
  fKampSites.push_back(aSite);
  
}

void KAmpKounselor::SetFiles(const char* inputRawKDataFile, const char* outputAmpKDataFile)
{
  if(fInput) delete fInput;
  fInput = new KDataReader(inputRawKDataFile);
  
  if(fOutput) delete fOutput;
  fOutput = new KDataWriter(outputAmpKDataFile, "KAmpEvent");
  
}

Bool_t KAmpKounselor::CheckSetup()
{
  if (fInput==0 || fOutput == 0) return false;
  else return true;
}

Bool_t KAmpKounselor::Scout()
{
  //check to see if any of the KAmpSites that are managed by this KAmpKounselor need
  //to "scout" their kampsite. For example, a KAmpSite that employs an Optimal Filter
  //will need to scout out the data in order to estimate the noise power for each channel.
  
  if(!CheckSetup()) return false;
  
  fInput->cd();
  int numEvents = fInput->GetEntries();
  KRawEvent *e = (KRawEvent *)fInput->GetEvent();
  
  vector<KAmpSite *>::iterator it;
  Bool_t bNeedScouting = false;
  
  KDataProcessingInfo *fNewInfo=0;
  fNewInfo = (KDataProcessingInfo *)(fOutput->GetTTree()->GetUserInfo()->Last());
  fNewInfo->AddModule("KAmpKounselor");
  
  for( it = fKampSites.begin(); it < fKampSites.end(); it++){
    fNewInfo->AddCommand( (*it)->GetName());
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
      fInput->GetEntry(i);
      for(int j = 0; j < e->GetNumBoloPulses(); j++){
        KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
        for( it = fKampSites.begin(); it < fKampSites.end(); it++){
          if ( (*it)->NeedScout() )
            (*it)->ScoutKampSite(pRaw, e);
        }
      }
    }
  }
  
  return true;
}

Bool_t KAmpKounselor::Prepare()
{
  //call each KampSite's Prepare method, which is used to calculate any information
  //obtained during the ScoutKampSite method that will be needed before running the analysis
  //on the data.

  if(!CheckSetup()) return false;

  vector<KAmpSite *>::iterator it;

  for( it = fKampSites.begin(); it < fKampSites.end(); it++){
    if ( (*it)->NeedScout() )
      (*it)->PrepareKampSite();
  }
  
  return true;
}

Bool_t KAmpKounselor::Run()
{
  //loop through the data, passing data to the KAmpSites in order to calculate
  //pulse amplitudes and other characteristics of each event.
  
  if(!CheckSetup()) return false;
    
  KAmpEvent *ee = (KAmpEvent *)fOutput->GetEvent();
  KRawEvent *e = (KRawEvent *)fInput->GetEvent();
  
  vector<KAmpSite *>::iterator it;
  
  fInput->cd();
  int numEvents = fInput->GetEntries();
  
  for(int ii = 0; ii < numEvents; ii++){
    fInput->GetEntry(ii);
    
    ee->Clear("C");
    *(KEvent *)ee = *(KEvent *)e; //copy the base-class stuff
        
    //copy the muon veto system record information
    KRawMuonVetoSysRecord *muonRaw = (KRawMuonVetoSysRecord *)e->GetMuonVetoSystemRecord();
    KRawMuonVetoSysRecord *muonAmp = (KRawMuonVetoSysRecord *)ee->GetMuonVetoSystemRecord();
    *muonAmp = *muonRaw;

    for(int j = 0; j < e->GetNumBolos(); j++){
      
      KRawBolometerRecord *boloRaw = (KRawBolometerRecord *)e->GetBolo(j);      
      KAmpBolometerRecord *boloAmp = ee->AddBolo(boloRaw);
      
      for( it = fKampSites.begin(); it < fKampSites.end(); it++){
        (*it)->RunKampSite(boloRaw, boloAmp, ee);
      }
      
    }
    
    fOutput->Fill();
  }
  
  int writeReturn = fOutput->Write();
  fOutput->Close();
  fInput->Close();
  return writeReturn;
}


Bool_t KAmpKounselor::RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile)
{
  //calls SetFiles, Scout, Prepare and then Run. Returns the result of Run.
  
  SetFiles(inputRawKDataFile, outputAmpKDataFile);
  
  if(!Scout()) return false;
  
  if(!Prepare()) return false;
  
  return Run();
}
