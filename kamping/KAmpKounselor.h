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
class KDataReader;
class KDataWriter;

class KAmpKounselor {

public:
  
  KAmpKounselor(void);
  virtual ~KAmpKounselor(void);
  virtual void AddKAmpSite(KAmpSite *aSite);
  virtual void SetFiles(const char* inputRawKDataFile, const char* outputAmpKDataFile);
  virtual Bool_t RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile);
  virtual Bool_t Scout();
  virtual Bool_t Prepare();
  virtual Bool_t Run();
  //virtual std::vector<KAmpSite *> GetListOfKAmpSites(void){ return fKampSites;}
  
private:
  std::vector<KAmpSite *> fKampSites;
  KDataReader *fInput;
  KDataWriter *fOutput;
  
  Bool_t CheckSetup();

};


#endif // __KAMPKOUNSELOR_H_
