//______________________________________________________________________
//
// KHalfComplexToRealDFT.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KHALFCOMPLEXTOREALDFT_H__
#define __KHALFCOMPLEXTOREALDFT_H__

#include "KPtaProcessor.h"

class KHalfComplexToRealDFT : public KPtaProcessor  { 
	
public:
  //Constructors
  KHalfComplexToRealDFT(void);
  virtual ~KHalfComplexToRealDFT(void);
	
	virtual bool RunProcess(void);
  
	virtual void SetFFTWFlag(const char* aFlag = 0);
	virtual const char* GetFFTWFlag(void){return fFFTWFlag.c_str();}
	
private:
	//private methods
  void InitializeMembers(void);
 
protected:
	void* fPlan; //points to the fftw_plan 
  std::string fFFTWFlag;
  
	virtual unsigned int MapFlag(void);
	virtual void AllocateArrays(unsigned int size);
	virtual void SetFFTWPlan(void);
	virtual bool Normalize(void);
	virtual bool CalculateFFT(void);
	
	//ClassDef(KHalfComplexToRealDFT,1);
  
};


#endif // __KHALFCOMPLEXTOREALDFT_H__
