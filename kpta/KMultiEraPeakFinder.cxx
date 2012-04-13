//______________________________________________________________________
//
// KMultiEraPeakFinder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2012 Karlsruhe Inst. of Technology. All Rights Reserved.
//


#include "KMultiEraPeakFinder.h"


KMultiEraPeakFinder::KMultiEraPeakFinder(void)
{
  SetName("KMultiEraPeakFinder");
  InitializeMembers();
 
}

KMultiEraPeakFinder::KMultiEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KMultiEraPeakFinder"); 
   InitializeMembers();
}

KMultiEraPeakFinder::~KMultiEraPeakFinder(void)
{
  
}

void KMultiEraPeakFinder::InitializeMembers(void)
{
  

}
