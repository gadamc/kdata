//______________________________________________________________________
//
// KLowPassFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Monday 20. June 2011
//


#ifndef __KLOWPASSFILTER_H__
#define __KLOWPASSFILTER_H__

#include "KPtaProcessor.h"

class KLowPassFilter : public KPtaProcessor  { 

public:
  //Constructors
  KLowPassFilter(void);
  virtual ~KLowPassFilter(void);
  virtual bool RunProcess(void);
  
  //getters

  //setters
private:
  
  //private methods
  void InitializeMembers(void);

};


#endif // __KLOWPASSFILTER_H__
