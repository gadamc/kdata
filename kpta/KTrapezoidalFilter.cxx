//______________________________________________________________________
//
// KTrapezoidalFilter.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// This filter convert an exponential input pulse to trapezoidal pulse. To work properly decay time constant
// of the input pulse, rise time of the trapezoid and width of the flat part of trapezoid have to be set before
// running the filtering process.

#include "KTrapezoidalFilter.h"
#include <cmath>
#include <iostream>

using namespace std;

ClassImp(KTrapezoidalFilter);

KTrapezoidalFilter::KTrapezoidalFilter(void)
{
  SetName("KTrapezoidalFilter");
  InitializeMembers();
}

KTrapezoidalFilter::KTrapezoidalFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KTrapezoidalFilter"); 
   InitializeMembers();
}

KTrapezoidalFilter::~KTrapezoidalFilter(void)
{
  
}

void KTrapezoidalFilter::SetParams(double DecayTimeConstant, unsigned int RiseTime, unsigned int FlatTopWidth)
{
  fDecayTimeConstant = DecayTimeConstant;
  fRiseTime = RiseTime;
  fFlatTopWidth = FlatTopWidth;
}


void KTrapezoidalFilter::InitializeMembers(void)
{
  fDecayTimeConstant = 0.0;
  fRiseTime = 0;
  fFlatTopWidth = 0;
}

bool KTrapezoidalFilter::RunProcess(void)
{
    
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. " << endl;
    return false;
  }
  if(fDecayTimeConstant == 0.0 || fRiseTime == 0){
    cerr << "filter parameters do not have valid values. " <<endl;
    return false;
  }
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));
  
  double D = 0.0;
  double P = 0.0;

  for (unsigned int n = 0; n < fOutputSize; n++){
    if(n >= (2*fRiseTime+fFlatTopWidth))
      D = *(fInputPulse+n) - *(fInputPulse+n-fRiseTime) - *(fInputPulse+n-fFlatTopWidth-fRiseTime) + *(fInputPulse+n-fFlatTopWidth-2*fRiseTime);
    else if(n >= (fRiseTime+fFlatTopWidth))
      D = *(fInputPulse+n) - *(fInputPulse+n-fRiseTime) - *(fInputPulse+n-fFlatTopWidth-fRiseTime);
    else if(n >= fRiseTime)
      D = *(fInputPulse+n) - *(fInputPulse+n-fRiseTime);
    else
      D = *(fInputPulse+n);
    P += D;
    *(fOutputPulse+n) = P + fDecayTimeConstant*D;
    if(n>0)
      *(fOutputPulse+n) += *(fOutputPulse+n-1);
  }
 
  return true;
}



