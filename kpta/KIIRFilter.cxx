//______________________________________________________________________
//
// KIIRFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// y_n = Sum_i (i = 0 to i = P) b_i * x[n-i]  -  Sum_j (j = 1 to j = Q) a_j * y[n-j] 
//
// note that in the KData implementation there is no "a_0" term. This is just a gain that can
// be applied by the user at a different step. When you set the 'a' coefficients you should
// NOT include a_0. The coefficients 'a' should start with the "a_1" term.
//
// However, this gain could be added to the iir filter. 
//
// Also, note the '-' sign in the equation above. Make sure that you provide the correct sign of the a parameters.
// This is the traditional form (from what I can tell in the literature), however some design codes may return 
// the opposite sign for the coefficients. 
//

#include "KIIRFilter.h"
#include <cmath>
#include <iostream>

using namespace std;

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
  if(fCoefB) delete [] fCoefB;
  if(fCoefA) delete [] fCoefA;
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
  fCoefA = 0;
  fCoefASize = 0;
  fCoefB = 0;
  fCoefBSize = 0;

}

bool KIIRFilter::RunProcess(void)
{
  //
  // y_n = Sum_i (i = 0 to i = P) b_i * x[n-i]  -  Sum_j (j = 1 to j = Q) a_j * y[n-j] 
  //
  // note that in the KData implementation there is no "a_0" term. This is just a gain that can
  // be applied by the user at a different step. When you set the 'a' coefficients you should
  // NOT include a_0. The coefficients 'a' should start with the "a_1" term.
  //
  // However, this gain could be added to the iir filter. 
  //
  
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. " << endl;
    return false;
  }
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));

  
  for (unsigned int n = 0; n < fOutputSize; n++){
    
    for(unsigned int i = 0; i < fCoefBSize && i <= n; i++){
      *(fOutputPulse+n) += *(fCoefB+i) * *(fInputPulse+n-i);
    }
    for(unsigned int j = 1; j <= fCoefASize && j <=n ; j++){
      *(fOutputPulse+n) -= *(fCoefA+j-1) * *(fOutputPulse+n-j-1);
    }
    
  }
 
  return true;
}



