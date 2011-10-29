//
// KTrapezoidalKamper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//


#ifndef __KTRAPEZOIDALKAMPER_H_
#define __KTRAPEZOIDALKAMPER_H_

#include "KAmper.h"
#include "Rtypes.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include "KRootMeanSquare.h"
#include <string>
#include "KPtaProcessor.h"
#include <vector>

class KTrapezoidalKamper : KAmper {

public:
  KTrapezoidalKamper(void);
  virtual ~KTrapezoidalKamper(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * ampBolo, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * ampBolo, KPulseAnalysisRecord *rec);

  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
  //virtual void SetDebugMode(Bool_t mode = true){fDebugMode = mode;}
  //virtual Bool_t IsDebugMode(void){ return fDebugMode;}
  
  //virtual std::vector< std::vector<double> >& GetDebugResults(void){return fDebugResults;}
  //virtual std::vector< std::string >& GetDebugSteps(void){return fDebugSteps;}
  
private:
  std::string fName;
  //Bool_t fDebugMode;
  
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
  
  double* fHeatPulse;
  double* fIonPulse;
  double* fHeatPulse2;
  double* fIonPulse2;
  unsigned int fHeatPulseSize;
  unsigned int fIonPulseSize;
  
  double* fOrderPulseHeat1;
  double* fOrderPulseHeat2;
  double* fOrderPulseIon1;
  double* fOrderPulseIon2;
                              
  double* fPeakPositionResultHeat;
  double* fPeakPositionResultIon;
  unsigned int fPeakPositionResultHeatSize;
  unsigned int fPeakPositionResultIonSize;
    
  //std::vector< std::vector<double> > fDebugResults;
  //std::vector< std::string > fDebugSteps;
  
  void SetHeatPointers();
  void SetIonPointers();
  void CheckMemory(KRawBoloPulseRecord *pRec);
                                               
  void FindPeaks(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, unsigned int flatTopTime,
    double* result, unsigned int resultSize, int polarity);
    
  unsigned int FindMaxPeak(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
    unsigned int flatTopTime, double* result, unsigned int resultSize, double* trapOut, unsigned int trapOutSize, int polarity);
    
  double GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength);
  //void FillKamperDebugResults(KPtaProcessor &mProcessor);
}; 

#endif // __KTRAPEZOIDALKAMPER_H_
