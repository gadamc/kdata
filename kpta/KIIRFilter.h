//______________________________________________________________________
//
// KIIRFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRFILTER_H__
#define __KIIRFILTER_H__

#include "KPtaProcessor.h"

class KIIRFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KIIRFilter(void);
  //for the memory-savy programmers
  KIIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KIIRFilter(void);
  
	virtual bool RunProcess(void) = 0;
	
  void SetCoefficients(double* a, unsigned int asize, double* b, unsigned int bsize);
  
protected:
  double *fCoefA;
  unsigned int fCoefASize;
  double *fCoefB;
  unsigned int fCoefBSize;
  
private:
  //private methods
  void InitializeMembers(void);
  
 // ClassDef(KIIRFilter,1);
  
};


#endif
