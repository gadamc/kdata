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
#include "TString.h"
//#include <math>

using namespace std;
// 
// typedef struct pulseId{
//   string aName;
//   bool isHeat;
// };
// 
// struct classcomp {
//   bool operator() (const pulseId& lhs, const pulseId& rhs) const
//   { 
//     if (lhs.isHeat < rhs.isHeat) return true;
//     else if(lhs.aName < rhs.aName) return true;
//     else return false;
//   }
// };

KGrandCanyonKAmpSite::KGrandCanyonKAmpSite(void)
{
  SetName("KGrandCanyonKAmpSite");
  //fTrapKamp.SetDebugMode();
  
  // fTrapKampHeat = new KTrapKamperProto();
  //   
  //   for(int i = 0; i < 6; i++){ //allocate space for six different ionization channels
  //     KTrapKamperProto *i = new KTrapKampterProto();
  //     TString mName = "KTrapKamperProto_Ion_" + i;
  //     i->SetName(mName.Data());
  //     fIonTraps.push_back(i);
  //   }
  //   
}

KGrandCanyonKAmpSite::~KGrandCanyonKAmpSite(void)
{

}

Bool_t KGrandCanyonKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  //
  
  //
    // 
    //   
    // //map<pulseId, KPulseAnalysisRecord*, classcomp> mFirstPass;
    // vector<KPulseAnalysisRecord* > mFirstPass;
    // 
    // //loop through the pulses in the event and get the first results from applying the trapezoidal filter.
    // for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    // 
    //   KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    //   //pulseId mPulseId;
    //   //mPulseId.aName = pRaw->GetChannelName();
    //   //mPulseId.isHeat = pRaw->GetIsHeatPulse();
    //   
    //   KPulseAnalysisRecord *rec  =  new KPulseAnalysisRecord();  //don't need to clean this up because the map destructor calls the destructor for you
    //   
    //   fTrapKamp.MakeKamp(pRaw, rec);
    //   mFirstPass.push_back(rec);
    //   
    // }    
    // 
    // //from the ionization pulses, get the pulse start time from the pulse with the greatest amplitude
    // //map<pulseId, KPulseAnalysisRecord*, classcomp>::iterator it;
    // 
    // unsigned long posOfMaxPeak;
    // double maxPeak = -1;
    // int boloRecordNum = -1;
    // 
    // for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    //   KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    //   
    //   if (pRaw->GetIsHeatPulse() == false){
    //     KPulseAnalysisRecord *rec = mFirstPass[k];
    //     
    //     if (fabs(rec->GetAmp()) > maxPeak &&
    //         pRaw->GetPulseTimeWidth()(rec->GetPeakPosition() - pRaw->GetPretriggerSize()) > 0.0 &&  //greater than the start time of the pulse
    //         pRaw->GetPulseTimeWidth()(rec->GetPeakPosition() - pRaw->GetPretriggerSize()) < 7000000.0  //the ion trigger position must be less than 7ms from the trigger position
    //       maxPeak = fabs(rec->GetAmp());
    //       posOfMaxPeak = rec->GetPeakPosition();
    //       boloRecordNum = k;
    //     }
    //   } 
    // }
    // 
    // //then take that largest pulse time and determine the value of the bin in the heat channel
    // double heatPulsePosition = -1;  //initialize to -1
    // //first, we restrict that the 
    // 
  //now loop through the pulses again, but this time when you find the heat pulse, force it to use the pulse
  //time that has been calculated.
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
