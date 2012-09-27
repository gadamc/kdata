//______________________________________________________________________
//
// KIIRThirdOrder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRTHIRDORDER_H__
#define __KIIRTHIRDORDER_H__

#include "KIIRFilter.h"

class KIIRThirdOrder : public KIIRFilter { 
  
public:
  //Constructors
  KIIRThirdOrder(void);
  KIIRThirdOrder(double a1, double a2, double a3, double b0, double b1, double b2, double b3);
  void SetCoefA(double a1, double a2, double a3);  
  void SetCoefB(double b0, double b1, double b2, double b3);  
  
  virtual ~KIIRThirdOrder(void);
  
	virtual bool RunProcess(void);
  
private:
  
  //private methods
  void InitializeMembers(void);
  //ClassDef(KIIRThirdOrder,1);
    
};


#endif
