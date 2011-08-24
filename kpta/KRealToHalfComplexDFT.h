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

#include "KPtaProcessor.h"
#include "KHalfComplexToRealDFT.h"

class KRealToHalfComplexDFT : public KHalfComplexToRealDFT  { 

public:
  //Constructors
  KRealToHalfComplexDFT(void);
  //for the memory-savy programmers
  KRealToHalfComplexDFT(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KRealToHalfComplexDFT(void);
	
	virtual bool RunProcess(void);
	virtual void SetFFTWPlan(void);
	
private:
	
	//private methods
  void InitializeMembers(void);

protected:
	
	
	//ClassDef(KRealToHalfComplexDFT,1);
  
};


#endif // __KREALTOHALFCOMPLEXDFT_H__
