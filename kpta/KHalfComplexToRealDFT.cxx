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
// However, there is a difference in this class. The output array is "normalized" by 1/N
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
// calculation. 


#include "KHalfComplexToRealDFT.h"
#include <fftw3.h>

KHalfComplexToRealDFT::KHalfComplexToRealDFT(void)
{
	SetName("KHalfComplexToRealDFT");
  InitializeMembers();
	
	
}

KHalfComplexToRealDFT::~KHalfComplexToRealDFT(void)
{
	if(fIn_fft != 0){
		fftw_free(fIn_fft);
		fIn_fft = 0;
	}
	if(fOut_fft != 0){
		fftw_free(fOut_fft);
		fOut_fft = 0;
	}
	
	fftw_destroy_plan((fftw_plan)fPlan);
}

void KHalfComplexToRealDFT::InitializeMembers(void)
{
 	fIn_fft = fOut_fft = 0;
	fPlan = 0;
	SetFFTWFlag(); //default option is to measure. 
}

bool KHalfComplexToRealDFT::RunProcess(void)
{
	if(CalculateFFT()) 		
		return CopyArrayToOutput();
	
	else return false;
}

bool KHalfComplexToRealDFT::CopyArrayToOutput(void)
{
	try{
		for(unsigned int i = 0; i < fOutputPulse.size(); i++){
			fOutputPulse.at(i) = fOut_fft[i]/fOutputPulse.size();  // this IS NORMALIZED
		}
	}
	catch (out_of_range &e) {
		//I think this should be impossible... 
		cerr << "KHalfComplexToRealDFT::RunProcess. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
		return false;
	}
	
	return true;
	
}

bool KHalfComplexToRealDFT::CalculateFFT(void)
{
	if(fIn_fft == 0 || fOut_fft == 0) {
		cerr << "KHalfComplexToRealDFT::CalculateFFT. Arrays for FFT have not been set." << endl;
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

void KHalfComplexToRealDFT::SetInputPulse(const vector<double> &aPulse)
{
	//Set the input pulse See the base class
	//KPtaProcessor::SetInputPulse(const vector<double> &aPulse)
	
	bool reAllocate = false;
	if(aPulse.size() != fInputPulse.size())
		reAllocate = true;
	
	//set it using the base class.
	KPtaProcessor::SetInputPulse(aPulse);
	fOutputPulse.resize(fInputPulse.size(),0);
	
	if(reAllocate) 
		AllocateFFTArrays();
	FillFFTArrays();
}

void KHalfComplexToRealDFT::SetInputPulse(const vector<short> &aPulse)
{
	//Set the input pulse. See the base class
	//KPtaProcessor::SetInputPulse(const vector<short> &aPulse)
	
	bool reAllocate = false;
	if(aPulse.size() != fInputPulse.size())
		reAllocate = true;
	
	//set it using the base class.
	KPtaProcessor::SetInputPulse(aPulse);
	fOutputPulse.resize(fInputPulse.size(),0);
	
	if(reAllocate) 
		AllocateFFTArrays();
	FillFFTArrays();
	
}

void KHalfComplexToRealDFT::SetInputPulse(const vector<float> &aPulse)
{ 
	//Set the input pulse See the base class
	//KPtaProcessor::SetInputPulse(const vector<float> &aPulse)
	
	bool reAllocate = false;
	if(aPulse.size() != fInputPulse.size())
		reAllocate = true;
	
	//set it using the base class.
	KPtaProcessor::SetInputPulse(aPulse);
	fOutputPulse.resize(fInputPulse.size(),0);
	
	if(reAllocate) 
		AllocateFFTArrays();
	FillFFTArrays();
} 

void KHalfComplexToRealDFT::SetInputPulse(const vector<int> &aPulse)
{
	//Set the input pulse See the base class
	//KPtaProcessor::SetInputPulse(const vector<int> &aPulse)
	
	bool reAllocate = false;
	if(aPulse.size() != fInputPulse.size())
		reAllocate = true;
	
	//set it using the base class.
	KPtaProcessor::SetInputPulse(aPulse);
	fOutputPulse.resize(fInputPulse.size(),0);
	
	if(reAllocate) 
		AllocateFFTArrays();
	FillFFTArrays();
}

void KHalfComplexToRealDFT::SetInputPulse(const char* aFile)
{
	//Set the input pulse. See the base class
	//KPtaProcessor::SetInputPulse(const char* aFile)
	
	//There's no way of knowing the size of the pulse, so we must
	//reallocate the FFT arrays. 
	bool reAllocate = true;
	
	//set it using the base class.
	KPtaProcessor::SetInputPulse(aFile);
	fOutputPulse.resize(fInputPulse.size(),0);
	
	if(reAllocate) 
		AllocateFFTArrays();
	FillFFTArrays();
}

void KHalfComplexToRealDFT::AllocateFFTArrays(void)
{
	if(fIn_fft != 0){
		fftw_free(fIn_fft);
		fIn_fft = 0;
	}
	if(fOut_fft != 0){
		fftw_free(fOut_fft);
		fOut_fft = 0;
	}
	
	fIn_fft = (double*)fftw_malloc(sizeof(double)*fInputPulse.size());
	fOut_fft = (double*)fftw_malloc(sizeof(double)*fInputPulse.size());
	
	SetFFTWPlan();  //reset the fftw plan with the new arrays. 
}

void KHalfComplexToRealDFT::FillFFTArrays(void)
{
	//you must make sure that the fIn_fft and fOut_fft
	//arrays are of the same size as the fInputPulse vector
	//or this will cause a crash.
	
	if(fIn_fft == 0 || fOut_fft == 0) return;
	
	try{
		for(unsigned int i = 0; i < fInputPulse.size(); i++){
			fIn_fft[i] = fInputPulse.at(i);
			fOut_fft[i] = 0;
		}
	}
	catch (out_of_range &e) {
		//I think this should be impossible... 
		cerr << "KHalfComplexToRealDFT::FillFFTArrays. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
	}
	
}

void KHalfComplexToRealDFT::SetFFTWPlan(void)
{
	//This will erase the contents of fIn_fft and fOut_fft. So make sure that these
	//arrays are filled with their initial values AFTER this method is called. 
	
	if(fIn_fft != 0 && fOut_fft != 0)
		fPlan = (void*)fftw_plan_r2r_1d( (int)fInputPulse.size(), fIn_fft, fOut_fft, FFTW_HC2R, MapFlag());
	else {
		cerr << "KHalfComplexToRealDFT::SetFFTWPlan. Arrays are empty." << endl;
	}
	
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

