//
// KChamonixKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This KampSite uses the KSimpleKamper1 to estimate the peak position and amplitude of the pulse. 
// It does nothing fancy at the moment and serves as a nice example. 
// You can see in the RunKampSite method that this KampSite adds a KPulseAnalysisRecord to the KAmpEvent
// and passes it to the KSimpleKamper1 to fill its data. It also sets the TRef links. 

#include "KChamonixKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include <algorithm>

using namespace std;

KChamonixKAmpSite::KChamonixKAmpSite(void)
{
  SetName("KChamonixKAmpSite");
  
  fIonPeakDetector.AddFilteringProcess(100, 0, 0.7,  1,  1, false,   true, false);
  fIonPeakDetector.AddFilteringProcess(50, 0, 0.7, 1, 1, false, true, false);
  fIonPeakDetector.AddFilteringProcess(10, 0, 1.0, 1, 0, false, true,  true);
  fIonPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true,  true);
  
  fMultipleHeatPeakDetector.AddFilteringProcess(20, 0, 0.5,  1,  1, true,   true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(10, 0, 0.5, 1, 1, false, true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true, true);
  
  
}

KChamonixKAmpSite::~KChamonixKAmpSite(void)
{

}

Bool_t KChamonixKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  
  
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){

    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
    
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    fOptKamper.MakeKamp(pRaw, rec);
    
    
  }
  
  
  return true;
}  

Bool_t KChamonixKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  //run through the data, look for noise events, find their power spectra and then find the average
  //noise power. use these noise spectra for the optimal filter.
  //
  return true;
}  
vector<double> KChamonixKAmpSite::GetTemplatePower(const char* channelName) const;
{
  if (fTemplateSpectra.find(channelName) != fTemplateSpectra.end()){
    return fTemplateSpectra.find(channelName)->second;
  }
  else return vector<double>;  //just return an empty vector
    
}

double KChamonixKAmpSite::GetTrapDecayConstant(const char* channelName) const;
{
  if (fDecayValues.find(channelName) != fDecayValues.end()){
    return fDecayValues.find(channelName)->second;
  }
  else return -1.0;  
    
}

void KChamonixKAmpSite::SetTemplate(const char* channelName, const std::vector<double>& pulse);
{
  fR2Hc.SetInputPulse(pulse);
  fR2Hc.RunProcess();
  fHc2P.SetInputPulse(fR2Hc.GetOutputPulse(), fR2Hc.GetOutputPulseSize());
  fHc2P.RunProcess();
  vector<double> power;
  copy(fHc2P.GetOutputPulse(), fHc2P.GetOutputPulseSize(), power.begin());
  fTemplateSpectra[channelName] = power;
}


