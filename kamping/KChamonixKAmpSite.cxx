//
// KChamonixKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
 

#include "KChamonixKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KWindowDesign.h"
#include "KOptimalFilter.h"
#include <algorithm>
#include <iostream>

using namespace std;

KChamonixKAmpSite::KChamonixKAmpSite(void): fPulseTemplateShifter(0,0,0,0)  //set the fPulseTemplateShifter pulse locations to NULL so we set them later.
{
  SetName("KChamonixKAmpSite");
  
  
  fHeatWindow.SetWindow(KWindowDesign::GetTukeyWindow(512,0.7), 512);
  fHeatPeakDetector.SetOrder(5);
  fHeatPeakDetector.SetNumRms(2.3);
}

KChamonixKAmpSite::~KChamonixKAmpSite(void)
{

}

Bool_t KChamonixKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  //cout << "running a kampsite" << endl;
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    if(pRaw->GetPulseLength() == 0) continue;
    if(!pRaw->GetIsHeatPulse()) continue;
    
    if(fTemplateSpectra.find(pRaw->GetChannelName()) == fTemplateSpectra.end()) {
      //cout << "no template! " << pRaw->GetChannelName() << endl;
      continue;
    }; //we don't have a template spectra for this channel. skip it.
    if(fNoiseSpectra.find(pRaw->GetChannelName()) == fNoiseSpectra.end()) {
      //cout << "no noise spectra! " << pRaw->GetChannelName() << endl;
      continue;
    };  //also skip if we don't have a noise spectr
    
    
    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    
    KOptimalFilter& filter = fOptKamper.GetOptimalFilter();
    
    filter.SetTemplateDFT(fTemplateSpectra.find(pRaw->GetChannelName())->second);
    filter.SetNoiseSpectrum(fNoiseSpectra.find(pRaw->GetChannelName())->second);
    filter.SetToRecalculate(); //tell the filter to recalculate its kernel
    
    fOptKamper.SetWindow(&fHeatWindow); //tell the optimal filter kamper to use this window function.
    fOptKamper.SetPreProcessor(&fBaselineRemovalHeat);
    //need to tell the Kamper about the template pulse shift.
    fOptKamper.SetPulseTemplateShiftFromPreTrigger(fPulseTemplateShifter.GetShift());
    map<string, KResult> myResults = fOptKamper.MakeKamp(pRaw);
    

    //fill in the KPulseAnalysisRecord with myResults

    
    
    //KPulseAnalysisRecord *basRec = ee->AddPulseAnalysisRecord();
    //SetTRefLinksForKAmpEvent(basRec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
    //we don't really need to call this! The answer should already be calculated for us! Just get the optimal filter from
    //the fOptKamper and get the results from the output
    
    fHeatPeakDetector.SetInputPulse( &fHeatWindow);
    if(!fHeatPeakDetector.RunProcess())
      cout << "KChamonixKAmpSite::RunKampSite. fHeatPeakDetector fail"<< endl;
    else{
      rec->SetExtra(fHeatPeakDetector.GetPeakBins().size(), 6); 
      //basRec->SetExtra(fHeatPeakDetector.GetPeakBins().size(), 6); //
    }
    
    //fOptKamper.MakeBaseKamp(pRaw, basRec);
    //basRec->SetIsBaseline(true);
    rec->SetName(GetName());
    //basRec->SetName(GetName());
  }
  
  
  return true;
}  

