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

#include <string>
#include <map>
#include "KPulseAnalysisRecord.h"
class KPulseAnalysisRecord;

class KResult {

public:
  KResult();
	KResult(std::string name, double value, std::string unit="");
	virtual ~KResult(void);
  std::string fName;
  double fValue;
  std::string fUnit;
  
  // void fill( void (KPulseAnalysisRecord::*Method)(double), KPulseAnalysisRecord* rec, 
  //             std::map<string, KResult>& theResult, string& theField );

};


#endif // __KResult_H_
