//______________________________________________________________________
//
// KBaselineRemoval.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KBASELINEREMOVAL_H__
#define __KBASELINEREMOVAL_H__

#include "KPtaProcessor.h"

class KBaselineRemoval : public KPtaProcessor { 

public:
  //Constructors
  KBaselineRemoval(void);

  //for the memory-savy programmers
  KBaselineRemoval(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KBaselineRemoval(void);
  virtual bool RunProcess(void);

  virtual bool CalculateAverage(void);
  virtual bool Subtract(double aVal);
  virtual void Subtract(double aVal, unsigned int i);
  
  virtual double GetBaselineStart(void) const {return fBaselineStart;}
  virtual double GetBaselineStop(void) const {return fBaselineStop;}

  virtual void SetBaselineStart(double aVal) {fBaselineStart = aVal;}
  virtual void SetBaselineStop(double aVal) {fBaselineStop = aVal;}

  virtual double GetBaselineOffset(void) const {return fBaseline;}
private:

  double fBaselineStart;  //starting position of region to be averaged for baseline subtraction, in percent of the total pulse length
  double fBaselineStop; //stoping position of region to be averaged for baseline subtraction, in percent of the total pulse length

  double fBaseline;
   
  //private methods
  void InitializeMembers(void);

  //ClassDef(KBaselineRemoval,1);
};


#endif // __KBASELINEREMOVAL_H__
