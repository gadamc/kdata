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

class KResult {

public:
	KResult(std::string name, double value, std::string unit="");
	virtual ~KResult(void);
  std::string fName;
  double fValue;
  std::string fUnit;
  
};


#endif // __KResult_H_
