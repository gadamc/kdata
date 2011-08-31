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
#include <string.h>

class KIIRFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KIIRFilter(void);
  //for the memory-savy programmers
  KIIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KIIRFilter(void);
  
	virtual bool RunProcess(void) = 0;
	
  void SetCoefficients(double* a, unsigned int asize, double* b, unsigned int bsize);

  virtual unsigned int GetCoefASize(void) {return fCoefASize;}
  virtual unsigned int GetCoefBSize(void) {return fCoefBSize;}
  virtual double* GetCoefA(void) {return fCoefA;}
  virtual double* GetCoefB(void) {return fCoefB;}

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
