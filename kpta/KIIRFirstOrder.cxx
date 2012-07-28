//______________________________________________________________________
//
// KIIRFirstOrder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y[n] = -a1y[n-1] + b0x[n] + b1x[n-1]
//
//

#include "KIIRFirstOrder.h"
#include <iostream>
using namespace std;

ClassImp(KIIRFirstOrder);

KIIRFirstOrder::KIIRFirstOrder(void)
{
  SetName("KIIRFirstOrder");
  InitializeMembers();
  SetCoefA(0);
  SetCoefB(0,0);
}

KIIRFirstOrder::KIIRFirstOrder(double a1, double b0, double b1)
{
  SetName("KIIRFirstOrder");
  InitializeMembers();
  SetCoefA(a1);
  SetCoefB(b0,b1);
}

KIIRFirstOrder::~KIIRFirstOrder(void)
{
  
}

void KIIRFirstOrder::InitializeMembers(void)
{
  

}

void KIIRFirstOrder::SetCoefA(double a)
{
  if(fCoefASize != 1) {
    if(fCoefA)  delete [] fCoefA;
    fCoefA = new double[1];
    fCoefASize = 1;
  }
  fCoefA[0] = a;
}

void KIIRFirstOrder::SetCoefB(double b0, double b1)
{
  if(fCoefBSize != 2) {
    if(fCoefB)  delete [] fCoefB;
    fCoefB = new double[2];
    fCoefBSize = 2;
  }
  fCoefB[0] = b0;
  fCoefB[1] = b1;
}

bool KIIRFirstOrder::RunProcess(void)
{
  //y[n] = -a1y[n-1] + b0x[n] + b1x[n-1]
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));

  *fOutputPulse =  (*fInputPulse) * (*fCoefB);
  
  unsigned int i = 1;
  for( ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  -*(fOutputPulse+i-1) * (*fCoefA)  + *(fInputPulse+i) * (*fCoefB) +  *(fInputPulse+i-1) *  (*(fCoefB+1));
 
  return true;
}

