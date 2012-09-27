//_____________________________________________
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
#include <set>
#include <vector>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;
class KPulseAnalysisChain;

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
  
  //these parameters should be available from the database 
  Bool_t SetTemplate(const char* channelName,  std::vector<double>& pulse, int pulseShift, unsigned int pulseType);
  std::vector<double> GetTemplateSpectrum(const char* channelName) const;
  
  void SetTrapDecayConstant(const char* channelName, double value){fDecayValues[channelName] = value;}
  double GetTrapDecayConstant(const char* channelName) const;
  
  unsigned int GetNumNoiseEventsFound(const char* channelName) const;
  std::vector<double> GetNoisePower(const char* channelName) const;

  void SetNoisePower(const char* channelName, std::vector<double> power);
  void SetNoiseEventCounts(const char* channelName, unsigned int n){fNoiseEventCounts[channelName] = n;}

  void SetTemplateShift(const char* channelName, int value){fTemplateShift[channelName] = value;}
  int GetTemplateShift(const char* channelName) const;

  KPulseAnalysisChain* GetHeatPreProcessor(void){return fHeatPreProcessor;}
  KPulseAnalysisChain* GetBBv1IonPreProcessor(void){return fBBv1IonPreProcessor;}
  KPulseAnalysisChain* GetBBv2IonPreProcessor(void){return fBBv2IonPreProcessor;}
  // void SetHeatPreProcessor(KPulseAnalysisChain* pta);
  // void SetBBv1IonPreProcessor(KPulseAnalysisChain* pta);
  // void SetBBv2IonPreProcessor(KPulseAnalysisChain* pta);

  KWindow* GetHeatWindow(void){return fHeatWindow;}
  KWindow* GetIonWindow(void){return fIonWindow;}

  // void SetHeatWindow(KWindow* pta);
  // void SetIonWindow(KWindow* pta);

  KOptimalKamper& GetOptimalKamper(void){return fOptKamper;}
  KRealToHalfComplexDFT& GetRealToHalfComplexDFT(void){return fR2Hc;}
  KHalfComplexPower& GetHalfComplexPower(void){return fHc2P;}
  KEraPeakFinder& GetHeatPeakDetector(void){return fHeatPeakDetector;}
  KPulseShifter& GetPulseTemplateShifter(void){return fPulseTemplateShifter;}
  
  void CreateHeatWindow(unsigned int pulseSize, double tukeyWindowParam = 0.5);
  void CreateIonWindow(unsigned int pulseSize, double tukeyWindowParam = 0.1);
  std::set<int> GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);

private:

  void FillResults(KPulseAnalysisRecord* rec, std::map<std::string, KResult> &resMap);

  
  KEraPeakFinder fHeatPeakDetector;  //change this to a Wavelet decomposition based pulse detector in the future... 
  Bool_t fScoutData;

  KPulseAnalysisChain* fHeatPreProcessor;  //by default is just a baseline removal
  KPulseAnalysisChain* fBBv1IonPreProcessor;  //by default is a pulse analysis chain with baseline removal + 2x pattern removal. 
  KPulseAnalysisChain* fBBv2IonPreProcessor;  //by default is a pulse analysis chain with linear removal + pattern removal. 
  KWindow* fHeatWindow;
  KWindow* fIonWindow;
  
  KRealToHalfComplexDFT fR2Hc;
  KHalfComplexPower fHc2P;
  KPulseShifter fPulseTemplateShifter;
  
  std::map<std::string, int> fTemplateShift;
  std::map<std::string, unsigned int> fNoiseEventCounts;
  std::map<std::string, std::vector<double> > fNoiseSpectra;
  std::map<std::string, std::vector<double> > fTemplateSpectra;
  std::map<std::string, std::set<double> > fIonPatternRemovalSize;
  std::map<std::string, double> fDecayValues;
  
  KOptimalKamper fOptKamper;
  // Bool_t fFirstPulse; 
  
};


#endif // __KCHAMONIXKAMPSITE_H_
