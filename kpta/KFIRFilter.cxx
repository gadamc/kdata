//______________________________________________________________________
//
// KFIRFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Standard FIR filter. Convolutions are only done in the time-domain at the moment. 
//
//
//

#include "KFIRFilter.h"
#include <algorithm>

using namespace std;

//ClassImp(KFIRFilter);


KFIRFilter::KFIRFilter(void)
{
  
  InitializeMembers();
}

KFIRFilter::~KFIRFilter(void)
{
  if(fCoefficients) delete [] fCoefficients;
}

void KFIRFilter::InitializeMembers(void)
{
  //no local members to initialize
  fCoefficients = 0;
  fCoefSize = 0;

}

bool KFIRFilter::RunProcess(void)
{
	//at this time, the number of coefficients, D,  must be less than the length of the 
	//pulse, N. The output pulse is the same length of the input pulse, but remember
	//that the first D points in the output pulse is the transient response of the 
	//filter. Currently, this only calcuates at time-domain convolution. 
	
	if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fCoefSize > fInputSize){
    cerr << "the number of coefficients is greater than the pulse length" << endl;
    return false; 
  }
  
  //in the future: 
  //check to see if the calculation done by fft convolution is faster than 
	//convolution in the time domain. then choose which algorithm to use.
	//for small D, time-domain calculation is faster.
	//
  
  double* inptr = fInputPulse;
  double* outptr = fOutputPulse;
  memset(outptr, 0, fOutputSize*sizeof(double)); //make sure the thing is empty. 
  unsigned int i, ii; i = ii = 0;
  
  for( ; i < fCoefSize; i++){
    ii = 0;
    for( ; ii <= i; ii++)
      *outptr += *(fCoefficients+ii) * *(inptr - ii);
    
    inptr++;
    outptr++;
  }
  
  
  for( ; i < fOutputSize; i++){ 
    ii = 0;
    for( ; ii < fCoefSize; ii++)
      *outptr += *(fCoefficients+ii) * *(inptr - ii);
    
    inptr++;
    outptr++;
  }

	
  return true;
  
}



