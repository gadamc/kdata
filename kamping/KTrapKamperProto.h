//
// KTrapKamperProto.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KTRAPKAMPERPROTO_H_
#define __KTRAPKAMPERPROTO_H_

#include "KAmper.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include "KRootMeanSquare.h"
#include <string>
#include <vector>

class KTrapKamperProto : KAmper {

public:
  KTrapKamperProto(void);
  virtual ~KTrapKamperProto(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * ampBolo, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * ampBolo, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
private:
  std::string fName;
  std::vector<double> fPeakPositionResult;
  
  KBaselineRemoval fBaseRemovalHeat;
  KBaselineRemoval fBaseRemovalIon;
  KPatternRemoval fPatRemoval;
  KRootMeanSquare fRms;
  
  KTrapezoidalFilter fTrapHeatTime;
  KTrapezoidalFilter fTrapIonTime;
  KTrapezoidalFilter fTrapHeatAmplitude;
  KTrapezoidalFilter fTrapIonAmplitude;

  KOrderFilter fOrderFilter1Heat;
  KOrderFilter fOrderFilter2Heat;
  KOrderFilter fOrderFilter1Ion;
  KOrderFilter fOrderFilter2Ion;
  
  
  void FindPeaks(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
    unsigned int flatTopTime, int polarity);
    
  unsigned int FindMaxPeak(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
    unsigned int flatTopTime, double* trapOut, unsigned int trapOutSize, int polarity);
    
  double GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity);
  
  
};


#endif // __KTRAPKAMPERPROTO_H_
