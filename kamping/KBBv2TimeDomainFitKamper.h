//_____________________________________________
// KBBv2TimeDomainFitKamper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBBV2TIMEDOMAINFITKAMPER_H_
#define __KBBV2TIMEDOMAINFITKAMPER_H_

#include "KAmper.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KPtaProcessor.h"
#include "TGraph.h"
#include "KPulsePolarityCalculator.h"

class TF1;

class KBBv2TimeDomainFitKamper : public KAmper{

public:
  KBBv2TimeDomainFitKamper(void);
  virtual ~KBBv2TimeDomainFitKamper(void);
  
  virtual std::map<std::string, KResult>&  MakeKamp(KRawBoloPulseRecord * rawPulseRecord);
  virtual std::map<std::string, KResult>&  MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord);
   
  KPtaProcessor* GetPreProcessor(void) {return fPulsePreProcessor;}
  void SetPreProcessor(KPtaProcessor *pta) { fPulsePreProcessor = pta;}

  TGraph& GetGraph(void) {return fData;}
  TF1* GetFitFunction(void){return fPulseFit;}
  
private:

  KPtaProcessor* fPulsePreProcessor; //you're probably going to pass in a KPulseAnalysis chain here..

  TGraph fData; //holds the data for the fit - using ROOT fittting
  TF1 *fPulseFit;  
  KPulsePolarityCalculator fPolCalc;
  

};



#endif // __KBBV2TIMEDOMAINFITKAMPER_H_
