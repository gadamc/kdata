//_____________________________________________
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
#include <sstream>
#include <math.h>


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
      fMTKamp.SetName("BlackForestKampSite");

      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
      map<string, KResult> resMap = fMTKamp.MakeKamp(pRaw);

      //fill in the results
      FillResults(rec, resMap);

      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKampSiteBaseline");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      resMap = fMTKamp.MakeBaseKamp(pRaw);
      
      //fill in the results
      FillResults(recBase, resMap);
      
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
    
    if(pRaw->GetIsHeatPulse()){      
      
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
      
      // without fixed peak position
      KPulseAnalysisRecord *recFree  =  ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKampSite");
      SetTRefLinksForKAmpEvent(recFree, boloAmp, pAmp);

      map<string, KResult> resMap = fMTKamp.MakeKamp(pRaw);

      //fill in the results
      FillResults(recFree, resMap);

      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase2 = ee->AddPulseAnalysisRecord();
      fMTKamp.SetName("BlackForestKampSiteBaseline");
      SetTRefLinksForKAmpEvent(recBase2, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      resMap = fMTKamp.MakeBaseKamp(pRaw);

      //fill in the results
      FillResults(recBase2, resMap);

    }
  } 
  return true;
}  

void KBlackForestKAmpSite::FillResults(KPulseAnalysisRecord* rec, map<string, KResult> &resMap)
{
  if(resMap.find("amp") != resMap.end())              rec->SetAmp( resMap["amp"].fValue );
  if(resMap.find("peakPosition") != resMap.end())     rec->SetPeakPosition( resMap["peakPosition"].fValue );
  if(resMap.find("baselineRemoved") != resMap.end())  rec->SetBaselineRemoved( resMap["baselineRemoved"].fValue );
  if(resMap.find("slopeRemoved") != resMap.end())     rec->SetSlopeRemoved( resMap["slopeRemoved"].fValue );
  if(resMap.find("trapDecayTime") != resMap.end())    rec->SetExtra( resMap["trapDecayTime"].fValue, 0 );
  if(resMap.find("trapFlatTopWidth") != resMap.end()) rec->SetExtra( resMap["trapFlatTopWidth"].fValue, 1 );
  if(resMap.find("trapRiseTime") != resMap.end())     rec->SetExtra( resMap["trapRiseTime"].fValue, 2 );
  if(resMap.find("trapDecayTime2") != resMap.end())   rec->SetExtra( resMap["trapDecayTime2"].fValue, 3 );
  if(resMap.find("numPeaks") != resMap.end())         rec->SetExtra( resMap["numPeaks"].fValue, 4 );

  for (unsigned int i = 0; i < 8; i=i+2){
    string name_index = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
    string namePos = "peak_" + name_index;
    string nameCoef = "coef_" + name_index;
    if(resMap.find(namePos.c_str()) != resMap.end())  rec->SetExtra( resMap[nameCoef.c_str()].fValue, i+5 );
    if(resMap.find(nameCoef.c_str()) != resMap.end()) rec->SetExtra( resMap[nameCoef.c_str()].fValue, i+6 );
  }

  if(resMap.find("pileUpFound") != resMap.end())      rec->SetExtra( resMap["pileUpFound"].fValue, 13 );

  rec->SetName(fMTKamp.GetName());
  rec->SetUnit(0); 
}

Bool_t KBlackForestKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  return true;
}  
