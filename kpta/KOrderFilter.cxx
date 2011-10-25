//______________________________________________________________________
//
// KOrderFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This is a special filter that has been successfully used in the ERA
// analysis to smooth a pulse before searching for peaks.
//
// y[n] = x[n] - x[n-k]
//
// where k is the "order" of the filter
//
// This processor can be run 'in-place' where the input and output pulse share the 
// same space in memory. 
//
// When this object is instantiated, the order, k, is set to 0 by default. This means
// it makes no transformation on your pulse. You must call SetOrder(k).
//

#include "KOrderFilter.h"
#include <iostream>
using namespace std;


KOrderFilter::KOrderFilter(void)
{
  SetName("KOrderFilter");
  InitializeMembers();
  fOrder = 0;
  fSetInitOutputValue = false;
  fInitOutputValue = 0;
}

KOrderFilter::KOrderFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KOrderFilter"); 
   InitializeMembers();
   fOrder = 0;
   fSetInitOutputValue = false;
   fInitOutputValue = 0;
}

KOrderFilter::~KOrderFilter(void)
{
  
}

void KOrderFilter::InitializeMembers(void)
{
  

}

bool KOrderFilter::RunProcess(void)
{
  // y[n] = x[n] - x[n-k]
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fOrder == 0) {
    if(fOutputPulse != fInputPulse)  //don't copy if running this processor "in-place"
       memcpy(fOutputPulse, fInputPulse, fOutputSize * sizeof(double));
       
    return true;
  }
  
  if(!fSetInitOutputValue){
    if(fOutputPulse != fInputPulse)  //don't copy if running this processor "in-place"
      memcpy(fOutputPulse, fInputPulse, fOrder * sizeof(double));
  }
  else {
    memset(fOutputPulse, fInitOutputValue, fOrder * sizeof(double));
  }
  
  for(unsigned int n = fOrder; n < fOutputSize; n++)
    *(fOutputPulse+n) = *(fInputPulse+n) + *(fInputPulse+n-fOrder);
    
  return true;
}

