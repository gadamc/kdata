//_____________________________________________
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
  
  virtual std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord);
  virtual std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord, double fixPeakPosition);


  KOptimalFilter& GetOptimalFilter(void){return fOptimalFilter;}
  void SetWindow(KWindow *pta){fWindow = pta;}
  void SetPreProcessor(KPtaProcessor *pta){fPreProcessor = pta;}

  void SetPulseAmplitudeShift(int a){fPulseAmplitudeShift = a;}
  void SetPulseTemplateShiftFromPreTrigger(double aVal){fPulseTemplateShift = -1*aVal;}
  void SetAmplitudeEstimatorSearchRangeMin(double aVal){fAmplitudeEstimatorSearchRangeMin = aVal;}
  void SetAmplitudeEstimatorSearchRangeMax(double aVal){fAmplitudeEstimatorSearchRangeMax = aVal;}
  double GetPulseTemplateShiftFromPreTrigger(void) const {return fPulseTemplateShift;}
  double GetAmplitudeEstimatorSearchRangeMin(void) const {return fAmplitudeEstimatorSearchRangeMin;}
  double GetAmplitudeEstimatorSearchRangeMax(void) const {return fAmplitudeEstimatorSearchRangeMax;}
  int GetPulseAmplitudeShift(void){return fPulseAmplitudeShift;}
  void SetIonPulseStartTime(double peakPos){fIonPulsePeakPos = peakPos;}

private:
 
  KOptimalFilter fOptimalFilter;
  KPtaProcessor *fPreProcessor; //this should probably be baseline removal will always happen before windowing. Its a KPtaProcessor in case you want to pass in a KLinearRemoval instead.
  KWindow *fWindow; //set the window pointer - this lets you choose different windows via KWindowDesign

  double fPulseTemplateShift;
  double fAmplitudeEstimatorSearchRangeMin;  //minimum value of pulse amplitude search range, relative to the pulse shift. that is, if you set -10, then the fPulseTeplateShift-10 is the minimum value of the range searched
  double fAmplitudeEstimatorSearchRangeMax;  //maximum value of pulse amplitude search range, relative to the pulse shift. that is, if you set 100, then the fPulseTeplateShift+100 is the maximum value of the range searched
  int fPulseAmplitudeShift;
  double fIonPulsePeakPos;
  
  KRealToHalfComplexDFT fR2Hc;
  
};


#endif // __KOPTIMALKAMPER_H_
