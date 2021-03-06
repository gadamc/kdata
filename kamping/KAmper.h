//_____________________________________________
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
#include <string>
#include "KResult.h"

class KRawBoloPulseRecord;

class KAmper {

public:
  KAmper(void);
  virtual ~KAmper(void);
  
  virtual std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord) = 0;
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
private:
  std::string fName;
};


#endif // __KAMPER_H_
