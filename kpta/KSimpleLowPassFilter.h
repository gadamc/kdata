//______________________________________________________________________
//
// KSimpleLowPassFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Monday 20. June 2011
//


#ifndef __KSIMPLELOWPASSFILTER_H__
#define __KSIMPLELOWPASSFILTER_H__

#include "KPtaProcessor.h"

class KSimpleLowPassFilter : public KPtaProcessor  { 

public:
  //Constructors
  KSimpleLowPassFilter(void);
  virtual ~KSimpleLowPassFilter(void);
  virtual bool RunProcess(void);
  
  double GetRc(void){return fRc;}
  void SetRc(double aVal){fRc = aVal;}
  
  double GetMinRCToBinRatio(void){return fMinRCToBinRatio;}
  void SetMinRCToBinRatio(double aVal){fMinRCToBinRatio = aVal;}
  
private:
  double fRc;  //should be in units of the time width of each sample of your pulse
  double fMinRCToBinRatio;
  
  //private methods
  void InitializeMembers(void);
  
};


#endif // __KSIMPLELOWPASSFILTER_H__
