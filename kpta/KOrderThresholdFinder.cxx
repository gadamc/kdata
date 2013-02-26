//______________________________________________________________________
//
// KOrderThresholdFinder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2013 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Implements the ERA style of peak detection, using a KOrderFilter to smooth the 
// pulse and then search the trace for any bin
// greater than some value fThreshold. 
// This processor applies the KOrderFilter to the inputPulse of this object
// and places the results in this object's outputPulse. Thus, the outputPulse of this processor is a "smoothed" 
// pulse. This should be considered if you want to place this processor within a KPulseAnalyisChain
// 
// You can SetPolarity( -1, +1, or 0). If set to 0, then this object searches for pulses 
// in both directions. otherwise, it only looks for pulses with the polarity you indicated.
//
// Developer note: This class was written after KEraPeakFinder and was essentially copied from that class.
// However, it is obvious that KEraPeakFinder should inherit this class and just set the threshold.
// This inheritance coding has not been done though because of time constraints. 
//

#include <iostream>
#include "KOrderThresholdFinder.h"

using namespace std;

//ClassImp(KOrderThresholdFinder);

KOrderThresholdFinder::KOrderThresholdFinder(void)
: fOrderFilter(0, 0, 0, 0)  //initialize the order filter with null pointers so that we can set the internal arrays at run-time
{
  SetName("KOrderThresholdFinder");
  InitializeMembers();
}

KOrderThresholdFinder::KOrderThresholdFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize), fOrderFilter(0, 0, 0, 0) //initialize the order filter with null pointers so that we can set the internal arrays at run-time
{
  SetName("KOrderThresholdFinder"); 
  InitializeMembers();
  
}

KOrderThresholdFinder::~KOrderThresholdFinder(void)
{

}

void KOrderThresholdFinder::InitializeMembers(void)
{
  //initialize members. 

  fOrderFilter.SetOrder(1);
  fThreshold = 1000.0; //in ADU  
  fPeakBins.reserve(100);
  fPolarity = 0;
}

bool KOrderThresholdFinder::RunProcess(void)
{
  
  if(SmoothPulse())
    return FindPeaks();
  else {
    return false;
  }

}

bool KOrderThresholdFinder::SmoothPulse(void)
{
  fOrderFilter.SetInputPulse(GetInputPulse());
  fOrderFilter.SetInputPulseSize(GetInputPulseSize());
  fOrderFilter.SetOutputPulse(GetOutputPulse());
  fOrderFilter.SetOutputPulseSize(GetOutputPulseSize());
  
  if(fOrderFilter.RunProcess()) return true;
  else return false;
}

bool KOrderThresholdFinder::FindPeaks(void)
{
      
  fPeakBins.erase(fPeakBins.begin(), fPeakBins.end());  //this should, from what I understand, keep the memory allocated, which would be faster than deallocation and reallocation for each pulse
  
  
  if (fPolarity == 0){
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if( abs(GetOutputPulse()[i]) >= fThreshold )
        fPeakBins.push_back(i);
    }
  }
  else if (fPolarity > 0){
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if (GetOutputPulse()[i] > fThreshold)
        fPeakBins.push_back(i);
    }
  }
  else {
    int negativeThreshold = -1.*fThreshold;
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if (GetOutputPulse()[i] < negativeThreshold)
        fPeakBins.push_back(i);
    } 
  }
  
  return true;
}
