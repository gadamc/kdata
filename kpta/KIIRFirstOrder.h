//______________________________________________________________________
//
// KIIRFirstOrder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRFIRSTORDER_H__
#define __KIIRFIRSTORDER_H__

#include "KIIRFilter.h"

class KIIRFirstOrder : public KIIRFilter { 
  
public:
  //Constructors
  KIIRFirstOrder(void);
  KIIRFirstOrder(double a1, double b0, double b1);
  void SetCoefA(double a1);  
  void SetCoefB(double b0, double b1);  
  
  virtual ~KIIRFirstOrder(void);
  
	virtual bool RunProcess(void);
  
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KIIRFirstOrder,1);
  
};


#endif
