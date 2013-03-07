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
// You can SetPolarity( -1, +1, or 0). If set to 0, then this object searches for pulses 
// in both directions. otherwise, it only looks for pulses with the polarity you indicated.
//
// This class now inherits from KOrderThresholdFilter. It sets the threshold based upon the value
// set in fNumRms and the calculated std deviation of the smoothed pulse between the start and stop values.

#include <iostream>
#include "KEraPeakFinder.h"

using namespace std;

//ClassImp(KEraPeakFinder);

KEraPeakFinder::KEraPeakFinder(void)
{
  SetName("KEraPeakFinder");
  InitializeMembers();
}

KEraPeakFinder::KEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KOrderThresholdFinder(inPulse, inSize, outPulse, outsize)
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
  //threshold for peak detection is 3.0 * rms. 

  fBaselineStart = 0.0;
  fBaselineStop = 0.40;
  SetOrder(3);
  fNumRms = 3.0; 

}

bool KEraPeakFinder::RunProcess(void)
{

  if( (unsigned int)(fBaselineStart*GetOutputPulseSize()) >= GetOutputPulseSize() ||  
  (unsigned int)(fBaselineStop*GetOutputPulseSize()) >= GetOutputPulseSize() )  //have to check this before using Rms calculator because it doesn't know how the size of the array
    return false;

  if(fBaselineStop < fBaselineStart) //require something reasonable
    return false;

  if(SmoothPulse()){  
        
    SetThreshold( fNumRms* fRms.GetStdDev(GetOutputPulse(), (unsigned int)(fBaselineStart*GetOutputPulseSize()), (unsigned int)(fBaselineStop*GetOutputPulseSize()) ) );

    return FindPeaks();
  }
  else {
    return false;
  }

}

