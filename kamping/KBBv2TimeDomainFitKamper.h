//
// KBBv2TimeDomainFitKamper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBBV2TIMEDOMAINFITKAMPER_H_
#define __KBBV2TIMEDOMAINFITKAMPER_H_

#include "KAmper.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KIIRFourthOrder.h"
#include "KLinearRemoval.h"
#include "TGraph.h"
#include "KPulsePolarityCalculator.h"

class TF1;

class KBBv2TimeDomainFitKamper : public KAmper{

public:
  KBBv2TimeDomainFitKamper(void);
  virtual ~KBBv2TimeDomainFitKamper(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
   
  KIIRFourthOrder& GetIIR(void) {return fIrrFour;}
  KLinearRemoval& GetLinearRemoval(void) {return fLineRemove;}
  TGraph& GetGraph(void) {return fData;}
  TF1* GetPositiveFitFunction(void){return fPositiveFit;}
  TF1* GetNegativeFitFunction(void){return fNegativeFit;}
  
private:

  KIIRFourthOrder fIrrFour;
  KLinearRemoval fLineRemove;
  TGraph fData;
  TF1 *fPositiveFit;
  TF1 *fNegativeFit;
  KPulsePolarityCalculator fPolCalc;
  
};



#endif // __KBBV2TIMEDOMAINFITKAMPER_H_