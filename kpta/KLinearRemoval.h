//______________________________________________________________________
//
// KLinearRemoval.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KLINEARREMOVAL_H__
#define __KLINEARREMOVAL_H__

#include "KPtaProcessor.h"

class KLinearRemoval : public KPtaProcessor { 

public:
  //Constructors
  KLinearRemoval(void);

  //for the memory-savy programmers
  KLinearRemoval(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KLinearRemoval(void);
  virtual bool RunProcess(void);

  double CalculateSlope(void);
  virtual bool Subtract(double aSlope);
  virtual void Subtract(double aSlope, unsigned int i);
  
  double GetBaselineStop(void) const {return fBaselineStop;}
  virtual void SetBaselineStop(double aVal) {fBaselineStop = aVal;}

private:

  double fBaselineStop; //stoping position of region to be averaged for line subtraction, in percent of the total pulse length
  double fSlope;  //slope of the line. the intercept is the first element of the input pulse
  
  //private methods
  void InitializeMembers(void);

};


#endif // __KLINEARREMOVAL_H__
