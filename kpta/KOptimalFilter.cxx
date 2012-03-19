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
// Another thing to note is that the resulting amplitude estimator as a function of start time is 
// reduced by a factor of 2 from what it should be. This is because the inverse fourier spectrum transformation is only done
// on the positive frequencies and not over all frequencies, thus is off by a factor of 2. 
//


#include "KOptimalFilter.h"
#include <iostream>
#include "KHalfComplexToRealDFT.h"
#include "KHalfComplexPower.h"
#include "KHalfComplexArray.h"
#include <cmath>

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
  fHc2r->DoNotDeletePulses(true);
  fHcPower = new KHalfComplexPower();
  fHcPower->DoNotDeletePulses(true);
  
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
  //Builds the kernel of the optimal filter using the template fourier transform and
  //the average noise power spectrum.
  //
  //Note, that in order to optimize this class, some internal memory of this class points
  //to the fOutputPulse pointer. If you do not call SetInputPulse before you call this method, 
  //the fOutputPulse pointer
  //will still be NILL and calling BuildFilter will break. To get around this, if you just
  //want to call BuildFilter without calling RunProcess (RunProcess requires the existence of both
  //the input and output pulses), you can use the publicly available KPtaProcessor::AllocateArrays
  //method to set up the output pulse memory. Good luck.
  //
  if(!fRecalculate)
    return true;
  
  if(fNoiseSpectrum == 0 || fTemplateDFT == 0) {
    cerr << "noise or template spectrum is not allocated." << endl;
    return false;
  }
  
  //if the size changes, need to reallocate memory and reset 
  //the appropriate pointers. 
  if(fOptFilterSize != fTemplateDFTSize){
    //cout << "optimal filter -- allocating space for filter" << endl;
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
    fHc2r->SetOutputPulse(fOutputPulse);  //crap!  there's a bug here when you use the optimal filter in 'out-of-the-box' mode... the output array isn't set until the input array is set... therefore you can't call BuildFilter until you give it a pulse. as a work around, I have made KPtaProcessor::AllocateArrays a public method.
    fHc2r->SetOutputPulseSize(fOutputSize);
    fHc2r->SetFFTWPlan();
  }
  //cout << "running hcpower" << endl;
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
    
  //cout << "denominator " << denom << endl;
  //now caluclate the real parts of the optimal filter
  for(unsigned int i = 0; i <= fOptFilterSize/2; i++)
    *(fOptFilter+i) =  *(fTemplateDFT+i) / (denom *  *(fNoiseSpectrum+i));
    
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

void KOptimalFilter::SetNoiseSpectrum(double* resp)
{
  //This sets the internal pointer to the NoiseSpectrum for this object. This is to be used
  //with caution, as you could crash if you later move the memory. However, this will be a useful
  //tool to optimize the performance of using this filter with multiple channels. You just have to 
  //set this pointer rather than creating separate optimal filters for each channel. 
  //
  //You should also tell this object the size of this pointer with the SetNoiseSpectrumSize method.
  //Otherwise, you could also cause crashes and unpredictable behavior.
  //
  //When you call this method, the SetToRecalculate(true) is also automatically called for you.
  //The next time that you call RunProcess (or BuildFilter), the optimal filter will be rebuilt
  //using the data pointed to by the noise spectrum pointer and the pulse template spectrum pointer.
  //You can undo this behavior, of course, by calling SetToRecalculate(false) before you call RunProcess.
  //But I'm not sure why you'd ever want to do that.  
  
  fNoiseSpectrum = resp;
  SetToRecalculate();
}
void KOptimalFilter::SetNoiseSpectrumSize(unsigned int s)
{
  //Explicitly set the size of the internal array for the noise spectrum that is pointed to
  //by the interal pointer. This is a relatively dangerous method and use it at your own risk.
  //This should be used if you use the SetNoiseSpectrum(double*) method. 
  //
  //When you call this method, the SetToRecalculate(true) is also automatically called for you.
  //The next time that you call RunProcess (or BuildFilter), the optimal filter will be rebuilt
  //using the data pointed to by the noise spectrum pointer and the pulse template spectrum pointer.
  //You can undo this behavior, of course, by calling SetToRecalculate(false) before you call RunProcess.
  //But I'm not sure why you'd ever want to do that.
  
  fNoiseSpectrumSize = s;
  SetToRecalculate();
}

