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
      //cout << "channel:"<< pRaw->GetChannelName()<<endl;
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
      KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Ion");

      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
      map<string, KResult> resMap = fMTKamp.MakeKamp(pRaw);

      //fill in the results
      if(resMap.find("amp") != resMap.end()) rec->SetAmp(resMap["amp"].fValue);

      if(resMap.find("peakPosition") != resMap.end()) rec->SetPeakPosition(resMap["peakPosition"].fValue);

      if(resMap.find("baselineRemoved") != resMap.end()) rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);

      if(resMap.find("slopeRemoved") != resMap.end()) rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);


      rec->SetName(fMTKamp.GetName());
      rec->SetUnit(0);

      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Ion-Baseline");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      resMap = fMTKamp.MakeBaseKamp(pRaw);
      recBase->SetName(fMTKamp.GetName());
      recBase->SetUnit(0);
      
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
      
      
      // without fixed peak position
      KPulseAnalysisRecord *recFree  =  ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Heat-Not-Fixed");
      SetTRefLinksForKAmpEvent(recFree, boloAmp, pAmp);

      map<string, KResult> resMap = fMTKamp.MakeKamp(pRaw);

      //fill in the results
      if(resMap.find("amp") != resMap.end())
        recFree->SetAmp(resMap["amp"].fValue);

      if(resMap.find("peakPosition") != resMap.end())
        recFree->SetPeakPosition(resMap["peakPosition"].fValue);

      if(resMap.find("baselineRemoved") != resMap.end())
        recFree->SetBaselineRemoved(resMap["baselineRemoved"].fValue);

      if(resMap.find("slopeRemoved") != resMap.end())
        recFree->SetSlopeRemoved(resMap["slopeRemoved"].fValue);

      recFree->SetName(fMTKamp.GetName());
      recFree->SetUnit(0);

      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase2 = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKAmperProto-Heat-Baseline");
      SetTRefLinksForKAmpEvent(recBase2, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fMTKamp.MakeBaseKamp(pRaw);
      recBase2->SetName(fMTKamp.GetName());
      recBase2->SetUnit(0);
    }
  } 
  return true;
}  

Bool_t KBlackForestKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  return true;
}  
