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
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
  virtual KTrapezoidalFilter* GetTrapHeatTime(Int_t i){return fTrapHeatTime[i];}
  virtual unsigned int GetNumTrapHeatTime(void){return fTrapHeatTime.size();}
  virtual KTrapezoidalFilter* GetTrapIonTime(Int_t i){return fTrapIonTime[i];}
  virtual unsigned int GetNumTrapIonTime(void){return fTrapIonTime.size();}
  
  virtual KTrapezoidalFilter* AddTrapHeatTime(double decay, unsigned int rise, unsigned int flat); 
  virtual KTrapezoidalFilter* AddTrapIonTime(double decay, unsigned int rise, unsigned int flat); 
  
  virtual KTrapezoidalFilter* GetTrapHeatAmplitude(void){return &fTrapHeatAmplitude;}
  virtual KTrapezoidalFilter* GetTrapIonAmplitude(void){return &fTrapIonAmplitude;}
  
  
private:
  std::string fName;
  std::vector<double> fPeakPositionResult;
  
  std::vector<KTrapezoidalFilter* > fTrapIonTime;
  std::vector<KTrapezoidalFilter* > fTrapHeatTime;

  KBaselineRemoval fBaseRemovalHeat;
  KBaselineRemoval fBaseRemovalIon;
  KPatternRemoval fPatRemoval;
  KRootMeanSquare fRms;
  
  //KTrapezoidalFilter fTrapHeatTime;
  //KTrapezoidalFilter fTrapIonTime;
  KTrapezoidalFilter fTrapHeatAmplitude;
  KTrapezoidalFilter fTrapIonAmplitude;

  KOrderFilter fOrderFilter1Heat;
  KOrderFilter fOrderFilter2Heat;
  KOrderFilter fOrderFilter1Ion;
  KOrderFilter fOrderFilter2Ion;
  
  
  void FillPeakPositionResult(KOrderFilter& fOrderFilter, KTrapezoidalFilter* trap, int polarity);
    
  unsigned int FindMaxPeak(vector<double>& pulse, unsigned int maxPosition);
    
  double GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity);
  
  double RunPulseStartTime(vector<KTrapezoidalFilter *>& trapVec, KOrderFilter1& ord1, KOrderFilter1& ord2, 
    KPtaProcessor& fromProcessor, int polarity);
  double RunIonPulseStartTime(int polarity);
  double RunHeatPulseStartTime(void);
  
  
  
  
};


#endif // __KTRAPKAMPERPROTO_H_