void KOptimalFilter::SetTemplateDFT(double* resp)
{
  //This sets the internal pointer to the TemplateDFT for this object. This is to be used
  //with caution, as you could crash if you later move the memory. However, this will be a useful
  //tool to optimize the performance of using this filter with multiple channels. You just have to 
  //set this pointer rather than creating separate optimal filters for each channel. 
  //
  //You should also tell this object the size of this pointer with the SetTemplateDFTSize method.
  //Otherwise, you could also cause crashes and unpredictable behavior.
  //
  //When you call this method, the SetToRecalculate(true) is also automatically called for you.
  //The next time that you call RunProcess (or BuildFilter), the optimal filter will be rebuilt
  //using the data pointed to by the noise spectrum pointer and the pulse template spectrum pointer.
  //You can undo this behavior, of course, by calling SetToRecalculate(false) before you call RunProcess.
  //But I'm not sure why you'd ever want to do that.
  
  fTemplateDFT = resp;
  SetToRecalculate();
}
void KOptimalFilter::SetTemplateDFTSize(unsigned int s)
{
  //Explicitly set the size of the internal array for the noise spectrum that is pointed to
  //by the interal pointer. This is a relatively dangerous method and use it at your own risk.
  //This should be used if you use the SetTemplateDFT(double*) method. 
  //
  //When you call this method, the SetToRecalculate(true) is also automatically called for you.
  //The next time that you call RunProcess (or BuildFilter), the optimal filter will be rebuilt
  //using the data pointed to by the noise spectrum pointer and the pulse template spectrum pointer.
  //You can undo this behavior, of course, by calling SetToRecalculate(false) before you call RunProcess.
  //But I'm not sure why you'd ever want to do that.
  
  fTemplateDFTSize = s;
  SetToRecalculate();
}

double KOptimalFilter::GetChiSquared(unsigned int aTimeBin)
{
  //
  
  //fOutputSize should equal fInputSize and fTemplateDFTSize.
  //while these are all in half-complex form, their size is equal to 
  //the size of the raw signal.
  //fNoiseSpectrumSize should equal fInputSize/2
  
  if(aTimeBin > fOutputSize) return -1;
  KHalfComplexArray complex;
  // double pi = 3.14159265358979;
  double twopi = 6.28318530717958; 
  
  double optimalAmp = fOutputPulse[aTimeBin];
  double optAmp2 = optimalAmp*optimalAmp;
  double chi2 = 0;
  double sig2, temp2, cos_i, sin_i, temp_re, temp_im, sig_re, sig_im; 
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++) {
  
    sig_re = complex.Real(fInputPulse, fInputSize, i);
    sig_im = complex.Imag(fInputPulse, fInputSize, i);
    temp_re = complex.Real(fTemplateDFT, fTemplateDFTSize, i);
    temp_im = complex.Imag(fTemplateDFT, fTemplateDFTSize, i);
    sig2 = sig_re*sig_re + sig_im*sig_im;
    temp2 = temp_re*temp_re + temp_im*temp_im;
    cos_i = cos(twopi*i *aTimeBin/fInputSize);
    sin_i = sin(twopi*i *aTimeBin/fInputSize);
    
    chi2 += (sig2 + temp2*optAmp2*(cos_i*cos_i + sin_i*sin_i) 
      - 2*optimalAmp*cos_i*(temp_re*sig_re + temp_im*sig_im) 
      + 2*optimalAmp*sin_i*(temp_re*sig_im - temp_im*sig_re)) / (fNoiseSpectrum[i]*fNoiseSpectrum[i]);
  }
  
  return chi2;
}




