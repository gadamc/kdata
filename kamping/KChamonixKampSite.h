//
// KChamonixKAmpSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KCHAMONIXKAMPSITE_H_
#define __KCHAMONIXKAMPSITE_H_

#include "KAmpSite.h"
#include "KOptimalKamper.h"
#include "KPeakDetectorProto.h"
#include "KRealToHalfComplexDFT.h"
#include "KHalfComplexPower.h"
#include <map>
#include <string>
#include <vector>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KChamonixKAmpSite : public KAmpSite {

public:
  KChamonixKAmpSite(void);
  virtual ~KChamonixKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return true;}
  
  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  void SetTemplate(const char* channelName, const std::vector<double>& pulse);
  std::vector<double> GetTemplatePower(const char* channelName) const;
  
  void SetTrapDecayConstant(const char* channelName, double value){fDecayValues[channelName] = value;}
  double GetTrapDecayConstant(const char* channelName) const;
  
private:
  
  KPeakDetectorProto fPeakDetector;
  KRealToHalfComplextDFT fR2Hc;
  KHalfComplexPower fHc2P;
  
  KPeakDetectorProto fIonPeakDetector;
  KPeakDetectorProto fMultipleHeatPeakDetector;
  
  std::map<std::string, unsigned int> fNoiseEventCounts;
  std::map<std::string, std::vector<double>> fNoiseSpectra;
  std::map<std::string, std::vector<double>> fTemplateSpectra;
  std::map<std::string, double> fDecayValues;
  
  KOptimalKamper fOptKamper;
  
};


#endif // __KCHAMONIXKAMPSITE_H_
