//______________________________________________________________________
//
// KRealToHalfComplexDFT.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// This processor finds the discrete fourier transform of a real 
// one-dimensionsal input pulse and returns a half-complex array. 
// The output pulse is in half-complex array format:
// real and imaginary parts for a transform of size n stored as 
//   (r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1)
// It uses the fftw3 libraries to calculate the transformation, using
// the r2r transformation 'plans'. 
// http://www.fftw.org/fftw3_doc/More-DFTs-of-Real-Data.html#More-DFTs-of-Real-Data
//
// BEGIN_LATEX
// #scale[1.5]{Y_{k} = #\sum_{j=0}^{n-1} X_{j} e^{-2 #pi j k #sqrt{-1}/n}}
// END_LATEX
//
// To use this processor
// 1. SetInputPulse(vector<double> aPulse); // or vector of floats, shorts, or ints. 
// 2. (optional). SetFlags(const char* ); // set a fftw specific flag. See the fftw documentation for details.
//                  The default option is FFTW_MEASURE. The options are 
//                         "ES" = FFTW_ESTIMATE
//                         "M"  = FFTW_MEASURE
//                         "P"  = FFTW_PATIENT
//                         "EX" = FFTW_EXHAUSTIVE
// 3. bool RunProcess()
// 4. vector<double> GetOutputPulse();
//
// To maximize the speed for DFT calculations, one should reuse 
// an instance of this processor as much as possible rather than 
// creating a new processor every time you want to make a single 
// calculation. The fftw3 libraries run faster when a 'plan' is reused.
//


#include "KRealToHalfComplexDFT.h"
#include <fftw3.h>

using namespace std;

//ClassImp(KRealToHalfComplexDFT);

KRealToHalfComplexDFT::KRealToHalfComplexDFT(void)
{
  SetName("KRealToHalfComplexDFT");
  InitializeMembers();
}

KRealToHalfComplexDFT::KRealToHalfComplexDFT(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KHalfComplexToRealDFT(inPulse, inSize, outPulse, outsize)
{
  SetName("KRealToHalfComplexDFT"); 
  InitializeMembers();
}

KRealToHalfComplexDFT::~KRealToHalfComplexDFT(void)
{

}

void KRealToHalfComplexDFT::InitializeMembers(void)
{

}

bool KRealToHalfComplexDFT::RunProcess(void)
{
  return CalculateFFT();

  // if(CalculateFFT())
  //   return Normalize();
  // else return false;

}
void KRealToHalfComplexDFT::SetFFTWPlan(void)
{
  //This will erase the contents of fIn_fft and fOut_fft. So make sure that these
  //arrays are filled with their initial values AFTER this method is called. 

  if(fInputPulse  && fOutputPulse )
    fPlan = (void*)fftw_plan_r2r_1d( (int)fInputSize, fInputPulse, fOutputPulse, FFTW_R2HC, MapFlag());
  else 
    cerr << "KRealToHalfComplexDFT::SetFFTWPlan. Arrays are empty." << endl;

}
