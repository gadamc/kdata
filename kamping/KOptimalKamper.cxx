//_____________________________________________
// KOptimalKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
// MakeKamp output map<string, KResult>
//
//  string                |   description
//  
//   amp                      amplitude from the optimal filter at the peak position
//   peakPosition             position in the optimal filter output
//   chi2AtPeakPosition       the chi^2 value calculated by the optimal filter at peakPosition
//   baselineRemoved          the amplitude of the baseline that was subtracted from the raw pulse before optimal filter. this is the DC offset of the raw pulse
//                              this value is filled only if the fPreProcessor object was a KPulseAnalysisChain that contained
//                              either a KBaselineRemoval or KLinearRemoval object
//   fixPeakPosition          the value of fixPeakPosition that was passed into MakeKamp (when fixPeakPosition > 0, then peakPosition == fixPeakPosition)
//                     
//                            If fixPeakPosition < 0 (meaning it was ignored), then the following results will be fund in the output map
//
//   pulseTemplateShift       the time shift of the pulse template
//   amplitudeShift           an extra time shift used to measure the baseline amplitude
//   ampAtTemplatePluseAmplitudeShift   the amplitude estimated by the optimal filter at the pulseTemplateShift+amplitudeShift position
//   minChi2                  the minimum value of the chi^2 as a function of pulse start time
//   minChi2Pos               the time(bin number) of the pulse where chi^2 is at a minimum
//   optAmpAtMinChi2          amplitude from the optimal filter at minChi2Pos
//   pulseAmpAtOptimalAmpPeakPosition   amplitude of the preProcessed+Windowed pulse at peakPosition
//   pulseAmpAtMinChi2Position          amplitude of the preProcessed+Windowed pulse at minChi2Pos
//   risetime                 crude estimate of the 10-90% risetime
//   pulsewidth               crude estimate of the 10% - 10% pulse width


#include "KOptimalKamper.h"
#include "KBaselineRemoval.h"
#include "KLinearRemoval.h"
#include "KWindow.h"
#include "KPulseAnalysisRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KResult.h"
#include <typeinfo>
#include <exception> 
#include <iostream>

using namespace std;

KOptimalKamper::KOptimalKamper(void)
{

  fPulseTemplateShift = 0;
  fAmplitudeEstimatorSearchRange = 0.10;
  fPulseAmplitudeShift = 10;
  
}

KOptimalKamper::~KOptimalKamper(void)
{

}

std::map<std::string, KResult> KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord)
{
  return MakeKamp(rawPulseRecord, -1);
}

