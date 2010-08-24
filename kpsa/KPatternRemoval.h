//______________________________________________________________________
//
// KPatternRemoval.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPATTERNREMOVAL_H__
#define __KPATTERNREMOVAL_H__

#include "KPsaProcessor.h"

class KPatternRemoval : public KPsaProcessor { 

public:
  //Constructors
  KPatternRemoval(void);
  virtual ~KPatternRemoval(void);
	virtual bool RunProcess(void);

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  //ClassDef(KPatternRemoval,1);
};


#endif // __KPATTERNREMOVAL_H__
