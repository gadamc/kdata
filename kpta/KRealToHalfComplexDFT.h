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
  virtual ~KRealToHalfComplexDFT(void);
	
	virtual bool RunProcess(void);
	
private:
	
	//private methods
  void InitializeMembers(void);

protected:
	
	virtual void SetFFTWPlan(void);
	
	ClassDef(KRealToHalfComplexDFT,1);
  
};


#endif // __KREALTOHALFCOMPLEXDFT_H__
