//
// KResult.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KResult_H_
#define __KResult_H_

#include "Rtypes.h"
#include <string>
#include <map>
#include <vector>

class KRawBoloPulseRecord;
class KPulseAnalysisRecord;

class KResult {

public:
  std::string fName;
  double fValue;
  std::string fUnit;
private:
  
};


#endif // __KResult_H_
