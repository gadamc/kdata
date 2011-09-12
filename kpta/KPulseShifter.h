//______________________________________________________________________
//
// KPulseShifter.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPULSESHIFTER_H__
#define __KPULSESHIFTER_H__

#include "KPtaProcessor.h"
#include <string.h>

class KPulseShifter : public KPtaProcessor { 

public:
  //Constructors
  KPulseShifter(void);
  //for the memory-savy programmers
  KPulseShifter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KPulseShifter(void);

  virtual bool RunProcess(void);

	virtual void SetShift(const int shift){ fPulseShift = shift;};
  virtual int GetShift(void) {return fPulseShift;};
  
protected:
  int fPulseShift;

private:
  //private methods
  void InitializeMembers(void);

 // ClassDef(KPulseShifter,1);

};


#endif
