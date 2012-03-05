//
// KFeldbergKAmpSite.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KFeldbergKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "TString.h"
#include <iostream>
#include <math.h>


using namespace std;


KFeldbergKAmpSite::KFeldbergKAmpSite(void)
{
  SetName("KFeldbergKAmpSite");

}

KFeldbergKAmpSite::~KFeldbergKAmpSite(void)
{

}

Bool_t KFeldbergKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  double PeakPos = -1;
  double maxPeak = 0.;
  int precNum = -1;
  // create KPulseAnalysisRecords for ionisation pulses and
  // look for the largest pulse in ionisation channels to fix the position of the heat pulse
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    if(!pRaw->GetIsHeatPulse()){
      // Create KPulseAnalysisRecord and a valid KAmpEvent
      //cout << "channel:"<< pRaw->GetChannelName()<<endl;
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
      KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Ion");
      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
      fFCKamp.MakeKamp(pRaw, rec);
      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Ion");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fFCKamp.MakeBaseKamp(pRaw, recBase);
      
      
      if((fabs(rec->GetAmp()) > maxPeak) and rec->GetAmp()!=-99999){
        maxPeak = fabs(rec->GetAmp());
        PeakPos = rec->GetPeakPosition();
        precNum = k;
      }
    }
  }
  // create KPulseAnalysisRecords for heat pulses, use largest ionisation pulse to fix the peak position
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
    
    if(pRaw->GetIsHeatPulse()){      
      // with peak position fixed by ionisation
      if(precNum != -1){
        // Create KPulseAnalysisRecord and a valid KAmpEvent
        
        KPulseAnalysisRecord *recFixed  =  ee->AddPulseAnalysisRecord();
        fFCKamp.SetName("FeldbergKAmper-Heat-Fixed");
        SetTRefLinksForKAmpEvent(recFixed, boloAmp,pAmp);
      
        
      
        KRawBoloPulseRecord *pRawIon = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(precNum);
        if(PeakPos != -1){
          PeakPos = (double) pRaw->GetPretriggerSize()+(pRawIon->GetPulseTimeWidth()*(PeakPos - (double)pRawIon->GetPretriggerSize())/((double)pRaw->GetPulseTimeWidth()));
        }
        fFCKamp.MakeKamp(pRaw, recFixed, PeakPos);
      }
      
      // without fixed peak position
      KPulseAnalysisRecord *recFree  =  ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Heat-Not-Fixed");
      SetTRefLinksForKAmpEvent(recFree, boloAmp, pAmp);
      fFCKamp.MakeKamp(pRaw, recFree);
      
      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Heat");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fFCKamp.MakeBaseKamp(pRaw, recBase);
    }
  } 
  return true;
}  

Bool_t KFeldbergKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  return true;
}  
