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

#include "KPsaProcessor.h"

class KHalfComplexPower : public KPsaProcessor { 

public:
  //Constructors
  KHalfComplexPower(void);
  virtual ~KHalfComplexPower(void);

	virtual bool RunProcess(void);
	
  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

};


#endif // __KHALFCOMPLEXPOWER_H__
