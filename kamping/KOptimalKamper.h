//
// KOptimalKamper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KOPTIMALKAMPER_H_
#define __KOPTIMALKAMPER_H_

#include "KAmper.h"
#include "KOptimalFilter.h"

class KOptimalKamper : KAmper {

public:
  KOptimalKamper(void);
  virtual ~KOptimalKamper(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);

  KOptimalFilter& GetOptimalFilter(void){return fOptimalFilter;}
  
private:
 
  KOptimalFilter fOptimalFilter;
  
  
};


#endif // __KOPTIMALKAMPER_H_
