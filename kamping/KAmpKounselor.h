//
// KAmpKounselor.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KAMPKOUNSELOR_H_
#define __KAMPKOUNSELOR_H_

#include "Rtypes.h"
#include <vector>

class KAmpSite;

class KAmpKounselor {

public:
  
  KAmpKounselor(void);
  virtual ~KAmpKounselor(void);
  virtual void AddKAmpSite(KAmpSite *aSite);
  virtual Bool_t RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile);
  //virtual std::vector<KAmpSite *> GetListOfKAmpSites(void){ return fKampSites;}
  
private:
  std::vector<KAmpSite *> fKampSites;
};


#endif // __KAMPKOUNSELOR_H_
