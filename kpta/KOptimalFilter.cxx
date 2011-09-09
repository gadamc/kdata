//______________________________________________________________________
//
// KOptimalFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// The template-DFT must be in half-complex format (see KRealToHalfComplexDFT)
// and should be of size N.
// But the noise spectrum is the average power spectrum of size N/2. 
// You must set these data properly, otherwise, this will probably break and go down in flames.
// 
// The optimal filter built from these two inputs will be a half-complex array of size N.
//
// To use this class, the input pulse will be DFT (in half-complex format of size N) of 
// the all-real input pulse (after baseline subtraction
// and pattern removal) and the output "pulse" will be an array of length N that is the pulse amplitude
// as a function of pulse start times. The maximum value in this array is the optimal filter estimate
// of the amplitude of the pulse and the element of the output array where this occurs is the estimate
// of the pulse start time.
//
//


#include "KOptimalFilter.h"
#include <iostream>
#include "KHalfComplexToRealDFT.h"
#include "KHalfComplexPower.h"

using namespace std;
//ClassImp(KOptimalFilter);

KOptimalFilter::KOptimalFilter(void)
{
  SetName("KOptimalFilter");
  InitializeMembers();
}

KOptimalFilter::~KOptimalFilter(void)
{
  //
  
  //need to destroy your stuff!
  if(fNoiseSpectrum) delete [] fNoiseSpectrum;
  if(fTemplateDFT) delete [] fTemplateDFT;
  if(fOptFilter) delete [] fOptFilter;
  if(fOptFilterAndSignal) delete [] fOptFilterAndSignal;
  if(fTemplatePower) delete [] fTemplatePower;
  if(fHc2r) delete fHc2r;
  if(fHcPower) delete fHcPower;
}

KOptimalFilter::KOptimalFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KOptimalFilter"); 
   InitializeMembers();
}


void KOptimalFilter::InitializeMembers(void)
{
  
  fNoiseSpectrum = 0;
  fTemplateDFT = 0;
  fOptFilter = 0;
  fOptFilterAndSignal = 0;
  fTemplatePower = 0;
  
  fNoiseSpectrumSize = 0;
  fTemplateDFTSize = 0;
  fOptFilterSize = 0;
  fOptFilterAndSignalSize = 0;
  fTemplatePowerSize = 0;
  fRecalculate = true;
  
  fHc2r = new KHalfComplexToRealDFT();
  fHcPower = new KHalfComplexPower();
  
}

bool KOptimalFilter::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fNoiseSpectrum == 0 || fTemplateDFT == 0) {
    cerr << "noise or template spectrum is not allocated." << endl;
    return false;
  }
  
  if(fRecalculate){
    if(!BuildFilter())
      return false;
  }
  
  for(unsigned int i = 0; i < fOptFilterAndSignalSize; i++)
    *(fOptFilterAndSignal+i) = *(fOptFilter+i) * *(fInputPulse+i);
  
  return fHc2r->RunProcess();

}

bool KOptimalFilter::BuildFilter(void)
{
  
  if(!fRecalculate)
    return true;
  
  if(fNoiseSpectrum == 0 || fTemplateDFT == 0) {
    cerr << "noise or template spectrum is not allocated." << endl;
    return false;
  }
  
  //if the size changes, need to reallocate memory and reset 
  //the appropriate pointers. 
  if(fOptFilterSize != fTemplateDFTSize){
    if(fOptFilter)  delete [] fOptFilter;
    fOptFilterSize= fTemplateDFTSize;
    fOptFilter = new double[fOptFilterSize];
    
    //allocate the memory for this now - its not used in this method
    //but is used in RunProcess.
    if(fOptFilterAndSignal) delete [] fOptFilterAndSignal;
    fOptFilterAndSignalSize = fTemplateDFTSize;
    fOptFilterAndSignal = new double[fOptFilterAndSignalSize];
      
    if(fTemplatePower) delete [] fTemplatePower;
    fTemplatePowerSize = fTemplateDFTSize/2 + 1;
    fTemplatePower = new double[fTemplatePowerSize];
    
    fHcPower->SetInputPulse(fTemplateDFT);
    fHcPower->SetInputPulseSize(fTemplateDFTSize);
    fHcPower->SetOutputPulse(fTemplatePower);
    fHcPower->SetOutputPulseSize(fTemplatePowerSize);
    
    fHc2r->SetInputPulse(fOptFilterAndSignal);
    fHc2r->SetInputPulseSize(fOptFilterAndSignalSize); 
    fHc2r->SetOutputPulse(fOutputPulse);
    fHc2r->SetOutputPulseSize(fOutputSize);
    fHc2r->SetFFTWPlan();
  }

  fHcPower->RunProcess();
  
  // the optimal filter has a length of n elements. the first n/2 + 1 are the real parts
  // the remaining elements are the imaginary parts. but, because we are assuming that
  // our pulses are real (which they are), the 0th and n/2th components have an imaginary value
  // of zero and are therefore not included in this half-complex format.
  // the templateDFT has the same half-complex structure. The first n/2 + 1 elements correspond
  // to the lowest frequency component (DC) up to the highest (Nyquist). The corresponding frequency
  // component order is reveresed in the imaginary part of the half-complex array. The lowest frequency 
  // component (not DC!) in the imaginary part is the (n-1)st element of the array and the highest frequncy
  // component (also not Nyquist) in the imaginary part is the (n/2 + 1)st element. 
  // See the KRealToHalfComplexDFT, KHalfComplextToRealDFT and the FFTW documentation.
  //
  // the noise power, on the other hand, has a lengh of n/2+1 elements starting with the lowest
  // frequency component (DC) up to the highest (Nyquist).
  
  
  //first calculate the denominator of the optimal filter
  double denom = 0;
  for (unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    denom += *(fTemplatePower+i) / *(fNoiseSpectrum+i);
    
  cout << "denominator " << denom << endl;
  //now caluclate the real parts of the optimal filter
  for(unsigned int i = 0; i <= fOptFilterSize/2; i++){
    *(fOptFilter+i) = *(fTemplateDFT+i) / (denom * *(fNoiseSpectrum+i));
  }
  
  //then the imaginary parts. make sure to use the correct index in the noise spectrum
  unsigned int j = 1;
  for(unsigned int i = fOptFilterSize-1; i > fOptFilterSize/2; i--)
    *(fOptFilter+i) = -1* *(fTemplateDFT+i) / (denom *  *(fNoiseSpectrum+(j++)));

  
  fRecalculate = false;
  return true;
}

void KOptimalFilter::SetOutputPulseSize(unsigned int s)
{
  fOutputSize = s;
  fHc2r->SetOutputPulseSize(fOutputSize); 
}

void KOptimalFilter::SetOutputPulse(double *aPulse)
{
  fOutputPulse = aPulse;
  fHc2r->SetOutputPulse(aPulse);
  fHc2r->SetFFTWPlan();
}
