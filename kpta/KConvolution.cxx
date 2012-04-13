//______________________________________________________________________
//
// KConvolution.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// y[n] = Sum_k  x[k] * h[n-k]
// 
// The output contains only the points for which the convolution can be proceeded completely
// ('valid' mode of scipy.signal.convolve)

#include "KConvolution.h"
#include <iostream>

using namespace std;
//ClassImp(KConvolution);

KConvolution::KConvolution(void)
{
  SetName("KConvolution");
  InitializeMembers();
}

KConvolution::~KConvolution(void)
{
  
}

KConvolution::KConvolution(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KConvolution"); 
   InitializeMembers();
}


void KConvolution::InitializeMembers(void)
{
  
  fResponse = 0;
  fResponseSize = 0;
  //fMode = 0;
}

bool KConvolution::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fResponse == 0) {
    cerr << "response function is not allocated." << endl;
    return false;
  }
  

  if((fOutputSize != fInputSize-fResponseSize+1)&&(fOutputSize != fResponseSize-fInputSize+1)){
    delete [] fOutputPulse;
    fOutputSize = (fInputSize > fResponseSize) ? fInputSize-fResponseSize+1 : fResponseSize-fInputSize+1;
    fOutputPulse = new double[fOutputSize];
  }





  double* inptr = (fResponseSize > fInputSize) ? fResponse : fInputPulse; // swap input and coefficients in case that the number of coefficients is greater than the pulse lenth
  double* resp = (fResponseSize > fInputSize) ? fInputPulse : fResponse; // to ensure that the following for loops do not break
  unsigned int respSize = (fResponseSize > fInputSize) ? fInputSize : fResponseSize;
  inptr += respSize-1;
  double* outptr = fOutputPulse;
  memset(outptr, 0, fOutputSize*sizeof(double)); //make sure the thing is empty. 
  unsigned int i, ii; i = ii = 0;
  
  for( ; i < fOutputSize; i++){ 
    ii = 0;
    for( ; ii < respSize; ii++)
      *outptr += *(resp+ii) * *(inptr - ii);
    
    inptr++;
    outptr++;
  }
  
  return true;

}

