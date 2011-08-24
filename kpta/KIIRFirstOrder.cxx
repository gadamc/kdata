//______________________________________________________________________
//
// KIIRFirstOrder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y[n] = a1y[n-1] + b0x[n] + b1x[n-1]
//
//

#include "KIIRFirstOrder.h"
#include <iostream>
using namespace std;

//ClassImp(KIIRFirstOrder);

KIIRFirstOrder::KIIRFirstOrder(void)
{
  SetName("KIIRFirstOrder");
  InitializeMembers();
  double a[1], b[2];
  a[0] = b[0] = b[1] = 0;
  SetCoefficients(a,1,b,2);
}

KIIRFirstOrder::KIIRFirstOrder(double a1, double b0, double b1)
{
  SetName("KIIRFirstOrder");
  InitializeMembers();
  double aa[1], bb[1];
  aa[0] = a1;
  bb[0] = b0;
  bb[1] = b1;
  SetCoefficients(aa,1,bb,2);
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
  //y[n] = a1y[n-1] + b0x[n] + b1x[n-1]
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  *fOutputPulse = *fCoefB * *fInputPulse;
  unsigned int i = 1;
  for( ; i < fOutputSize; i++)
    *(fOutputPulse+i) = *fCoefA * *(fOutputPulse+i-1)  + *fCoefB **(fInputPulse+i);
  
  return true;
}

