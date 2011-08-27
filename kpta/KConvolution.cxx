//______________________________________________________________________
//
// KConvolution.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// 
//
//

#include "KConvolution.h"
#include <iostream>
using namespace std;
//ClassImp(KConvolution);

KConvolution::KConvolution(void)
{
  SetName("KConvolution");
  InitializeMembers();
}

KConvolution::~KConvolution(void)
{
  
}


void KConvolution::InitializeMembers(void)
{
  

}

bool KConvolution::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fResponse == 0) {
    cerr << "response function is not allocated." << endl;
    return false;
  }
  
  unsigned int i, j, k;


  // start convolution from out[fResponseSize-1] to out[dataSize-1] (last)
  for(i = fResponseSize-1; i < fOutputSize; ++i)
  {
      *(fOutputPulse+i) = 0;                             // init to 0 

      for(j = i, k = 0; k < fResponseSize; --j, ++k)
          *(fOutputPulse+i) += *(fInputPulse+j) *  *(fResponse+k);
  }

  // convolution from fOutputPulse[0] to fOutputPulse[fResponseSize-2]
  for(i = 0; i < fResponseSize - 1; ++i)
  {
      *(fOutputPulse+i) = 0;                             // init to 0 

      for(j = i, k = 0; j >= 0; --j, ++k)
          *(fOutputPulse+i) += *(fInputPulse+j) *  *(fResponse+k);
  }

  return true;

}

