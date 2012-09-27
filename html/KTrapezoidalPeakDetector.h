//______________________________________________________________________
//
// KTrapezoidalPeakDetector.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KTRAPZOIDALPEAKDETECTOR_H__
#define __KTRAPZOIDALPEAKDETECTOR_H__

#include "KPtaProcessor.h"
#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include <string.h>
#include <vector>

class KTrapezoidalPeakDetector : public KPtaProcessor { 
  
public:
  //Constructors
  KTrapezoidalPeakDetector(void);
  //for the memory-savy programmers
  KTrapezoidalPeakDetector(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KTrapezoidalPeakDetector(void);
  
	virtual bool RunProcess(void);
	
  virtual void SetDecayTimeConstant(double aDecayTimeConstant) {fDecayTimeConstant = aDecayTimeConstant;}
  virtual void SetThreshold(double aThreshold) {fThreshold = aThreshold;}
  
  virtual void AddTrapFilter(unsigned int aRiseTime, unsigned int aFlatTopWidth);

  virtual double GetDecayTimeConstant(void) {return fDecayTimeConstant;}
  
  //debug:
  double* GetDerivative(void){return fDerivative;}
  

protected:
  double fDecayTimeConstant; // decay time constant of the input pulses
	std::vector<KTrapezoidalFilter* > fTrapFilters;
  
  //for debugging only:
  double *fDerivative;
  double *fCorrelation;
  KOrderFilter fOrderFilter1;
  KOrderFilter fOrderFilter2;
  unsigned int maxWidth;
  double fThreshold;
private:
  //private methods
  void InitializeMembers(void);
  
  //ClassDef(KTrapezoidalPeakDetector,1);
  
};


#endif
