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

class KSimpleHighPassFilter : public KPtaProcessor  { 
  
public:
  //Constructors
  KSimpleHighPassFilter(void);
  virtual ~KSimpleHighPassFilter(void);
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


#endif // __KSIMPLEHIGHPASSFILTER_H__
