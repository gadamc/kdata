//______________________________________________________________________
//
// KRealToHalfComplexDFT.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KREALTOHALFCOMPLEXDFT_H__
#define __KREALTOHALFCOMPLEXDFT_H__

#include "KPsaProcessor.h"

class KRealToHalfComplexDFT : public KPsaProcessor  { 

public:
  //Constructors
  KRealToHalfComplexDFT(void);
  virtual ~KRealToHalfComplexDFT(void);
	
	virtual bool RunProcess(void);
	
  //setters
	virtual void SetInputPulse(const vector<double> &aPulse); //Set the input pulse
	virtual void SetInputPulse(const vector<short> &aPulse); //Set the input pulse 
	virtual void SetInputPulse(const vector<float> &aPulse); //Set the input pulse
	virtual void SetInputPulse(const vector<int> &aPulse); //Set the input pulse
	virtual void SetInputPulse(const char* aFile);

	virtual void SetFFTWFlag(const char* aFlag = 0);
	
	//getters
	virtual const double* GetIn_fft(void)const {return fIn_fft;}
	virtual const double* GetOut_fft(void)const {return fOut_fft;}
	virtual const char* GetFFTWFlag(void){return fFFTWFlag.c_str();}
	
private:
	
	void* fPlan; //points to the fftw_plan 
	double *fIn_fft;  //all real input
	double *fOut_fft;  //in half-complex format
  string fFFTWFlag;
	
	//private methods
  void InitializeMembers(void);

protected:
	
	virtual unsigned int MapFlag(void);
	virtual void AllocateFFTArrays(void);
	virtual void FillFFTArrays(void);
	virtual void SetFFTWPlan(void);
	virtual bool CopyArrayToOutput(void);
	virtual bool CalculateFFT(void);
	
};


#endif // __KREALTOHALFCOMPLEXDFT_H__
