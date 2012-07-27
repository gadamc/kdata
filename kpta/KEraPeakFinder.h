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

#include "KPtaProcessor.h"
#include "KOrderFilter.h"
#include "KRootMeanSquare.h"
#include <vector>

class KEraPeakFinder : public KPtaProcessor { 

public:
  //Constructors
  KEraPeakFinder(void);

  //for the memory-savy programmers
  KEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KEraPeakFinder(void);
  virtual bool RunProcess(void);

  virtual bool SmoothPulse(void);
  virtual bool FindPeaks(void);
  
  virtual double GetBaselineStart(void) const {return fBaselineStart;}
  virtual double GetBaselineStop(void) const {return fBaselineStop;}
  virtual int GetOrder(void) const {return fOrder;}
  virtual double GetNumRms(void) const {return fNumRms;}
  virtual double GetPolarity(void) const {return fPolarity;}
  virtual std::vector<int>& GetPeakBins(void) {return fPeakBins;}
  
  virtual void SetBaselineStart(double aVal) {fBaselineStart = aVal;}
  virtual void SetBaselineStop(double aVal) {fBaselineStop = aVal;}
  virtual void SetOrder(int aVal) {fOrder = aVal;}
  virtual void SetNumRms(double aVal) {fNumRms = aVal;}
  virtual void SetPolarity(int aVal) {fPolarity = aVal;}
  
private:

  double fBaselineStart;  //starting position of region to be averaged for baseline subtraction, in percent of the total pulse length
  double fBaselineStop; //stoping position of region to be averaged for baseline subtraction, in percent of the total pulse length
  int fOrder;
  double fNumRms;
  int fPolarity;
  std::vector<int> fPeakBins;
  
  KOrderFilter fOrderFilter;
  KRootMeanSquare fRms;
  
  //private methods
  void InitializeMembers(void);

  ClassDef(KEraPeakFinder,1);
};


#endif // __KERAPEAKFINDER_H__
