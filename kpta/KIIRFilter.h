//______________________________________________________________________
//
// KIIRFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KIIRFILTER_H__
#define __KIIRFILTER_H__

#include "KPtaProcessor.h"

class KIIRFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KIIRFilter(void);
  virtual ~KIIRFilter(void);
  
	virtual bool RunProcess(void);
	
  //getters
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KIIRFilter,1);
  
};


#endif
