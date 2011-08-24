//______________________________________________________________________
//
// KIIRSecondOrder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRSECONDORDER_H__
#define __KIIRSECONDORDER_H__

#include "KIIRFilter.h"

class KIIRSecondOrder : public KIIRFilter { 
  
public:
  //Constructors
  KIIRSecondOrder(void);
  KIIRSecondOrder(double a1, double a2, double b0, double b1, double b2);
  void SetCoefA(double a1, double a2);  
  void SetCoefB(double b0, double b1, double b2);  
  
  virtual ~KIIRSecondOrder(void);
  
	virtual bool RunProcess(void);
  
private:
  
  //private methods
  void InitializeMembers(void);
    
};


#endif
