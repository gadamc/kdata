//______________________________________________________________________
//
// KOptimalFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This class encapsulates all the steps needed to use the Wiener Optimal Filter. To calculate the optimal filter 
// amplitude as a function of pulse start-time for a real physical pulse of length N, you must supply an instance of this class
// with the pulse template (in fourier space in half-complex notation array of size N), and the noise power spectrum of length N/2 + 1.
// Then, for each pulse, you give the instance of this class the input pulse (again in fourier space) via the SetInput method. 
// Next, you call RunProcess(). The amplitude estimator as a function of pulse start-time (where the start-times are the same
// as the discrete pulse sample points) is found in the output pulse, obtained via GetOutput().
//
// The template-DFT must be in half-complex format (see KRealToHalfComplexDFT)
// and should be of size N.
// But the noise spectrum is the average power spectrum of size N/2 + 1. 
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
// If the noise spectrum was produced from noise events that were windowed before the power spectrum
// was calculated, then you must take care to window the input signal before you calculate the Fourier transform
// of that signal and pass it to this class.
//
// One can choose three methods of calculation, which will give the exact same results. However, depending upon the length
// of the pulse and the range of interest (specified by fOptimalFilterTimeRangeMin/Max), one of these methods will 
// require the fewest number of operations to perform the calculation. The choices are
//    a. use inverse FFT
//    b. direct integration
//    c. chi^2 minimization
// and are selected by fUseInverseFFT (true = a, false = b) or by not using the KOptimalFilter::RunProcess method, and rather
// using a minimization routine to find the amplitudet/start-time that minimizes KOptimalFilter::GetChiSquared. 
//
// The number of operations for each method are estimated here.
// 
// a. use inverse FFT 
//    There are two things that must be done here. The first is the multiplication of the N-lengthed input pulse with
//    the optimal filter kernel. This is done using the KHalfComplexArray::Multiply method. This multiply method is not
//    terribly efficient since it was written for clarity and for dealing with the half-complex array notation. This method
//    takes about 21*N operations. 
//
//    Following this, there are N*logN operations (roughly) that need to be done for the inverse FFT. The FFTW library, used
//    in kdata to compute the DFT, searches for the fastest algorithm possible, so N*logN may not be exactly correct. But for 
//    comparison with method (b), this estimate is used. 
//   
//    In total, there are ~ 20 N + N log N operations using method a.  Then, one must search the output over the range (either N)
//    or between fOptimalFilterTimeRangeMin/Max in order to find the maximum amplitude, which is another R operations, where 
//    fOptimalFilterTimeRangeMax - fOptimalFilterTimeRangeMin =  R.
//
// b. direct integration
//    Instead of using FFTW to calculate the amplitude estimator for all pulse starting points (still with single bin accuracy), 
//    the integration in fourier space is done explicity, but only in the time range specified by fOptimalFilterTimeRangeMin/Max. 
//    For each pulse start time in R, there are roughly 15N + 10 operations. 
//    So, This will take R * (15N + 10) 
// 
//
// c. chi^2 minimization. 
//   For a pulse of length N = 512 (8192), the number of operations for method (a) is ~12k (~196k). It may be possible to use
//   a minimization algorithm to attain the best-fit pulse amplitude and start-time in a fewer number of operations. A number of 
//   minimization routines are available via ROOT. If you're using this class within Python, the Scipy/Numpy libraries also have
//   minimization routines. 
//   For this purpose, the chi^2 as a function of pulse amplitude and start-time is available. See the KOptimalKamper for
//   an example. Preliminary testing with a perfect filter (the noise power spectrum and pulse templates are exactly known) shows 
//   that the minimimum chi^2 value is reached when analyzing a pulse of significant amplitude (well above the noise) after ~25-100
//   calls of the chi^2 function. The chi^2 function requires ~35N operations.  Optimization of this method could reduce the number of
//   calls down to ~10N, perhaps. 
//
//
// Comparison of number of operations between methods.
//    Clearly, chi^2 minimization appears to require the largest number of operations. 
//    One can see that for all cases (except for R = 1), method b is slower than method a, and MUCH slower for reasonable values of R.
//    Even if one were to optimize the code such that R*(15N + 10) -> R*(5N + 10), method b is slower than a for R > 4. 
//
//
// The comments above were detemined by simply attempting to count the number of operations required for each line of code. There has
// been no actual time measurements done for these algorithms, so your mileage may vary.  When/If measurements become available, they
// should be documented here. 
//

