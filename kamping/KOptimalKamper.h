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
#include "KRealToHalfComplexDFT.h"

class KWindow;
class KPtaProcessor;

class KOptimalKamper : KAmper {

public:
  KOptimalKamper(void);
  virtual ~KOptimalKamper(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition);


  KOptimalFilter& GetOptimalFilter(void){return fOptimalFilter;}
  void SetWindow(KWindow *pta){fWindow = pta;}
  void SetBaselineRemoval(KPtaProcessor *pta){fBaseRemove = pta;}

  void SetPulseTemplateShiftFromPreTrigger(double aVal){fPulseTemplateShift = -1*aVal;}
  void SetAmplitudeEstimatorSearchRange(double aVal){fAmplitudeEstimatorSearchRange = aVal;}
  double GetPulseTemplateShiftFromPreTrigger(void) const {return fPulseTemplateShift;}
  double GetAmplitudeEstimatorSearchRange(void) const {return fAmplitudeEstimatorSearchRange;}
  
private:
 
  KOptimalFilter fOptimalFilter;
  KPtaProcessor *fBaseRemove; //baseline removal will always happen before windowing. Its a KPtaProcessor in case you want to pass in a KLinearRemoval instead.
  KWindow *fWindow; //set the window pointer - this lets you choose different windows via KWindowDesign

  double fPulseTemplateShift;
  double fAmplitudeEstimatorSearchRange;
  
  KRealToHalfComplexDFT fR2Hc;
  
};


#endif // __KOPTIMALKAMPER_H_
