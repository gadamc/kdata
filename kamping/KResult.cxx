//
// KResult.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KResult.h"
KResult::KResult()
{
  fName = "";
  fValue = 0;
  fUnit = "";
}


KResult::KResult(std::string name, double value, std::string unit)
{
	fName = name;
  fValue = value;
  fUnit = unit;
}

KResult::~KResult(void)
{

}


