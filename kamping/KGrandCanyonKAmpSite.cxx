//_____________________________________________
// KGrandCanyonKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KTrapKamperProto to estimate the peak position and amplitude of the pulse. 
// It does nothing fancy at the moment and serves as a nice example. 
// You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
// and passes it to the KTrapKamperProto to fill its data. It also sets the TRef links. 

#include "KGrandCanyonKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "TString.h"
//#include <math>

using namespace std;

KGrandCanyonKAmpSite::KGrandCanyonKAmpSite(void)
{
  SetName("KGrandCanyonKAmpSite");
  SetTitle("KGrandCanyonKAmpSite");
}

KGrandCanyonKAmpSite::~KGrandCanyonKAmpSite(void)
{

}

Bool_t KGrandCanyonKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
 
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we get a bolo amp pulse record
    

    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    map<string, KResult> myResults = fTrapKamp.MakeKamp(pRaw);
    FillResults(rec, myResults);
    
    
    //baseline estimates
    KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  
    
    myResults = fTrapKamp.MakeBaseKamp(pRaw);  //MakeBaseKamp performs estimates of the baseline 
    FillResults(recBase, myResults);

    string basName = GetName();
    basName += "Base";
    rec->SetName(basName.c_str());
  }
  
  return true;
}  

void KGrandCanyonKAmpSite::FillResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap)
{
  if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
  if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
  if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
  if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);

  if(resMap.find("trapDecayTime") != resMap.end())               rec->SetExtra(resMap["trapDecayTime"].fValue, 0);
  if(resMap.find("trapRiseTime") != resMap.end())              rec->SetExtra(resMap["trapRiseTime"].fValue, 1);
  if(resMap.find("trapFlatTopWidth") != resMap.end())    rec->SetExtra(resMap["trapFlatTopWidth"].fValue, 2);
  if(resMap.find("rmsPreProc") != resMap.end())             rec->SetExtra(resMap["rmsPreProc"].fValue, 3);
  if(resMap.find("rmsPostProc") != resMap.end())            rec->SetExtra(resMap["rmsPostProc"].fValue, 4);

  rec->SetName(GetName());  //rename this pulse analysis record to this kampsite

}

Bool_t KGrandCanyonKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //just returns true;  no scouting required 
  return true;
}  
