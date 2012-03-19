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
  rec->SetExtra(fOptimalFilter.GetOutputPulse()[rawPulseRecord->GetPretriggerSize()+2], 0);
  
  //find the maximum value in the optimal filter
  double maxValue= fOptimalFilter.GetOutputPulse()[0];
  int maxPosition = 0;
  
  if(fixPeakPosition < 0){  //search for it
    for(UInt_t i = 0; i < (UInt_t)(fOptimalFilter.GetOutputPulseSize()*0.10); i++){ //hack - just put in first 50 bins for each pulse
      if (fOptimalFilter.GetOutputPulse()[i] > maxValue) {
        maxValue = fOptimalFilter.GetOutputPulse()[i];
        maxPosition = i;
        
      }
    }    
    double minChi2 = fOptimalFilter.GetChiSquared(0);
    double temp;
    double minChi2Pos = 0;
    for(UInt_t i = 0; i < (UInt_t)(fOptimalFilter.GetOutputPulseSize()*0.10); i++){ //hack - just put in first 50 bins for each pulse
      temp = fOptimalFilter.GetChiSquared(i); 
      if (temp < minChi2) {
        minChi2 = temp;
        minChi2Pos = i;
        
      }
    }
    rec->SetExtra(minChi2, 1);
    rec->SetExtra(minChi2Pos, 2);
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
  return MakeKamp(rawPulseRecord, rec, 200);
  rec->SetIsBaseline(true);
  rec->SetUnit(0);
}
