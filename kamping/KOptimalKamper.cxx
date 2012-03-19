//
// KOptimalKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KOptimalKamper.h"


KOptimalKamper::KOptimalKamper(void)
{

}

KOptimalKamper::~KOptimalKamper(void)
{

}

Bool_t KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec)
{
  return MakeKamp(rawPulseRecord, rec, -1);
}

Bool_t KOptimalKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
  return true;
}

Bool_t KOptimalKamper::MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec)
{
  return MakeKamp(rawPulseRecord, rec, 100);
}
