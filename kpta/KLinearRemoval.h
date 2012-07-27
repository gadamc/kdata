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

  virtual bool CalculateLine(void);
  virtual bool Subtract(void);
  virtual void Subtract(double aSlope, double aOffset, unsigned int i);
  
  virtual double GetBaselineStop(void) const {return fBaselineStop;}
  virtual void SetBaselineStop(double aVal) {fBaselineStop = aVal;}
  virtual void SetSlope(double slope){fSlope = slope;}
  virtual void SetOffset(double offset){fOffset = offset;}
  virtual double GetSlope(void) const {return fSlope;}
  virtual double GetOffset(void) const {return fOffset;}
  virtual void SetDefaultSettings(void);
  
private:

  double fBaselineStop; //stoping position of region to be averaged for line subtraction, in percent of the total pulse length
  double fSlope;  //calculated slope of the line. 
  double fOffset;  //calculated offset
  
  //private methods
  void InitializeMembers(void);
  ClassDef(KLinearRemoval,1);

};


#endif // __KLINEARREMOVAL_H__
