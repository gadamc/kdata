//_____________________________________________
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
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

using namespace std;

KBackyardKampSite::KBackyardKampSite(void)
{
  SetName("KBackyardKampSite");
  SetTitle("KBackyardKampSite");
}

KBackyardKampSite::~KBackyardKampSite(void)
{

}

Bool_t KBackyardKampSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  // This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulses in a bolometer record. 
  // It does nothing fancy at the moment and serves as a nice example. 
  // You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
  // and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links.
  // 
  
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){

    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
    
    //Create a new analysis record to hold your results
    //and set up the necessary TRef Links for the KAmpEvent
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    
    //run the kamper and grab the results
    map<string, KResult> resMap = fSimpKamp1.MakeKamp(pRaw);
    
    //fill the KPulseAnalysisRecord with the data found in the resMap
    //In order to know what is in the resMap, the author the KAmper must
    //document the results in their code! (Otherwise, you'll have to go and
    //read the details of the MakeKamp method.)

    //this code looks annoying... wait until c++11, then we'll get Lambda functions
    //which will simplify all of this... gcc v4.5 or greater is needed for Lambda functions
    //this code would compile without the if statements, but if the map object didnt'
    //exit, a new one would be created automatically. So, the if statements here
    //check for existence first and only set values in the KPulseAnalysisRecord if they are there. 

    if(resMap.find("amp") != resMap.end()) rec->SetAmp(resMap["amp"].fValue);

    if(resMap.find("peakPosition") != resMap.end()) rec->SetPeakPosition(resMap["peakPosition"].fValue);

    if(resMap.find("baselineRemoved") != resMap.end()) rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);

    if(resMap.find("slopeRemoved") != resMap.end()) rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);

    rec->SetName(fSimpKamp1.GetName());
    rec->SetUnit(0);

    //an alternative to the code above is a new method being constructed in the KAmpSite base class
    //however, this method isn't yet ready because of problems incorporating it into the ROOT dictionary code.
    //
    //fill(rec, &KPulseAnalysisRecord::SetAmp, resMap, "amp");



    //if there were more kampers in your kampsite, you could use them to estimate pulse amplitudes
    //and pack the results into appropriate KPulseAnalysisRecords
    //for example:
    //   KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    //   SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    //   map<string, KResults> complicatedResults = fComplicatedKamper.MakeKamp(pRaw);
    //
    
    //however, really you can hack the data anyway you want...
    
  }
  
  
  return true;
}  

Bool_t KBackyardKampSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //just returns true;  no scouting required -- its my freakin' backyard!
  return true;
}  
