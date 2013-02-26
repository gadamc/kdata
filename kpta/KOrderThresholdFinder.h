//______________________________________________________________________
//
// KOrderThresholdFinder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KORDERTHRESHOLDFINDER_H__
#define __KORDERTHRESHOLDFINDER_H__

#include "KPtaProcessor.h"
#include "KOrderFilter.h"
#include "KRootMeanSquare.h"
#include <vector>

class KOrderThresholdFinder : public KPtaProcessor { 

public:
  //Constructors
  KOrderThresholdFinder(void);

  //for the memory-savy programmers
  KOrderThresholdFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KOrderThresholdFinder(void);
  virtual bool RunProcess(void);

  virtual bool SmoothPulse(void);
  virtual bool FindPeaks(void);

  virtual int GetOrder(void) const {return fOrderFilter.GetOrder();}
  virtual double GetThreshold(void) const {return fThreshold;}
  virtual double GetPolarity(void) const {return fPolarity;}
  virtual std::vector<int>& GetPeakBins(void) {return fPeakBins;}
  

  virtual void SetOrder(int aVal) {fOrderFilter.SetOrder(aVal);}
  virtual void SetThreshold(double aVal) {fThreshold = aVal;}
  virtual void SetPolarity(int aVal) {fPolarity = aVal;}
  
private:

  double fThreshold; //if a sample is found greater than this value, the sample index number is added to fPeakBins
  int fPolarity; //if set to 0, then this object searches for pulses in both directions. otherwise, it only looks for the polarity you give it
  std::vector<int> fPeakBins;
  
  KOrderFilter fOrderFilter;
  
  //private methods
  void InitializeMembers(void);

};


#endif // __KORDERTHRESHOLDFINDER_H__
