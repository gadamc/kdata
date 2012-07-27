//______________________________________________________________________
//
// KIIRFourthOrder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRFOURTHORDER_H__
#define __KIIRFOURTHORDER_H__

#include "KIIRFilter.h"

class KIIRFourthOrder : public KIIRFilter { 
  
public:
  //Constructors
  KIIRFourthOrder(void);
  KIIRFourthOrder(double a1, double a2, double a3, double a4, double b0, double b1, double b2, double b3, double b4);
  void SetCoefA(double a1, double a2, double a3, double a4);  
  void SetCoefB(double b0, double b1, double b2, double b3, double b4);  
  
  virtual ~KIIRFourthOrder(void);
  
	virtual bool RunProcess(void);
  
private:
  
  //private methods
  void InitializeMembers(void);
  ClassDef(KIIRFourthOrder,1);

};


#endif
