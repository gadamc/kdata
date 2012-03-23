//______________________________________________________________________
//
// KEraPeakFinder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2012 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Implements the ERA style of peak detection, using a KOrderFilter to smooth the 
// pulse and then search the trace for any bin
// greater than some factor of the pulse's pretrigger RMS. 
// This processor applies the KOrderFilter to the inputPulse of this object
// and places the results in this object's outputPulse. Thus, the outputPulse of this processor is a "smoothed" 
// pulse. This should be considered if you want to place this processor within a KPulseAnalyisChain
// 

#include <iostream>
#include "KEraPeakFinder.h"

using namespace std;

KEraPeakFinder::KEraPeakFinder(void)
: fOrderFilter(0, 0, 0, 0)  //initialize the order filter with null pointers so that we can set the internal arrays at run-time
{
  SetName("KEraPeakFinder");
  InitializeMembers();
}

KEraPeakFinder::KEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize), fOrderFilter(0, 0, 0, 0) //initialize the order filter with null pointers so that we can set the internal arrays at run-time
{
  SetName("KEraPeakFinder"); 
  InitializeMembers();
  
}

KEraPeakFinder::~KEraPeakFinder(void)
{

}

void KEraPeakFinder::InitializeMembers(void)
{
  //initialize members. The baseline start and stop values are initially
  //set to 0.0 and 0.40, respectively. The default order is 3 and the default
  //threshold for peak detection is 3.5 * rms. 

  fBaselineStart = 0.0;
  fBaselineStop = 0.40;
  fOrder = 3; 
  fNumRms = 3.0; 
  fPeakBins.reserve(100);
  fPolarity = 0;
}

bool KEraPeakFinder::RunProcess(void)
{
  //cout << "Run Process: " << GetName() << endl;
  
  if(SmoothPulse())
    return FindPeaks();
  else {
    return false;
  }

}

bool KEraPeakFinder::SmoothPulse(void)
{
  fOrderFilter.SetInputPulse(GetInputPulse());
  fOrderFilter.SetInputPulseSize(GetInputPulseSize());
  fOrderFilter.SetOutputPulse(GetOutputPulse());
  fOrderFilter.SetOutputPulseSize(GetOutputPulseSize());
  fOrderFilter.SetOrder(fOrder);
  
  if(fOrderFilter.RunProcess()) return true;
  else return false;
}

bool KEraPeakFinder::FindPeaks(void)
{
  if( (unsigned int)(fBaselineStart*GetOutputPulseSize()) >= GetOutputPulseSize() ||  
  (unsigned int)(fBaselineStop*GetOutputPulseSize()) >= GetOutputPulseSize() )  //have to check this before using Rms calculator because it doesn't know how the size of the array
    return false;
    
  fPeakBins.erase(fPeakBins.begin(), fPeakBins.end());  //this should, from what I understand, keep the memory allocated, which would be faster than deallocation and reallocation for each pulse
  
  double threshold = fNumRms* fRms.GetRms(GetOutputPulse(), (unsigned int)(fBaselineStart*GetOutputPulseSize()), (unsigned int)(fBaselineStop*GetOutputPulseSize()) );
  
  if (fPolarity == 0){
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if( abs(GetOutputPulse()[i]) >= threshold )
        fPeakBins.push_back(i);
    }
  }
  else if (fPolarity > 0){
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if (GetOutputPulse()[i] > threshold)
        fPeakBins.push_back(i);
    }
  }
  else {
    threshold = -1.*threshold;
    for (unsigned int i = 0; i < GetOutputPulseSize(); i++) {
      if (GetOutputPulse()[i] < threshold)
        fPeakBins.push_back(i);
    } 
  }
  
  return true;
}
