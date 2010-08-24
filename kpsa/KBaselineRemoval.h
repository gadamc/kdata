//______________________________________________________________________
//
// KBaselineRemoval.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KBASELINEREMOVAL_H__
#define __KBASELINEREMOVAL_H__

#include "KPsaProcessor.h"

class KBaselineRemoval : public KPsaProcessor { 

public:
  //Constructors
  KBaselineRemoval(void);
  virtual ~KBaselineRemoval(void);
	virtual bool RunProcess(void);
  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  //ClassDef(KBaselineRemoval,1);
};


#endif // __KBASELINEREMOVAL_H__