Bool_t KChamonixKAmpSite::ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent* /*e*/)
{
  //run through the data, look for noise events, find their power spectra and then find the average
  //noise power. use these noise spectra for the optimal filter.
  //
  if(pRaw->GetPulseLength() == 0) return false;
  if(!pRaw->GetIsHeatPulse()) return false;
  
  // double decayVal = GetTrapDecayConstant(pRaw->GetChannelName());
  //  d if (decayVal < 0) return false;
  
  fBaselineRemovalHeat.SetInputPulse( (std::vector<short> &)pRaw->GetTrace());
  if(!fBaselineRemovalHeat.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fBaselineRemovalHeat failed" << endl;
    return false;
  }
  
  //should replace the following with the KOrderFilter and then look for noise
  //have to figure out how to effectively find noise events using the ionization channel
  //but could also talk to Benjamin Censier on how he does this work and how he analyzes
  //the events. 
  // fHeatPeakDetector.SetDecayTimeConstant(decayVal);
  //   fHeatPeakDetector.SetPolarity(-1);
  //   fHeatPeakDetector.SetInputPulse(fBaselineRemovalHeat.GetOutputPulse(), fBaselineRemovalHeat.GetOutputPulseSize());
  //   if( !fHeatPeakDetector.RunProcess()){
  //     cout << "KChamonixKAmpSite::ScoutKampSite. fHeatPeakDetector failed" << endl;
  //     return false;
  //   }
  //   std::vector< std::vector<double> > foundPeaks = fHeatPeakDetector.GetRemainingPeaks();
  //   if(foundPeaks.size() > 0) return false; //we found a peak, so this is not noise.
  
  
  fHeatPeakDetector.SetPolarity(-1);
  fHeatPeakDetector.SetInputPulse(&fBaselineRemovalHeat);
  if( !fHeatPeakDetector.RunProcess()){
      cout << "KChamonixKAmpSite::ScoutKampSite. fHeatPeakDetector failed" << endl;
      return false;
  }
  if(fHeatPeakDetector.GetPeakBins().size() > 0) return false; //we found a peak, so this is not noise.
      
  //we found noise, now window it, find its power spectrum and then add it to the running average power spectrum  
  fHeatWindow.SetInputPulse(&fBaselineRemovalHeat);
  if(!fHeatWindow.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fHeatWindow failed" << endl; return false;
  }

  fR2Hc.SetInputPulse(&fHeatWindow);
  if(!fR2Hc.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fR2Hc failed" << endl; return false;
  }
  fHc2P.SetInputPulse(&fR2Hc);
  if(!fHc2P.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fHc2P failed" << endl; return false;
  }
  
  if(fHc2P.GetOutputPulse() == 0){
    cout << "KChamonixKAmpSite::ScoutKampSite. fHc2P null pointer" << endl; return false;
  }
  
  if(fNoiseEventCounts.find(pRaw->GetChannelName()) == fNoiseEventCounts.end()){
    fNoiseEventCounts[pRaw->GetChannelName()] = 1;
  }
  else fNoiseEventCounts[pRaw->GetChannelName()]++;
    
  //I'm assuming that the length of the pulse will never change during a data file!
  //if the map doesn't find a vector for this channel,then we create one
  if(fNoiseSpectra.find(pRaw->GetChannelName()) == fNoiseSpectra.end()){
    vector<double> power;
    power.resize(fHc2P.GetOutputPulseSize());
    std::copy(fHc2P.GetOutputPulse(), fHc2P.GetOutputPulse()+fHc2P.GetOutputPulseSize(), power.begin());
    fNoiseSpectra[pRaw->GetChannelName()] = power;
  }
  else{ //add noise spectrum to the running average
    for(unsigned int i = 0; i < fNoiseSpectra[pRaw->GetChannelName()].size(); i++ ){
      fNoiseSpectra[pRaw->GetChannelName()][i] = fNoiseSpectra[pRaw->GetChannelName()][i] * (fNoiseEventCounts[pRaw->GetChannelName()]-1.0)/fNoiseEventCounts[pRaw->GetChannelName()] + fHc2P.GetOutputPulse()[i]/fNoiseEventCounts[pRaw->GetChannelName()];
    }
  }

  return true;
}  

//
//
//Get and Set template and decay constant parameters
//
//
//
Bool_t KChamonixKAmpSite::SetTemplate(const char* channelName,  std::vector<double>& pulse)
{
  fHeatWindow.SetInputPulse(pulse);
  if(!fHeatWindow.RunProcess()){
    cout << "fHeatWindow failed" << endl; return false;
  }
    
  fPulseTemplateShifter.SetInputPulse(fHeatWindow.GetOutputPulse());
  fPulseTemplateShifter.SetInputPulseSize(fHeatWindow.GetOutputPulseSize());
  fPulseTemplateShifter.SetOutputPulse(fHeatWindow.GetOutputPulse());
  fPulseTemplateShifter.SetOutputPulseSize(fHeatWindow.GetOutputPulseSize());
  fPulseTemplateShifter.SetMode(2);
  fPulseTemplateShifter.RunProcess();
  
  fR2Hc.SetInputPulse( &fHeatWindow);
  if(!fR2Hc.RunProcess()){
    cout << "fR2Hc failed" << endl; return false;
  }
  
  if(fR2Hc.GetOutputPulse() == 0){
    cout << "fR2Hc null pointer" << endl; return false;
  }
  
  vector<double> dft;
  dft.resize(fR2Hc.GetOutputPulseSize());
  
  std::copy(fR2Hc.GetOutputPulse(), fR2Hc.GetOutputPulse()+fR2Hc.GetOutputPulseSize(), dft.begin());
  fTemplateSpectra[channelName] = dft;
  return true;
}

vector<double> KChamonixKAmpSite::GetTemplateSpectrum(const char* channelName) const
{
  if (fTemplateSpectra.find(channelName) != fTemplateSpectra.end()){
    return fTemplateSpectra.find(channelName)->second;
  }
  else {
    vector<double> empty;  //just return an empty vector
    return empty;
  }
    
}

double KChamonixKAmpSite::GetTrapDecayConstant(const char* channelName) const
{
  if (fDecayValues.find(channelName) != fDecayValues.end()){
    return fDecayValues.find(channelName)->second;
  }
  else return -1.0;  
    
}

unsigned int KChamonixKAmpSite::GetNumNoiseEventsFound(const char* channelName) const
{
  if (fNoiseEventCounts.find(channelName) != fNoiseEventCounts.end()){
    return fNoiseEventCounts.find(channelName)->second;
  }
  else return 0;
}

vector<double> KChamonixKAmpSite::GetNoisePower(const char* channelName) const
{
  if (fNoiseSpectra.find(channelName) != fNoiseSpectra.end()){
    return fNoiseSpectra.find(channelName)->second;
  }
  else {
    vector<double> empty;  //just return an empty vector
    return empty;
  }
    
}

void KChamonixKAmpSite::SetNoisePower(const char* channelName, vector<double> power)
{
  fNoiseSpectra[channelName] = power;
}