#include "KOptimalFilter.h"
#include <iostream>
#include "KHalfComplexToRealDFT.h"
#include "KHalfComplexPower.h"
#include <cmath>
#include <set>

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

  fUseInverseFFT = true;  
  fOptimalFilterTimeRangeMin = 0;
  fOptimalFilterTimeRangeMax = -1;  //unless the user changes this, the range will go to the end of the output pulse.

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
  
  if (fOptimalFilterTimeRangeMax > 0  && 
      (fOptimalFilterTimeRangeMax < (int)fOptimalFilterTimeRangeMin ||
       fOptimalFilterTimeRangeMax > (int)fOutputSize)) {
    cerr << "koptimal filter. incorrect time range" << endl;
    return false;        
  } 

  if(fRecalculate){
    if(!BuildFilter())
      return false;
  }
  
  set<int> checkSizes;
  checkSizes.insert(fInputSize);
  checkSizes.insert(fOutputSize);
  checkSizes.insert(fOptFilterSize);
  checkSizes.insert(fOptFilterAndSignalSize);

  if (checkSizes.size() != 1){
    cout << "KOptimalFilter. Pulse Size Mismatch." << endl;
    for (set<int>::iterator it=checkSizes.begin(); it!=checkSizes.end(); ++it)
      cout << " " << *it;
    cout << endl;

    return false;
  }

  
  if(fUseInverseFFT) {
    //g(f) = h(f) * s(f)  where  s(f) = input, h(f) = optFilter
    fComplex.Multiply(fOptFilterAndSignal, fOptFilter, fInputPulse, fOptFilterAndSignalSize);
    return fHc2r->RunProcess();  //inverse fourier transform to get the amplitude estimator as a function of time.
  }



  //if, calculating the ingegral explicitly, 
  
  unsigned int max = fOutputSize;
  if(fOptimalFilterTimeRangeMax > 0 && fOptimalFilterTimeRangeMax < (int)fOutputSize)
    max = fOptimalFilterTimeRangeMax;
  

  double freqsum;

  for(unsigned int timeindex = fOptimalFilterTimeRangeMin; timeindex < max; timeindex++ ){

    //this has been distilled from the original discrete formula of the amplitude estimator for the optimal
    //filter, and exploits the hermiticity of the fourier-space arrays.    
    
    fOutputPulse[timeindex] = fComplex.Real(fOptFilter, fOptFilterSize, 0) * fComplex.Real(fInputPulse, fInputSize, 0)/fInputSize;
    freqsum = 0;
    //8 operations

    //plus N/2 * ( 14 + 8 + 10); 14 for the real*real +- imag*imam, 8 for getphase, ~10 for cos/sin
    // ~15 N 
    for(unsigned int k = 1; k < fInputSize/2 + 1; k++){
      freqsum += cos(GetPhase(k, timeindex))*(
          fComplex.Real(fOptFilter, fOptFilterSize, k)* fComplex.Real(fInputPulse, fInputSize, k)
            -
          fComplex.Imag(fOptFilter, fOptFilterSize, k)* fComplex.Imag(fInputPulse, fInputSize, k)
          )

        - sin(GetPhase(k,timeindex))*(
            fComplex.Real(fOptFilter, fOptFilterSize, k)*fComplex.Imag(fInputPulse, fInputSize, k)
            +
            fComplex.Imag(fOptFilter, fOptFilterSize, k)*fComplex.Real(fInputPulse, fInputSize, k)
          );
    }

    fOutputPulse[timeindex] += 2.0*freqsum/fInputSize;

    //4 operations

    //~15N + 12... or  ~15 N + 10
  }

  return true;

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
  
  // the optimal filter has a length of n elements. the first n/2 + 1 (0 to n/2) are the real parts
  // the remaining elements are the imaginary parts. but, because we are assuming that
  // our pulses are real (which they are), the 0th and n/2th components have an imaginary value
  // of zero and are therefore not included in this half-complex format.
  // the templateDFT has the same half-complex structure. The first n/2 + 1 (0 to n/2) elements correspond
  // to the lowest frequency component (DC) up to the highest (Nyquist). The corresponding frequency
  // component order is reveresed in the imaginary part of the half-complex array. The lowest frequency 
  // component (not DC!) in the imaginary part is the (n-1)st element of the array and the highest frequncy
  // component (also not Nyquist) in the imaginary part is the (n/2 + 1)st element. 
  // See the KRealToHalfComplexDFT, KHalfComplexToRealDFT and the FFTW documentation.
  //
  // the noise power, on the other hand, has a lengh of n/2+1 elements starting with the lowest
  // frequency component (DC) up to the highest (Nyquist).
  
  set<int> checkSizes;
  checkSizes.insert(2*(fNoiseSpectrumSize-1));
  checkSizes.insert(fOptFilterSize);
  checkSizes.insert(fTemplateDFTSize);
  checkSizes.insert(2*(fTemplatePowerSize-1));
  if (checkSizes.size() != 1){
    cout << "KOptimalFilter. Build Kernel. Pulse Size Mismatch." << endl;
    for (set<int>::iterator it=checkSizes.begin(); it!=checkSizes.end(); ++it)
      cout << ' ' << *it;
    cout << endl;

    return false;
  }

  
  //first calculate the denominator of the optimal filter
  fAmpEstimatorDenominator = 0;
  for (unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    fAmpEstimatorDenominator += *(fTemplatePower+i) / *(fNoiseSpectrum+i);
    
  //now caluclate the real parts of the optimal filter
  for(unsigned int i = 0; i <= fOptFilterSize/2; i++)
    *(fOptFilter+i) =  *(fTemplateDFT+i) / (fAmpEstimatorDenominator *  *(fNoiseSpectrum+i));
    
  //then the imaginary parts. make sure to use the correct index in the noise spectrum
  //the -1 in the imaginary part is because the optimal filter is proportional to the complex conjugate 
  //of the templateDFT
  unsigned int j = 1;
  for(unsigned int i = fOptFilterSize-1; i > fOptFilterSize/2; i--)
    *(fOptFilter+i) = -1.0* *(fTemplateDFT+i) / (fAmpEstimatorDenominator *  *(fNoiseSpectrum+(j++)));
  
  
  
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

double KOptimalFilter::GetChiSquareElement(double optimalAmp, double index, unsigned int freq_k, bool debug)
{

    double sig_re = fComplex.Real(fInputPulse, fInputSize, freq_k);
    double sig_im = fComplex.Imag(fInputPulse, fInputSize, freq_k);
    double temp_re = fComplex.Real(fTemplateDFT, fTemplateDFTSize, freq_k);
    double temp_im = fComplex.Imag(fTemplateDFT, fTemplateDFTSize, freq_k);
    
    double sig2 = sig_re*sig_re + sig_im*sig_im;
    double temp2 = temp_re*temp_re + temp_im*temp_im;
    
    double chi2 = (sig2 + optimalAmp*optimalAmp*temp2)/ fNoiseSpectrum[freq_k];
    double phase = GetPhase((double)freq_k, (double)index);

    chi2 -= 2.0*optimalAmp*(sig_re*temp_re + sig_im*temp_im)*cos( phase )/fNoiseSpectrum[freq_k];
    chi2 -= 2.0*optimalAmp*(temp_im*sig_re - sig_im*temp_re)*sin( phase )/fNoiseSpectrum[freq_k]; 

    if(debug){
      cout << "amp, index, freq_k, sig_re, sig_im, temp_re, temp_im, sig**2, temp**2, phase, cos(phase), sin(phase), noise[k]" << endl;
      cout << optimalAmp << " " << index << " " << freq_k << " " << sig_re << " " << sig_im << " " << " " << temp_re << " " << temp_im << " " << sig2 << " " << temp2 << " " << phase <<  " " << cos( phase ) << " " << sin(phase) << " " << fNoiseSpectrum[freq_k] << endl;
      cout << "chi2 line 1 " << (sig2 + optimalAmp*optimalAmp*temp2)/ fNoiseSpectrum[freq_k] << endl;
      cout << "chi2 line 2 " << -2.0*optimalAmp*(sig_re*temp_re + sig_im*temp_im)*cos( phase )/fNoiseSpectrum[freq_k] << endl;
      cout << "chi2 line 3 " << -2.0*optimalAmp*(temp_im*sig_re - sig_im*temp_re)*sin( phase )/fNoiseSpectrum[freq_k] << endl; 
    }

    return chi2;
}


double KOptimalFilter::GetChiSquared(double optimalAmp, double index)
{
  //
  
  //fOutputSize should equal fInputSize and fTemplateDFTSize.
  //while these are all in half-complex form, their size is equal to 
  //the size of the raw signal.
  //fNoiseSpectrumSize should equal fInputSize/2+1
  

  double chi2 = 0;
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    chi2 += GetChiSquareElement(optimalAmp, index, i);


  return 2.0 * chi2 / fInputSize;  //divide by N for proper normalization

}






