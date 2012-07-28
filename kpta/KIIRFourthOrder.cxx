//______________________________________________________________________
//
// KIIRFourthOrder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y[n] = -a1y[n-1] - a2y[n-2] - a3y[n-3] - a4y[n-4] + b0x[n] + b1x[n-1] + b2x[n-2] + b3x[n-3] + b4x[n-4]
//
//

#include "KIIRFourthOrder.h"
#include <iostream>
using namespace std;

//ClassImp(KIIRFourthOrder);

KIIRFourthOrder::KIIRFourthOrder(void)
{
  SetName("KIIRFourthOrder");
  InitializeMembers();
  SetCoefA(0,0,0,0);
  SetCoefB(0,0,0,0,0);
}

KIIRFourthOrder::KIIRFourthOrder(double a1, double a2, double a3, double a4,
                               double b0, double b1, double b2, double b3, double b4)
{
  SetName("KIIRFourthOrder");
  InitializeMembers();
  SetCoefA(a1, a2, a3, a4);
  SetCoefB(b0, b1, b2, b3, b4);
}

KIIRFourthOrder::~KIIRFourthOrder(void)
{
  
}

void KIIRFourthOrder::InitializeMembers(void)
{
  

}

void KIIRFourthOrder::SetCoefA(double a1, double a2, double a3, double a4)
{
  if(fCoefASize != 4) {
    if(fCoefA)  delete [] fCoefA;
    fCoefA = new double[4];
    fCoefASize = 4;
  }
  fCoefA[0] = a1;
  fCoefA[1] = a2;
  fCoefA[2] = a3;
  fCoefA[3] = a4;
}

void KIIRFourthOrder::SetCoefB(double b0, double b1, double b2, double b3, double b4)
{
  if(fCoefBSize != 5) {
    if(fCoefB)  delete [] fCoefB;
    fCoefB = new double[5];
    fCoefBSize = 5;
  }
  fCoefB[0] = b0;
  fCoefB[1] = b1;
  fCoefB[2] = b2;
  fCoefB[3] = b3;
  fCoefB[4] = b4;
}

bool KIIRFourthOrder::RunProcess(void)
{
  // y[n] = a1y[n-1] + a2y[n-2] + a3y[n-3] + a4y[n-4] + b0x[n] + b1x[n-1] + b2x[n-2] + b3x[n-3] + b4x[n-4]
  
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
  
  *(fOutputPulse+3) = -*(fOutputPulse+2) * *fCoefA - *(fOutputPulse+1) * *(fCoefA+1) - 
                      + *fOutputPulse * *(fCoefA+2) + *(fInputPulse+3) * *fCoefB 
                      + *(fInputPulse+2) * *(fCoefB+1) + *(fInputPulse+1) * *(fCoefB + 2)
                      + *fInputPulse * *fCoefB;
  
  unsigned int i = 4;
  for( ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  -*(fOutputPulse+i-1) * *fCoefA  - *(fOutputPulse+i-2) * *(fCoefA+1) - *(fOutputPulse+i-3) * *(fCoefA+2)
                        - *(fOutputPulse+i-4) * *(fCoefA+3)
                        + *(fInputPulse+i) * *fCoefB +  *(fInputPulse+i-1) *  *(fCoefB+1) +  *(fInputPulse+i-2) *  *(fCoefB+2)
                        + *(fInputPulse+i-3) *  *(fCoefB+3) + *(fInputPulse+i-4) * *(fCoefB+4);
 
  return true;
}

