//______________________________________________________________________
//
// KNullProcessor.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KNULLPROCESSOR_H__
#define __KNULLPROCESSOR_H__

#include "KPtaProcessor.h"

class KNullProcessor : public KPtaProcessor { 

public:
  //Constructors
  KNullProcessor(void);

  virtual ~KNullProcessor(void);
  virtual bool RunProcess(void);

  virtual void SetInputPulseSize(unsigned int s){fOutputSize = fInputSize = s;}
  virtual void SetOutputPulseSize(unsigned int /*s*/){}
  virtual void DeleteInputPulse(void){if(fInputPulse){delete [] fInputPulse; fOutputPulse = fInputPulse = 0; fOutputSize = fInputSize = 0;}}
  virtual void DeleteOutputPulse(void){}

  virtual void AllocateArrays(unsigned int size);

private:

 
  //private methods
  void InitializeMembers(void);

  //ClassDef(KNULLPROCESSOR,1);
};


#endif // __KNULLPROCESSOR_H__
