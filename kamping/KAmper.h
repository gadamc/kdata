//
// KAmper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KAMPER_H_
#define __KAMPER_H_

#include "Rtypes.h"

class KAmper {

public:
  KAmper(void);
  virtual ~KAmper(void);
  
  virtual Bool_t MakeKamp(void) = 0;

private:

  ClassDef(KAmper, 1);
};


#endif // __KAMPER_H_
