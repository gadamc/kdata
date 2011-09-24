//
// KBackyardKampSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulse. 
// It does nothing fancy at the moment and serves as a nice example. 
// You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
// and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links. 

#include "KBackyardKampSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

KBackyardKampSite::KBackyardKampSite(void)
{

}

KBackyardKampSite::~KBackyardKampSite(void)
{

}

Bool_t KBackyardKampSite::RunKampSite(KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw)
{
  // This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulse. 
  // It does nothing fancy at the moment and serves as a nice example. 
  // You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
  // and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links.
  // 
  
  KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
  
  SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
  
  //use the KSimpleKamper to fill the KPulseAnalysisRecord
  fSimpKamp1.MakeKamp(pRaw, rec);
  
  return true;
}  

Bool_t KBackyardKampSite::ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e)
{
  //just returns true;  no scouting required -- its my freakin' backyard!
  return true;
}  
