//_____________________________________________
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
  fStatusCheckPercentage = 0.10;
  fReport = 2;
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

Bool_t KAmpKounselor::Scout( int maxNumEvents)
{
  //check to see if any of the KAmpSites that are managed by this KAmpKounselor need
  //to "scout" their kampsite. For example, a KAmpSite that employs an Optimal Filter
  //will need to scout out the data in order to estimate the noise power for each channel.
  
  if(!CheckSetup()) return false;
  
  fInput->cd();
  
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
  if(bNeedScouting){
    
    int numEvents = fInput->GetEntries();
    if(maxNumEvents > -1) {
      cout << "KAmpKounselor. Only scouting the first " << maxNumEvents << " events in the file" << endl; 
      numEvents = maxNumEvents;
    }

    KRawEvent *e = (KRawEvent *)fInput->GetEvent();

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
  //call each KampSite's PrepareKampSite method, which is used to calculate any information
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

Int_t KAmpKounselor::Run( int maxNumEvents)
{
  //loop through the data, passing data to the KAmpSites in order to calculate
  //pulse amplitudes and other characteristics of each event.
  
  if(!CheckSetup()) return false;
    
  KAmpEvent *eventOutput = (KAmpEvent *)fOutput->GetEvent();
  KRawEvent *eventInput = (KRawEvent *)fInput->GetEvent();
  
  vector<KAmpSite *>::iterator it;
  
  fInput->cd();
  int numEvents = fInput->GetEntries();
  if(maxNumEvents > -1) {
    cout << "KAmpKounselor. Only processing the first " << maxNumEvents << " events in the file" << endl; 
    numEvents = maxNumEvents;
  }

  int checkpoint = numEvents*fStatusCheckPercentage;
  
  for(int ii = 0; ii < numEvents; ii++){
    fInput->GetEntry(ii);
    
    fOutput->NewEvent();
    *(KEvent *)eventOutput = *(KEvent *)eventInput; //copy the base-class stuff
        
    //copy the muon veto system record information
    KRawMuonVetoSysRecord *muonRaw = (KRawMuonVetoSysRecord *)eventInput->GetMuonVetoSystemRecord();

    //NO, this is NOT a bug, its just bad design. :)  the KAmpEvent holds KRawMuonVeto data....
    KRawMuonVetoSysRecord *muonAmp = (KRawMuonVetoSysRecord *)eventOutput->GetMuonVetoSystemRecord();
    *muonAmp = *muonRaw;


    for(int j = 0; j < eventInput->GetNumBolos(); j++){
      
      KRawBolometerRecord *boloRaw = (KRawBolometerRecord *)eventInput->GetBolo(j);      
      KAmpBolometerRecord *boloAmp = eventOutput->AddBolo(boloRaw);
      
      for( it = fKampSites.begin(); it < fKampSites.end(); it++){
        (*it)->RunKampSite(boloRaw, boloAmp, eventOutput);
      }
      
    }
    
    fOutput->Fill();

    for( it = fKampSites.begin(); it < fKampSites.end(); it++){      
      if (fReport >= 2 && ii % checkpoint == 0){
        cout << "KAmpKounselor. calling ReportStatus. Reached event: " << ii << " out of " << numEvents <<  " events to be processed" << endl;
        (*it)->ReportStatus();    
      }
    }


  }
  
  int writeReturn = fOutput->Write();
  fOutput->Close();
  fInput->Close();

  return writeReturn;
}


void KAmpKounselor::ReportResults(void)
{
  vector<KAmpSite *>::iterator it;

  for( it = fKampSites.begin(); it < fKampSites.end(); it++){
      (*it)->ReportResults();
  }
}


Bool_t KAmpKounselor::RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile,  int maxNumEvents)
{
  //calls SetFiles, Scout, Prepare and then Run. Returns the result of Run.
  
  SetFiles(inputRawKDataFile, outputAmpKDataFile);
  
  if(!Scout(maxNumEvents)) return false;
  
  if(!Prepare()) return false;
  
  Int_t retVal = Run(maxNumEvents);

  if(fReport >= 1)
    ReportResults();

  return retVal;
}

