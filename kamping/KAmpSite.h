//
// KAmpSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KAMPSITE_H_
#define __KAMPSITE_H_

#include "Rtypes.h"


class KAmpSite {

public:
  KAmpSite(void);
  virtual ~KAmpSite(void);
  
  virtual Bool_t RunKampSite(void) = 0;

private:

  ClassDef(KAmpSite, 1);
};


#endif // __KAMPSITE_H_
