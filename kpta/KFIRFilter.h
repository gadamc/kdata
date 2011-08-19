//______________________________________________________________________
//
// KFIRFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KFIRFILTER_H__
#define __KFIRFILTER_H__

#include "KPtaProcessor.h"
#include <vector>

class KFIRFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KFIRFilter(void);
  virtual ~KFIRFilter(void);
  
	virtual bool RunProcess(void);
	
  virtual void SetCoefficients(vector<double> &coef);
private:
  double * fCoefficients;
  unsigned int fCoefSize;
  
  //private methods
  void InitializeMembers(void);
  
};


#endif
