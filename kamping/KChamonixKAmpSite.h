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
#include "KBaselineRemoval.h"
#include "KWindow.h"
#include "KEraPeakFinder.h"
#include "KPulseShifter.h"
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
  virtual Bool_t NeedScout(void){ return fScoutData;}
  
  void SetNeedScout(Bool_t aVal){fScoutData = aVal;}
  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database
  void SetEraOrder(Int_t aVal){fHeatPeakDetector.SetOrder(aVal);}
  void SetEraNumRms(Float_t aVal){fHeatPeakDetector.SetNumRms(aVal);}
  void SetEraPolarity(Int_t aVal){fHeatPeakDetector.SetPolarity(aVal);}
  
  Bool_t SetTemplate(const char* channelName,  std::vector<double>& pulse);
  std::vector<double> GetTemplateSpectrum(const char* channelName) const;
  
  void SetTrapDecayConstant(const char* channelName, double value){fDecayValues[channelName] = value;}
  double GetTrapDecayConstant(const char* channelName) const;
  
  unsigned int GetNumNoiseEventsFound(const char* channelName) const;
  std::vector<double> GetNoisePower(const char* channelName) const;

  void SetNoisePower(const char* channelName, std::vector<double> power);
  void SetNoiseEventCounts(const char* channelName, unsigned int n){fNoiseEventCounts[channelName] = n;}

  KBaselineRemoval& GetBaselineRemovalHeat(void){return fBaselineRemovalHeat;}
  KWindow& GetHeatWindow(void){return fHeatWindow;}
  
  KOptimalKamper& GetOptimalKamper(void){return fOptKamper;}
  KRealToHalfComplexDFT& GetRealToHalfComplexDFT(void){return fR2Hc;}
  KHalfComplexPower& GetHalfComplexPower(void){return fHc2P;}
  KEraPeakFinder& GetHeatPeakDetector(void){return fHeatPeakDetector;}
  KPulseShifter& GetPulseTemplateShifter(void){return fPulseTemplateShifter;}
  
private:
  
  //KPeakDetectorProto fIonPeakDetector;
  //KPeakDetectorProto fHeatPeakDetector;
  KEraPeakFinder fHeatPeakDetector;
  Bool_t fScoutData;

  KBaselineRemoval fBaselineRemovalHeat;
  //KBaselineRemoval fBaselineRemovalIon;
  
  KRealToHalfComplexDFT fR2Hc;
  KHalfComplexPower fHc2P;
  KWindow fHeatWindow;
  KPulseShifter fPulseTemplateShifter;
  
  std::map<std::string, unsigned int> fNoiseEventCounts;
  std::map<std::string, std::vector<double> > fNoiseSpectra;
  std::map<std::string, std::vector<double> > fTemplateSpectra;
  std::map<std::string, double> fDecayValues;
  
  KOptimalKamper fOptKamper;
  
};


#endif // __KCHAMONIXKAMPSITE_H_
