//______________________________________________________________________
//
// KIIRSecondOrder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y[n] = a1y[n-1] + a2y[n-2] + b0x[n] + b1x[n-1] + b2x[n-2]
//
//

#include "KIIRSecondOrder.h"
#include <iostream>
using namespace std;

//ClassImp(KIIRSecondOrder);

KIIRSecondOrder::KIIRSecondOrder(void)
{
  SetName("KIIRSecondOrder");
  InitializeMembers();
  SetCoefA(0,0);
  SetCoefB(0,0,0);
}

KIIRSecondOrder::KIIRSecondOrder(double a1, double a2, double b0, double b1, double b2)
{
  SetName("KIIRSecondOrder");
  InitializeMembers();
  SetCoefA(a1, a2);
  SetCoefB(b0, b1, b2);
}

KIIRSecondOrder::~KIIRSecondOrder(void)
{
  
}

void KIIRSecondOrder::InitializeMembers(void)
{
  

}

void KIIRSecondOrder::SetCoefA(double a1, double a2)
{
  if(fCoefASize != 2) {
    if(fCoefA)  delete [] fCoefA;
    fCoefA = new double[2];
    fCoefASize = 2;
  }
  fCoefA[0] = a1;
  fCoefA[1] = a2;
}

void KIIRSecondOrder::SetCoefB(double b0, double b1, double b2)
{
  if(fCoefBSize != 3) {
    if(fCoefB)  delete [] fCoefB;
    fCoefB = new double[3];
    fCoefBSize = 3;
  }
  fCoefB[0] = b0;
  fCoefB[1] = b1;
  fCoefB[2] = b2;
}

bool KIIRSecondOrder::RunProcess(void)
{
  //y[n] = a1y[n-1] + a2y[n-2] + b0x[n] + b1x[n-1] + b2x[n-2]
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));

  *fOutputPulse =  *fInputPulse * *fCoefB;
  *(fOutputPulse+1) = *fOutputPulse * *fCoefA + *(fInputPulse+1) * *fCoefB + *fInputPulse * *(fCoefB+1);
  
  unsigned int i = 2;
  for( ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  *(fOutputPulse+i-1) * *fCoefA  + *(fOutputPulse+i-2) * *(fCoefA+1)  + *(fInputPulse+i) * *fCoefB +  *(fInputPulse+i-1) *  *(fCoefB+1) +  *(fInputPulse+i-2) *  *(fCoefB+2);
 
  return true;
}

