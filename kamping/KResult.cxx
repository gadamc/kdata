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

// void KResult::fill( void(KPulseAnalysisRecord::*Method)(double), KPulseAnalysisRecord* rec, 
// 	std::map<string, KResult>& theResult, string& theField)
// {
// 	if( theResult.find(theField.c_str()) != theResult.end() )
//       rec->*Method( theResult[theField.c_str()].fValue );
// }
