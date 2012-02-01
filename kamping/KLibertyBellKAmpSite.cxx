//
// KLibertyBellKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KTrapKamperProto to estimate the peak position and amplitude of the pulse. 
// It does nothing fancy at the moment and serves as a nice example. 
// You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
// and passes it to the KTrapKamperProto to fill its data. It also sets the TRef links. 

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
    
    
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    if(pRaw->GetIsHeatPulse())
      fTrapKamp.MakeKamp(pRaw, rec);
    else
      fBBv2TimeFitter.MakeKamp(pRaw, rec);
    rec->SetName(GetName());  //rename this pulse analysis record to this kampsite
    
    
    KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    if(pRaw->GetIsHeatPulse())
      fTrapKamp.MakeBaseKamp(pRaw, rec);
    else
      fBBv2TimeFitter.MakeBaseKamp(pRaw, rec);
    rec->SetName(GetName());  //rename this pulse analysis record to this kampsite
  }
  
  return true;
}  

Bool_t KLibertyBellKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //just returns true;  no scouting required 
  return true;
}  
