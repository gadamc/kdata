//
// KGrandCanyonKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulse. 
// It does nothing fancy at the moment and serves as a nice example. 
// You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
// and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links. 

#include "KGrandCanyonKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

KGrandCanyonKAmpSite::KGrandCanyonKAmpSite(void)
{
  SetName("KGrandCanyonKAmpSite");
  //fTrapKamp.SetDebugMode();
}

KGrandCanyonKAmpSite::~KGrandCanyonKAmpSite(void)
{

}

Bool_t KGrandCanyonKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  // This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulses in a bolometer record. 
  // It does nothing fancy at the moment and serves as a nice example. 
  // You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
  // and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links.
  // 
  
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){

    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
    
    //use the KSimpleKamper to fill the KPulseAnalysisRecord
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    fTrapKamp.MakeKamp(pRaw, rec);
    
    //create a new KPulseAnalysisRecord to store the results from 
    //KTrapezoidalKamper::MakeBaseKamp, which will estimate the amplitude
    //of the baseline. 
    KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    fTrapKamp.MakeBaseKamp(pRaw, recBase);
    
    //if there were more kampers in your kampsite, you could use them to estimate pulse amplitudes
    //and pack the results into appropriate KPulseAnalysisRecords
    //for example:
    //   KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    //   SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    //   fComplicatedKamper.MakeKamp(pRaw, rec);
    //
    
    //however, really you can hack the data anyway you want...
    
  }
  
  
  return true;
}  

Bool_t KGrandCanyonKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //just returns true;  no scouting required -- its my freakin' backyard!
  return true;
}  
