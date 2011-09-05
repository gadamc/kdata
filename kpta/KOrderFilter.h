//______________________________________________________________________
//
// KOrderFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KORDERFILTER_H__
#define __KORDERFILTER_H__

#include "KPtaProcessor.h"

class KOrderFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KOrderFilter(void);
  KOrderFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);  
  virtual ~KOrderFilter(void);
  
  virtual void SetOrder(unsigned int a){fOrder = a;}  
  virtual unsigned int GetOrder(void) const {return fOrder;}
  
  virtual bool RunProcess(void);
  
private:
  unsigned int fOrder;
  //private methods
  void InitializeMembers(void);
    
};


#endif
