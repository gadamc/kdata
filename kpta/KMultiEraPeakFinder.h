//______________________________________________________________________
//
// KMultiEraPeakFinder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KMULTIERAPEAKFINDER_H__
#define __KMULTIERAPEAKFINDER_H__

#include "KPtaProcessor.h"
#include "KEraPeakFinder.h"
#include <vector>

class KMultiEraPeakFinder : public KPtaProcessor { 

public:
  //Constructors
  KMultiEraPeakFinder(void);

  //for the memory-savy programmers
  KMultiEraPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KMultiEraPeakFinder(void);
  virtual bool RunProcess(void){return true;}

  // virtual bool SmoothPulse(void);
  //   virtual bool FindPeaks(void);
  //   
  //   virtual double GetBaselineStart(unsigned int i) const;
  //   virtual double GetBaselineStop(unsigned int i) const;
  //   virtual int GetOrder(unsigned int i) const;
  //   virtual double GetNumRms(unsigned int i) const;
  //   virtual double GetPolarity(unsigned int i) const;
  //   virtual std::vector<int>& GetPeakBins(unsigned int i) const;
  //   
  //   virtual void SetBaselineStart(unsigned int i, double aVal);
  //   virtual void SetBaselineStop(unsigned int i, double aVal);
  //   virtual void SetOrder(unsigned int i, int aVal);
  //   virtual void SetNumRms(unsigned int i, double aVal);
  //   virtual void SetPolarity(unsigned int i, int aVal);
  
private:
  
  std::vector<KEraPeakFinder> fPeakFinders;
  
  //private methods
  void InitializeMembers(void);
  ClassDef(KMultiEraPeakFinder,1);

};


#endif // __KMULTIERAPEAKFINDER_H__
