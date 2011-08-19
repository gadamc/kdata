//______________________________________________________________________
//
// KSimpleLowPassFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Monday 20. June 2011
//
//
//
// This class performs a 1st-order low pass filter operation on the input pulse in the time domain.
// Can set the characteristic RC time constant via SetRc(double value);
// The value must be in units of "bin width", that is, in units of the sampling time
// of the input pulse.
// The output pulse is also in the time domain.
// Additionally, if you want to avoid some rounding errors during the integration
// you can call SetMinRCToBinRatio and give it a value greater than the value
// you gave in SetRc. The routine will then subdivide your input pulse accordingly
// and peform the integration on the subdivided pulse. 


#include "KSimpleLowPassFilter.h"
#include "math.h"

KSimpleLowPassFilter::KSimpleLowPassFilter(void)
{

  InitializeMembers();
}

KSimpleLowPassFilter::~KSimpleLowPassFilter(void)
{


}


void KSimpleLowPassFilter::InitializeMembers(void)
{
  SetRc(20);
  SetMinRCToBinRatio(10.0);
}

bool KSimpleLowPassFilter::RunProcess(void)
{
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  //first, make the pulse "quasi-periodic" to eliminate artificial
  //filtering effects due to filtering of a signal that has a DC offset
  int numBinsToAdd = floor(GetRc()*10);
  int tempSize = numBinsToAdd + (int)fInputSize;
  double *tempPulse = new double[tempSize];
  
  try {
    for (int i = 0; i < tempSize; i++){
      if (i < numBinsToAdd)
        *(tempPulse+i) = *(fInputPulse+0);
      else
        *(tempPulse+i) = *(fInputPulse+(i - numBinsToAdd));
      
    }
	}
	catch (out_of_range& e) {
		cerr << "KSimpleLowPassFilter. exception caught while creating temp pulse: " << e.what() << endl;
		cerr << "    stopping calculation";
		return false;
	}
  
  try {
    double binDivFactor = GetMinRCToBinRatio()/GetRc() + 1.0;  //this is only useful if the RC is small.
    double binSize = 1./binDivFactor;
    double theIntegral = *tempPulse;

    for(int t=0; t < tempSize; t++){
      
      if(t >= numBinsToAdd) *(fOutputPulse+(t-numBinsToAdd)) = 0.;  //don't care about the first numBinsToAdd
      
      for(int i = 0; i < (int)binDivFactor; i++){
        theIntegral = theIntegral*exp(-binSize/fRc) + (*(tempPulse+t) / binDivFactor) *exp(-0.5*binSize/fRc)*binSize/fRc;
        if(t >= numBinsToAdd) *(fOutputPulse+(t-numBinsToAdd)) += theIntegral;
      }
    }
    
    
  } 
  catch (out_of_range& e) {
		cerr << "KSimpleLowPassFilter. exception caught while performing calculation: " << e.what() << endl;
		cerr << "    stopping calculation";
		return false;
	}  
  
  
  
  return true;
}