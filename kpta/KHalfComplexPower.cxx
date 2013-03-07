//______________________________________________________________________
//
// KHalfComplexPower.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Input pulse is assumed to be of the half-complex form
//   (r0, r1, r2, ..., rN/2, i(N+1)/2-1, ..., i2, i1).
// Calculates 1/N(rk*rk + ik*ik), for each k, with i0 = 0 and iN/2 = 0.
// The output "pulse" is this power spectrum and N/2 + 1 values long
//
// This power spectrum is "normalized" by the 1/N factor. It is meant to work in conjunction 
// with KRealToHalfComplexDFT()
//

#include "KHalfComplexPower.h"
#include <cmath>
#include <iostream>

using namespace std;
//ClassImp(KHalfComplexPower);

KHalfComplexPower::KHalfComplexPower(void)
{
  SetName("KHalfComplexPower");
  InitializeMembers();
}

KHalfComplexPower::KHalfComplexPower(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KHalfComplexPower"); 
  InitializeMembers();
}

KHalfComplexPower::~KHalfComplexPower(void)
{

}

void KHalfComplexPower::InitializeMembers(void)
{


}

bool KHalfComplexPower::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "KHalfComplexPower::RunProcess. input and output arrays have not been set." << endl;
    return false;
  }

  if(fInputSize < 2) return false;

  if(fOutputPulse==0 || fOutputSize != 1 + fInputSize/2 ){  //resize if necessary
    delete [] fOutputPulse;
    fOutputPulse = new double[1 + (fInputSize/2)];
    fOutputSize = 1 + fInputSize/2;
  }

  *fOutputPulse = pow(*fInputPulse,2) / fInputSize ; //the first element in the array
  *(fOutputPulse+(fInputSize/2)) = pow(*(fInputPulse+(fInputSize/2)) ,2) / fInputSize ;

  for (unsigned int k=1; k<fInputSize/2; k++) 
    *(fOutputPulse+k) = (pow(*(fInputPulse+k),2) + pow(*(fInputPulse+(fInputSize-k)),2)) / fInputSize;

  return true;

}
