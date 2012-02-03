//
// KBlackForestKamperProto.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBLACKFORESTKAMPERPROTO_H_
#define __KBLACKFORESTKAMPERPROTO_H_

#include "KAmper.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include "KPeakDetectorProto.h"
#include "KRootMeanSquare.h"
#include <string>
#include <vector>
#include <map>

class KBlackForestKamperProto : KAmper {

public:
  KBlackForestKamperProto(void);
  virtual ~KBlackForestKamperProto(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
  virtual KBaselineRemoval* GetBaselineRemovalHeat(void){return &fBaseRemovalHeat;}
  virtual KBaselineRemoval* GetBaselineRemovalIon(void){return &fBaseRemovalIon;}
  virtual KPatternRemoval* GetPatternRemoval(void){return &fPatRemoval;}

  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  virtual void SetParams(const char* channelName, std::vector<double> value){ fParams[channelName] = value;}
  virtual std::vector<double>  GetParams(const char* channelName) const;
  
  
  std::vector<int>& GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  
private:
  std::string fName;
  std::vector<int> fHeatPulseStampWidths;

  KBaselineRemoval fBaseRemovalHeat;
  KBaselineRemoval fBaseRemovalIon;
  KPatternRemoval fPatRemoval;
  KRootMeanSquare fRms;
  
  KTrapezoidalFilter fTrapAmplitudeIon;
  KTrapezoidalFilter fTrapAmplitudeHeat1;
  KTrapezoidalFilter fTrapAmplitudeHeat2;
  double fHeat1Decay;
  double fHeat2Coeff;
  double fHeat2Decay;
  
  KPeakDetectorProto fIonPeakDetector;
  
  KPeakDetectorProto fMultipleHeatPeakDetector;
  
  std::map<std::string, std::vector<double> > fParams;
  std::vector<double> fDefaultTrapHeatAmplitudeParameters;
  std::vector<double> fDefaultTrapIonAmplitudeParameters;
  
  double GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity);
  void FillTrapAmplitudeParameters(const char* channelName, bool isHeatPulse);
  
};


#endif // __KBLACKFORESTKAMPERPROTO_H_
