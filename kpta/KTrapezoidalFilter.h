//______________________________________________________________________
//
// KTrapezoidalFilter.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KTRAPEZOIDALFILTER_H__
#define __KTRAPEZOIDALFILTER_H__

#include "KPtaProcessor.h"
#include <string.h>

class KTrapezoidalFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KTrapezoidalFilter(void);
  //for the memory-savy programmers
  KTrapezoidalFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KTrapezoidalFilter(void);
  
	virtual bool RunProcess(void);
	
  void SetParams(double DecayTimeConstant, unsigned int RiseTime, unsigned int FlatTopWidth);

  virtual double GetDecayTimeConstant(void) {return fDecayTimeConstant;}
  virtual unsigned int GetRiseTime(void) {return fRiseTime;}
  virtual unsigned int GetFlatTopWidth(void) {return fFlatTopWidth;}

protected:
  double fDecayTimeConstant;
  unsigned int fRiseTime;
  unsigned int fFlatTopWidth;
  
private:
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KTrapezoidalFilter,1);
  
};


#endif
