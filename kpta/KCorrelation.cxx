//______________________________________________________________________
//
// KCorrelation.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// y[n] = Sum_k  x[k] * h[n+k]
//
// This class derives from KConvolution. When you call SetResponse, it automatically reverses the 
// response function that you give it. 
//


#include "KCorrelation.h"

//ClassImp(KCorrelation);


KCorrelation::KCorrelation(void)
{
  SetName("KCorrelation");
  InitializeMembers();
}

KCorrelation::KCorrelation(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KConvolution(inPulse, inSize, outPulse, outsize)
{
   SetName("KCorrelation"); 
   InitializeMembers();
}

KCorrelation::~KCorrelation(void)
{

}

void KCorrelation::InitializeMembers(void)
{
 
}