std::map<std::string, KResult> KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, double fixPeakPosition)
{
  //make sure that you set up the Optimal Filter properly before each event. This means you have to 
  //set the optimal filter's noise and template power spectra before MakeKamp gets called.
  //Do this inside of the KAmpSite that is using this KAmper. 
  //
  //if the rawPulseRecord->GetPulseLength() == 0 
  //this kamper returns an empty map

  map<string, KResult> myResults;

  if(rawPulseRecord->GetPulseLength() == 0) return myResults;
  //if(!rawPulseRecord->GetIsHeatPulse()) return myResults;
  
  
  KPtaProcessor *last = 0;
  if(fPreProcessor){
    fPreProcessor->SetInputPulse( (std::vector<short>&)rawPulseRecord->GetTrace());
    if(!fPreProcessor->RunProcess()){
      cout << "fPreProcessor failed" << endl; return myResults;
    }
    last = fPreProcessor;
  }
  if (fWindow){
    if(fPreProcessor)
      fWindow->SetInputPulse(fPreProcessor);
    else
      fWindow->SetInputPulse((std::vector<short>&)rawPulseRecord->GetTrace());
    
    if(!fWindow->RunProcess()){
      cout << "fWindow failed" << endl; return myResults;
    }
    last = fWindow;
  }
  
  
  //have to pass in the fourier transformed pulse into the optimal filter
  if(last) fR2Hc.SetInputPulse(last);
  else fR2Hc.SetInputPulse((std::vector<short>&)rawPulseRecord->GetTrace());
  
  if(!fR2Hc.RunProcess()){
    cout << "KOptimalKamper::MakeKamp. fR2Hc failed" << endl; return myResults;
  }
  
  fOptimalFilter.SetInputPulse( &fR2Hc);
  
  if(!fOptimalFilter.RunProcess()){
    cout << "KOptimalKamper::MakeKamp. fOptimalFilter failed" << endl; return myResults;
  }
  
  //what is this: just a test to see what is the amplitude of the pulse at this point in time.
  myResults["ampAtTemplatePluseAmplitudeShift"] = KResult("ampAtTemplatePluseAmplitudeShift", fOptimalFilter.GetOutputPulse()[(int)fPulseTemplateShift + fPulseAmplitudeShift], "ADU");
  myResults["pulseTemplateShift"]  = KResult("pulseTemplateShift", fPulseTemplateShift, "bin");
  myResults["amplitudeShift"] = KResult("amplitudeShift", fPulseAmplitudeShift, "bin");

  //rec->SetExtra(fOptimalFilter.GetOutputPulse()[(int)fPulseTemplateShift + fPulseAmplitudeShift], 0);

  
  //find the maximum value in the optimal filter
  double maxValue = fOptimalFilter.GetOutputPulse()[0];
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
    myResults["minChi2"] = KResult("minChi2", minChi2);
    myResults["minChi2Pos"] = KResult("minChi2Pos", minChi2Pos, "bin");
    myResults["optAmpAtMinChi2"] = KResult("optAmpAtMinChi2", fOptimalFilter.GetOutputPulse()[minChi2Pos], "ADU");

    if(last){
      myResults["pulseAmpAtOptimalAmpPeakPosition"] = KResult("pulseAmpAtOptimalAmpPeakPosition", last->GetOutputPulse()[maxPosition], "ADU");
      myResults["pulseAmpAtMinChi2Position"] = KResult("pulseAmpAtMinChi2Position", last->GetOutputPulse()[minChi2Pos], "ADU");
    
    
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
      myResults["risetime"] = KResult("risetime", (ninetyPercTime - tenPercStartTime)*rawPulseRecord->GetPulseTimeWidth()*1.0e-9, "seconds");
      myResults["pulseWidth"] = KResult("pulseWidth", (tenPercEndTime - tenPercStartTime)*rawPulseRecord->GetPulseTimeWidth()*1.0e-9, "seconds");
    }
  }
  else if (fixPeakPosition >= 0 && fixPeakPosition < fOptimalFilter.GetOutputPulseSize()){  //use the position specificed by the caller
    maxValue = fOptimalFilter.GetOutputPulse()[(int)fixPeakPosition];
    maxPosition = (int)fixPeakPosition;
  }
  myResults["amp"] = KResult("amp", maxValue, "ADU");
  myResults["peakPosition"] = KResult("peakPosition", maxPosition, "bin");
  myResults["chi2AtPeakPosition"] = KResult("chi2AtPeakPosition", fOptimalFilter.GetChiSquared(maxPosition));
  myResults["fixPeakPosition"] = KResult("fixPeakPosition", fixPeakPosition, "bin");

  
  
  try{
    if( dynamic_cast<KBaselineRemoval*>(fPreProcessor) != 0)
      myResults["baselineRemoved"] = KResult("baselineRemoved", (dynamic_cast<KBaselineRemoval*>(fPreProcessor))->GetBaselineOffset(), "ADU");
      
    else if( dynamic_cast<KLinearRemoval*>(fPreProcessor) != 0)
      myResults["baselineRemoved"] = KResult("baselineRemoved", (dynamic_cast<KLinearRemoval*>(fPreProcessor))->GetOffset(), "ADU");
  }
  catch (exception& e) {
    cout << "Exception: " << e.what() << endl;
  }  
  
  return myResults;
}

