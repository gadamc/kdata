//
// KBlackForestKAmpSite.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KBlackForestKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "TString.h"
#include <iostream>


using namespace std;


KBlackForestKAmpSite::KBlackForestKAmpSite(void)
{
  SetName("KBlackForestKAmpSite");

}

KBlackForestKAmpSite::~KBlackForestKAmpSite(void)
{

}

Bool_t KBlackForestKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
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
      
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
      KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Ion");
      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
      fMTKamp.MakeKamp(pRaw, rec);
      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Ion-Baseline");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fMTKamp.MakeBaseKamp(pRaw, recBase);
      
      if(fabs(rec->GetAmp()) > maxPeak){
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
        fMTKamp.SetName("BlackForestKAmperProto-Heat-Fixed");
        SetTRefLinksForKAmpEvent(recFixed, boloAmp,pAmp);
      
        
      
        KRawBoloPulseRecord *pRawIon = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(precNum);
        if(PeakPos != -1){
          PeakPos = (double) pRaw->GetPretriggerSize()+(pRawIon->GetPulseTimeWidth()*(PeakPos - (double)pRawIon->GetPretriggerSize())/((double)pRaw->GetPulseTimeWidth()));
        }
        fMTKamp.MakeKamp(pRaw, recFixed, PeakPos);
      }
      
      // without fixed peak position
      KPulseAnalysisRecord *recFree  =  ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Heat-Not-Fixed");
      SetTRefLinksForKAmpEvent(recFree, boloAmp, pAmp);
      fMTKamp.MakeKamp(pRaw, recFree);
      
      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Heat-Baseline");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fMTKamp.MakeBaseKamp(pRaw, recBase);
    }
  }
  
  return true;
}  

Bool_t KBlackForestKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  return true;
}  
