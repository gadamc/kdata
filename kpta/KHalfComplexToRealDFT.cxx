//______________________________________________________________________
//
// KHalfComplexToRealDFT.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// This processor finds the inverse fourier transform of a half-complex 
// one-dimensionsal array and returns a real array. 
// A half-complex array of size n stored as 
//   (r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1)
// where r and i are the real and imaginary parts components, respectively.
// It uses the fftw3 libraries to calculate the transformation, useing
// the r2r transformation 'plans'. 
// http://www.fftw.org/fftw3_doc/More-DFTs-of-Real-Data.html#More-DFTs-of-Real-Data
//
//
// BEGIN_LATEX
// #scale[1.5]{Y_{k} = #frac{1}{N} \sum_{j=0}^{n-1} X_{j} e^{2 #pi j k #sqrt{-1}/n}}
// END_LATEX
//
// To use this processor
// 1. SetInputPulse(vector<double> aPulse); // or vector of floats, shorts, or ints in half-complex format. 
// 2. (optional). SetFlags(const char* ); // set a fftw specific flag. See the fftw documentation for details.
//                  The default option is FFTW_MEASURE. The options are 
//                         "ES" = FFTW_ESTIMATE
//                         "M"  = FFTW_MEASURE
//                         "P"  = FFTW_PATIENT
//                         "EX" = FFTW_EXHAUSTIVE
// 3. bool RunProcess()
// 4. vector<double> GetOutputPulse(); //returns a real array. 
//
// To maximize the speed for DFT calculations, one should reuse 
// an instance of this processor as much as possible rather than 
// creating a new processor every time you want to make a single 
// calculation. The fftw3 libraries run faster when a 'plan' is reused.
//


#include "KHalfComplexToRealDFT.h"
#include <fftw3.h>

using namespace std;

ClassImp(KHalfComplexToRealDFT);

KHalfComplexToRealDFT::KHalfComplexToRealDFT(void)
{
  SetName("KHalfComplexToRealDFT");
  InitializeMembers();	
}

KHalfComplexToRealDFT::KHalfComplexToRealDFT(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KHalfComplexToRealDFT"); 
  InitializeMembers();
  SetFFTWPlan();
}

KHalfComplexToRealDFT::~KHalfComplexToRealDFT(void)
{
  //since using fftw, destroy the pulses here with fftw_free
  //see the fftw documentation for why this is done
  if(!fDoNotDelete){
  
    if(fInputPulse != 0){
      fftw_free(fInputPulse);
      fInputPulse = 0;
      fInputSize = 0;
    }
    if(fOutputPulse != 0){
      fftw_free(fOutputPulse);
      fOutputPulse = 0;
      fOutputSize = 0;
    }
  }

  fftw_destroy_plan((fftw_plan)fPlan);

}

void KHalfComplexToRealDFT::InitializeMembers(void)
{
  fPlan = 0;
}

bool KHalfComplexToRealDFT::RunProcess(void)
{
  return CalculateFFT();

}

bool KHalfComplexToRealDFT::Normalize(void)
{
  //copies the normalized real array to the fOutputPulse.

  for(unsigned int i = 0; i < fOutputSize; i++)
    *(fOutputPulse+i) = *(fOutputPulse+i)/fOutputSize;  // this IS NORMALIZED

  return true;

}

bool KHalfComplexToRealDFT::CalculateFFT(void)
{

  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "KHalfComplexToRealDFT::CalculateFFT. input and output arrays have not been set." << endl;
    return false;
  }

  if(fPlan)
    fftw_execute((fftw_plan)fPlan);
  else {
    cerr << "KHalfComplexToRealDFT::CalculateFFT. The plan hasn't been initialized" << endl;
    return false;
  }

  return true;
}


void KHalfComplexToRealDFT::AllocateArrays(unsigned int size)
{
  //allocate memory for the input and output arrays. but use the fftw_malloc function
  //see the fftw documentation for why this is done

  if(fInputPulse)	fftw_free(fInputPulse);
  if(fOutputPulse) fftw_free(fOutputPulse);	
  fInputSize = fOutputSize = size;

  fInputPulse = (double*)fftw_malloc(sizeof(double)*fInputSize);
  fOutputPulse = (double*)fftw_malloc(sizeof(double)*fInputSize);

  SetFFTWPlan();  //reset the fftw plan with the new arrays. 
}


void KHalfComplexToRealDFT::SetFFTWPlan(void)
{
  //This will erase the contents of fIn_fft and fOut_fft. So make sure that these
  //arrays are filled with their initial values AFTER this method is called. 

  if(fInputPulse && fOutputPulse )
    fPlan = (void*)fftw_plan_r2r_1d( (int)fInputSize, fInputPulse, fOutputPulse, FFTW_HC2R, MapFlag());
  else 
    cerr << "KHalfComplexToRealDFT::SetFFTWPlan. Arrays are empty." << endl;


}

void KHalfComplexToRealDFT::SetFFTWFlag(const char* aFlag)
{
  //allowed options
  //"ES" == FFTW_ESTIMATE
  //"M" == FFTW_MEASURE
  //"P" == FFTW_PATIENT
  //"EX" == FFTW_EXHAUSTIVE
  //
  // These flags tell FFTW how much it should look for the very best
  // algorithm to make the caluclation of the DFT. ES has the lowest
  // overhead for searching for an algorithm, but the choice it makes
  // may be suboptimal. For repeated use of an instance of this object
  // FFTW_MEASURE is preferred, which is the default value.
  //

  if(aFlag != 0)
    fFFTWFlag = aFlag;
  else fFFTWFlag = "M";

  if(fFFTWFlag != "ES" && fFFTWFlag != "M" && fFFTWFlag != "P" && fFFTWFlag != "EX"){
    cerr << "KHalfComplexToRealDFT::SetOption. Invalid option: " << fFFTWFlag << endl;
    cerr << "    setting option to default value of 'M' "  << endl;
    fFFTWFlag = "M";
  }

}

unsigned int KHalfComplexToRealDFT::MapFlag(void)
{
  //allowed options
  //"ES" == FFTW_ESTIMATE
  //"M" == FFTW_MEASURE
  //"P" == FFTW_PATIENT
  //"EX" == FFTW_EXHAUSTIVE

  if(fFFTWFlag == "ES")
    return FFTW_ESTIMATE;
  if(fFFTWFlag == "M")
    return FFTW_MEASURE;
  if(fFFTWFlag == "P")
    return FFTW_PATIENT;
  if(fFFTWFlag == "EX")
    return FFTW_EXHAUSTIVE;

  return FFTW_ESTIMATE;
}
