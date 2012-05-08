//______________________________________________________________________
//
// KNoiseWhitening.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Input pulse is assumed to be of the half-complex form
//   (r0, r1, r2, ..., rN/2, i(N+1)/2-1, ..., i2, i1).
// You must set the noise spectrum, which is also in half-complex form. 
// The output of this processor is the element-by-element division of the input pulse with the noise spectrum. 
// That is output_i = input_i / noise_i, where i is the i'th element of the half-complex array.
// 
//  Output(f) = Input(f) / Noise(f)
//
//
//

#include "KNoiseWhitening.h"
#include <cmath>
#include <iostream>

using namespace std;

KNoiseWhitening::KNoiseWhitening(void)
{
  SetName("KNoiseWhitening");
  InitializeMembers();
}

KNoiseWhitening::KNoiseWhitening(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KNoiseWhitening"); 
  InitializeMembers();
}

KNoiseWhitening::~KNoiseWhitening(void)
{

}

void KNoiseWhitening::InitializeMembers(void)
{


}

bool KNoiseWhitening::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "KNoiseWhitening::RunProcess. input and output arrays have not been set." << endl;
    return false;
  }

  if(fInputSize != fOutputSize) {
    cerr << "KNoiseWhitening::RunProcess. input and output arrays must be the same size." << endl;
    return false;
  }
  
  if(fNoiseSpectrumSize != fOutputSize) {
    cerr << "KNoiseWhitening::RunProcess. noise spectrum array must be the same size as the input and output arrays." << endl;
    return false;
  }
  
  for(unsigned int i = 0; i < fInputSize; i++)
    *(fOutputPulse+i) = *(fInputPulse+i) / *(fNoiseSpectrum+i);

  return true;

}
