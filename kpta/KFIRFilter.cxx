//______________________________________________________________________
//
// KFIRFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Standard FIR filter. Convolutions are only done in the time-domain at the moment. 
// This is exactly the same as KConvolution - it inherits from KConvolution.
// The FIR "coefficients" are the exact same as the "response" function in the KConvolution class. 
//
//
//

#include "KFIRFilter.h"

//ClassImp(KFIRFilter);


KFIRFilter::KFIRFilter(void)
{
  SetName("KFIRFilter");
  InitializeMembers();
}

KFIRFilter::KFIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KConvolution(inPulse, inSize, outPulse, outsize)
{
   SetName("KFIRFilter"); 
   InitializeMembers();
}

KFIRFilter::~KFIRFilter(void)
{

}

void KFIRFilter::InitializeMembers(void)
{
 
}

bool KFIRFilter::RunProcess(void)
{
  return KConvolution::RunProcess();
}



