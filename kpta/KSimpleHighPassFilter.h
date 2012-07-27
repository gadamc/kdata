//______________________________________________________________________
//
// KSimpleHighPassFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Monday 20. June 2011
//


#ifndef __KSIMPLEHIGHPASSFILTER_H__
#define __KSIMPLEHIGHPASSFILTER_H__

#include "KPtaProcessor.h"
#include "KSimpleLowPassFilter.h"


class KSimpleHighPassFilter : public KSimpleLowPassFilter  { 
  
public:
  //Constructors
  KSimpleHighPassFilter(void);
  //for the memory-savy programmers
  KSimpleHighPassFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  
  virtual ~KSimpleHighPassFilter(void);
  virtual bool RunProcess(void);
  
private:
 
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSimpleHighPassFilter,1);
  
};


#endif // __KSIMPLEHIGHPASSFILTER_H__
