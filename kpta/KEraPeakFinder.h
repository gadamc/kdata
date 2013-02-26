//______________________________________________________________________
//
// KEraPeakFinder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KERAPEAKFINDER_H__
#define __KERAPEAKFINDER_H__

#include "KOrderThresholdFinder.h"
#include "KRootMeanSquare.h"

class KEraPeakFinder : public KOrderThresholdFinder { 

public:
  //Constructors
  KEraPeakFinder(void);

  //for the memory-savy programmers
  KEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KEraPeakFinder(void);
  virtual bool RunProcess(void);

  virtual double GetBaselineStart(void) const {return fBaselineStart;}
  virtual double GetBaselineStop(void) const {return fBaselineStop;}
  virtual double GetNumRms(void) const {return fNumRms;}
  
  virtual void SetBaselineStart(double aVal) {fBaselineStart = aVal;}
  virtual void SetBaselineStop(double aVal) {fBaselineStop = aVal;}
  virtual void SetNumRms(double aVal) {fNumRms = aVal;}
  
private:

  double fBaselineStart;  //starting position of region to be averaged for baseline subtraction, in percent of the total pulse length
  double fBaselineStop; //stoping position of region to be averaged for baseline subtraction, in percent of the total pulse length
  double fNumRms;
  
  KRootMeanSquare fRms;
  
  //private methods
  void InitializeMembers(void);

  //ClassDef(KEraPeakFinder,1);
};


#endif // __KERAPEAKFINDER_H__
