//_____________________________________________
// KLibertyBellKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KTrapKamperProto to estimate the peak position and amplitude of the pulse. 
// It also uses the KBBv2TimeDomainFitKamper for bbv2 ionization channels.
//
// This KAmpSite is still under construction.... 
//
//
// Heat Pulse and BBv1 ionization channel results - using a trapezoidal filter
// if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
// if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
// if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
// if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);
//
// //I've started on Extra[][3] since there is room in the data structure to do this, and because
// //they won't collide with Extra parameters filled in by the bbv2 fitter.
// if(resMap.find("trapDecayTime") != resMap.end())          rec->SetExtra(resMap["trapDecayTime"].fValue, 3);
// if(resMap.find("trapRiseTime") != resMap.end())           rec->SetExtra(resMap["trapRiseTime"].fValue, 4);
// if(resMap.find("trapFlatTopWidth") != resMap.end())       rec->SetExtra(resMap["trapFlatTopWidth"].fValue, 5);
// if(resMap.find("rmsPreProc") != resMap.end())             rec->SetExtra(resMap["rmsPreProc"].fValue, 6);
// if(resMap.find("rmsPostProc") != resMap.end())            rec->SetExtra(resMap["rmsPostProc"].fValue, 7);
//
// 
// Ionization channel results if using a BBv2 channel -- uses the KBBv2TimeDomainFitKamper
//
// if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
// if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
// if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
// if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);
// if(resMap.find("chiSq") != resMap.end())                  rec->SetChiSq(resMap["chiSq"].fValue);   
//
// if(resMap.find("baselineAmp") != resMap.end())            rec->SetExtra(resMap["baselineAmp"].fValue, 0);
// if(resMap.find("sharpness") != resMap.end())              rec->SetExtra(resMap["sharpness"].fValue, 1);
// if(resMap.find("fitStatus") != resMap.end())              rec->SetExtra(resMap["fitStatus"].fValue, 2);



#include "KLibertyBellKAmpSite.h"

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

KLibertyBellKAmpSite::KLibertyBellKAmpSite(void)
{
  SetName("KLibertyBellKAmpSite");
}

KLibertyBellKAmpSite::~KLibertyBellKAmpSite(void)
{

}

Bool_t KLibertyBellKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
 
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
    
    map<string, KResult> myResults;

    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    
    if(pRaw->GetIsHeatPulse() || pRaw->GetBoloBoxVersion() < 2.0) {
      myResults = fTrapKamp.MakeKamp(pRaw);
      FillTrapResults(rec, myResults);
    }
    else{
      myResults = fBBv2TimeFitter.MakeKamp(pRaw);
      FillTimeFitterResults(rec, myResults);

    }
    
    
    KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  

    if(pRaw->GetIsHeatPulse() || pRaw->GetBoloBoxVersion() < 2.0) {
      myResults = fTrapKamp.MakeBaseKamp(pRaw);
      FillTrapResults(rec, myResults);
    }
    else{
      myResults = fBBv2TimeFitter.MakeBaseKamp(pRaw);
      FillTimeFitterResults(rec, myResults);
    }
  }
  
  return true;
}  

void KLibertyBellKAmpSite::FillTrapResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap)
{
  if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
  if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
  if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
  if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);

  //I've started on Extra[][3] since there is room in the data structure to do this, and because
  //they won't collide with Extra parameters filled in by the bbv2 fitter.
  if(resMap.find("trapDecayTime") != resMap.end())          rec->SetExtra(resMap["trapDecayTime"].fValue, 3);
  if(resMap.find("trapRiseTime") != resMap.end())           rec->SetExtra(resMap["trapRiseTime"].fValue, 4);
  if(resMap.find("trapFlatTopWidth") != resMap.end())       rec->SetExtra(resMap["trapFlatTopWidth"].fValue, 5);
  if(resMap.find("rmsPreProc") != resMap.end())             rec->SetExtra(resMap["rmsPreProc"].fValue, 6);
  if(resMap.find("rmsPostProc") != resMap.end())            rec->SetExtra(resMap["rmsPostProc"].fValue, 7);

  rec->SetName(GetName());  //rename this pulse analysis record to this kampsite
}

void KLibertyBellKAmpSite::FillTimeFitterResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap)
{


  if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
  if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
  if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
  if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);
  if(resMap.find("chiSq") != resMap.end())                  rec->SetChiSq(resMap["chiSq"].fValue); 

  if(resMap.find("baselineAmp") != resMap.end())            rec->SetExtra(resMap["baselineAmp"].fValue, 0);
  if(resMap.find("sharpness") != resMap.end())              rec->SetExtra(resMap["sharpness"].fValue, 1);
  if(resMap.find("fitStatus") != resMap.end())              rec->SetExtra(resMap["fitStatus"].fValue, 2);
  

  rec->SetName(GetName());  //rename this pulse analysis record to this kampsite
}



Bool_t KLibertyBellKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //just returns true;  no scouting required 
  return true;
}  
