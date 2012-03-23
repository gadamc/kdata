//
// KOptimalKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KOptimalKamper.h"
#include "KBaselineRemoval.h"
#include "KLinearRemoval.h"
#include "KWindow.h"
#include "KPulseAnalysisRecord.h"
#include "KRawBoloPulseRecord.h"
#include <typeinfo>
#include <exception> 
#include <iostream>

using namespace std;

KOptimalKamper::KOptimalKamper(void)
{
  fPulseTemplateShift = 0;
  fAmplitudeEstimatorSearchRange = 0.10;
  
}

KOptimalKamper::~KOptimalKamper(void)
{

}

Bool_t KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec)
{
  return MakeKamp(rawPulseRecord, rec, -1);
  rec->SetIsBaseline(false);
  rec->SetUnit(0);
}

Bool_t KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
  //make sure that you set up the Optimal Filter properly before each event. This means you have to 
  //set the optimal filter's noise and template power spectra before MakeKamp gets called.
  //Do this inside of the KAmpSite that is using this KAmper. 

  if(rawPulseRecord->GetPulseLength() == 0) return false;
  if(!rawPulseRecord->GetIsHeatPulse()) return false;
  
  
  KPtaProcessor *last = 0;
  if(fBaseRemove){
    fBaseRemove->SetInputPulse( (std::vector<short>&)rawPulseRecord->GetTrace());
    if(!fBaseRemove->RunProcess()){
      cout << "fBaseRemove failed" << endl; return false;
    }
    last = fBaseRemove;
  }
  if (fWindow){
    if(fBaseRemove)
      fWindow->SetInputPulse(fBaseRemove->GetOutputPulse(), fBaseRemove->GetOutputPulseSize());
    else
      fWindow->SetInputPulse((std::vector<short>&)rawPulseRecord->GetTrace());
    
    if(!fWindow->RunProcess()){
      cout << "fWindow failed" << endl; return false;
    }
    last = fWindow;
  }
  
  
  //have to pass in the fourier transformed pulse into the optimal filter
  if(last) fR2Hc.SetInputPulse(last->GetOutputPulse(), last->GetOutputPulseSize());
  else fR2Hc.SetInputPulse((std::vector<short>&)rawPulseRecord->GetTrace());
  
  if(!fR2Hc.RunProcess()){
    cout << "KOptimalKamper::MakeKamp. fR2Hc failed" << endl; return false;
  }
  
  fOptimalFilter.SetInputPulse(fR2Hc.GetOutputPulse(), fR2Hc.GetOutputPulseSize());
  
  if(!fOptimalFilter.RunProcess()){
    cout << "KOptimalKamper::MakeKamp. fOptimalFilter failed" << endl; return false;
  }
  
  //what is this: just a test to see what is the amplitude of the pulse at this point in time.
  rec->SetExtra(fOptimalFilter.GetOutputPulse()[(int)fPulseTemplateShift+5], 0);
  
  //find the maximum value in the optimal filter
  double maxValue= fOptimalFilter.GetOutputPulse()[0];
  int maxPosition = 0;
  
  if(fixPeakPosition < 0){  //search for it
    
    //figure out start and stop positions
    UInt_t start = fPulseTemplateShift - fOptimalFilter.GetOutputPulseSize()*fAmplitudeEstimatorSearchRange > 0 ? fPulseTemplateShift - fOptimalFilter.GetOutputPulseSize()*fAmplitudeEstimatorSearchRange : 0;
    UInt_t stop = fPulseTemplateShift + fOptimalFilter.GetOutputPulseSize()*fAmplitudeEstimatorSearchRange < fOptimalFilter.GetOutputPulseSize() ? fPulseTemplateShift + fOptimalFilter.GetOutputPulseSize()*fAmplitudeEstimatorSearchRange : fOptimalFilter.GetOutputPulseSize(); 
    
    //the amplitude estimator and the chi^2 should be cyclical in time, so, one could write the code
    //here to cycle through the beginning/end of the pulse trace to properly search for the start time of 
    //the pulse. However, at the moment, its easiest to shift the pulse template to a position where 
    //this code works as is. 
    for(UInt_t i = start; i < stop; i++){ 
      if (fOptimalFilter.GetOutputPulse()[i] > maxValue) {
        maxValue = fOptimalFilter.GetOutputPulse()[i];
        maxPosition = i;
        
      }
    }    
    double minChi2 = fOptimalFilter.GetChiSquared(0);
    double temp;
    int minChi2Pos = 0;
    for(UInt_t i = start; i < stop; i++){ 
      temp = fOptimalFilter.GetChiSquared(i); 
      if (temp < minChi2) {
        minChi2 = temp;
        minChi2Pos = i;
        
      }
    }
    rec->SetExtra(minChi2, 1);
    rec->SetExtra(minChi2Pos, 2);
    rec->SetExtra(fOptimalFilter.GetOutputPulse()[minChi2Pos], 3);
    rec->SetExtra(last->GetOutputPulse()[maxPosition], 4);
    rec->SetExtra(last->GetOutputPulse()[minChi2Pos], 5);
    
    //other pulse shape characteristics. rise time, width
    stop = fOptimalFilter.GetOutputPulseSize();
    double tenPerc = last->GetOutputPulse()[maxPosition]*0.10;
    double ninetyPerc = last->GetOutputPulse()[maxPosition]*0.90;
    int tenPercStartTime = start;
    int ninetyPercTime = start;
    int tenPercEndTime = stop;
    bool foundTenPercStart = false;
    bool foundninetyPercTime = false;
    for(UInt_t i = start+1; i < stop; i++){ 
      if(!foundTenPercStart){
        if(last->GetOutputPulse()[i] > tenPerc && last->GetOutputPulse()[i-1] < tenPerc){
          foundTenPercStart = true;
          tenPercStartTime = i;  
        }
      }
      else if(foundTenPercStart && foundninetyPercTime == false){
        if(last->GetOutputPulse()[i] > ninetyPerc && last->GetOutputPulse()[i-1] < ninetyPerc){
          foundninetyPercTime = true;
          ninetyPercTime = i;
        }
      }
      else if(foundTenPercStart && foundninetyPercTime){
        if(last->GetOutputPulse()[i] < tenPerc && last->GetOutputPulse()[i-1] < tenPerc){
          tenPercEndTime = i;
        }
      }
    }
    rec->SetRisetime( (ninetyPercTime - tenPercStartTime)*rawPulseRecord->GetPulseTimeWidth()*1.0e-9);
    rec->SetPulseWidth( (tenPercEndTime - tenPercStartTime)*rawPulseRecord->GetPulseTimeWidth()*1.0e-9);
    
  }
  else if (fixPeakPosition >= 0 && fixPeakPosition < fOptimalFilter.GetOutputPulseSize()){  //use the position specificed by the caller
    maxValue = fOptimalFilter.GetOutputPulse()[(int)fixPeakPosition];
    maxPosition = (int)fixPeakPosition;
  }
  
  rec->SetAmp(maxValue);
  rec->SetPeakPosition(maxPosition);
  rec->SetChiSq( fOptimalFilter.GetChiSquared(maxPosition) );
  
  
  try{
    if( dynamic_cast<KBaselineRemoval*>(fBaseRemove) != 0)
      rec->SetBaselineRemoved( (dynamic_cast<KBaselineRemoval*>(fBaseRemove))->GetBaselineOffset());
      
    else if( dynamic_cast<KLinearRemoval*>(fBaseRemove) != 0)
      rec->SetBaselineRemoved((dynamic_cast<KLinearRemoval*>(fBaseRemove))->GetOffset());
  }
  catch (exception& e) {
    cout << "Exception: " << e.what() << endl;
  }  
  
  return true;
}

Bool_t KOptimalKamper::MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec)
{
  return MakeKamp(rawPulseRecord, rec, 60);
  rec->SetIsBaseline(true);
  rec->SetUnit(0);
}
