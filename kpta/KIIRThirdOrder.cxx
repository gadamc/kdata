//______________________________________________________________________
//
// KIIRThirdOrder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y[n] = -a1y[n-1] - a2y[n-2] - a3y[n-3] + b0x[n] + b1x[n-1] + b2x[n-2] + b3x[n-3]
//
//

#include "KIIRThirdOrder.h"
#include <iostream>
using namespace std;


KIIRThirdOrder::KIIRThirdOrder(void)
{
  SetName("KIIRThirdOrder");
  InitializeMembers();
  SetCoefA(0,0,0);
  SetCoefB(0,0,0,0);
}

KIIRThirdOrder::KIIRThirdOrder(double a1, double a2, double a3, double b0, double b1, double b2, double b3)
{
  SetName("KIIRThirdOrder");
  InitializeMembers();
  SetCoefA(a1, a2, a3);
  SetCoefB(b0, b1, b2, b3);
}

KIIRThirdOrder::~KIIRThirdOrder(void)
{
  
}

void KIIRThirdOrder::InitializeMembers(void)
{
  

}

void KIIRThirdOrder::SetCoefA(double a1, double a2, double a3)
{
  if(fCoefASize != 3) {
    if(fCoefA)  delete [] fCoefA;
    fCoefA = new double[3];
    fCoefASize = 3;
  }
  fCoefA[0] = a1;
  fCoefA[1] = a2;
  fCoefA[2] = a3;
}

void KIIRThirdOrder::SetCoefB(double b0, double b1, double b2, double b3)
{
  if(fCoefBSize != 4) {
    if(fCoefB)  delete [] fCoefB;
    fCoefB = new double[4];
    fCoefBSize = 4;
  }
  fCoefB[0] = b0;
  fCoefB[1] = b1;
  fCoefB[2] = b2;
  fCoefB[3] = b3;
}

bool KIIRThirdOrder::RunProcess(void)
{
  //y[n] = -a1y[n-1] - a2y[n-2] - a3y[n-3] + b0x[n] + b1x[n-1] + b2x[n-2] + b3x[n-3]
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));

  *fOutputPulse =  *fInputPulse * *fCoefB;
  *(fOutputPulse+1) = -*fOutputPulse * *fCoefA + *(fInputPulse+1) * *fCoefB + *fInputPulse * *(fCoefB+1);  
  *(fOutputPulse+2) = -*(fOutputPulse+1) * *fCoefA - *fOutputPulse * *(fCoefA+1) + *(fInputPulse+2) * *fCoefB 
                      + *(fInputPulse+1) * *(fCoefB+1) + *fInputPulse * *(fCoefB + 2);
  
  unsigned int i = 3;
  for( ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  -*(fOutputPulse+i-1) * *fCoefA  - *(fOutputPulse+i-2) * *(fCoefA+1) - *(fOutputPulse+i-3) * *(fCoefA+2) 
                        + *(fInputPulse+i) * *fCoefB +  *(fInputPulse+i-1) *  *(fCoefB+1) +  *(fInputPulse+i-2) *  *(fCoefB+2)
                        + *(fInputPulse+i-3) *  *(fCoefB+3);
 
  return true;
}

