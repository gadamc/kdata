//______________________________________________________________________
//
// KHalfComplexPower.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KHALFCOMPLEXPOWER_H__
#define __KHALFCOMPLEXPOWER_H__

#include "KPtaProcessor.h"

class KHalfComplexPower : public KPtaProcessor { 

public:
  //Constructors
  KHalfComplexPower(void);
  //for the memory-savy programmers
  KHalfComplexPower(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KHalfComplexPower(void);

  virtual bool RunProcess(void);
  
  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KHalfComplexPower,1);
  
};


#endif // __KHALFCOMPLEXPOWER_H__
