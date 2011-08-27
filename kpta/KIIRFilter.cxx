//______________________________________________________________________
//
// KIIRFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// 
//
//

#include "KIIRFilter.h"
#include <cmath>

//ClassImp(KIIRFilter);

KIIRFilter::KIIRFilter(void)
{
  SetName("KIIRFilter");
  InitializeMembers();
}

KIIRFilter::KIIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KIIRFilter"); 
   InitializeMembers();
}

KIIRFilter::~KIIRFilter(void)
{
  
}

void KIIRFilter::SetCoefficients(double* a, unsigned int asize, double* b, unsigned int bsize)
{
  if(bsize != fCoefBSize){
    if(fCoefB) delete [] fCoefB;
    fCoefBSize = bsize;
    fCoefB = new double[fCoefBSize];
  }
  
  if(asize != fCoefASize){
    if(fCoefA) delete [] fCoefA;
    fCoefASize = asize;
    fCoefA = new double[fCoefASize];
  }
  
  memcpy(fCoefA, a, asize);
  memcpy(fCoefB, b, bsize);
}


void KIIRFilter::InitializeMembers(void)
{
  //no local members to initialize
  fCoefA = 0;
  fCoefASize = 0;
  fCoefB = 0;
  fCoefBSize = 0;

}


