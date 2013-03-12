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
#include "KOrderThresholdFinder.h"
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
  void NeedScout(Bool_t aVal){fScoutData = aVal;}
  virtual void ReportResults(void);
  virtual void WriteExtraData(TDirectory *dd);

  
  //these parameters should be available from the database 
  Bool_t SetTemplate(const char* channelName,  std::vector<double>& pulse, int pulseShift, unsigned int pulseType);
  std::vector<double> GetTemplateSpectrum(const char* channelName) const;
  
  
  unsigned int GetNumNoiseEventsFound(const char* channelName) const;
  std::vector<double> GetNoisePower(const char* channelName) const;

  void SetNoisePower(const char* channelName, std::vector<double> power);
  void SetNoiseEventCounts(const char* channelName, unsigned int n){fNoiseEventCounts[channelName] = n;}

  void SetTemplateShift(const char* channelName, int value){fTemplateShift[channelName] = value;}
  int GetTemplateShift(const char* channelName) const;

  void SetEraPeakFinderOrder(const char* channelName, UInt_t n){fEraPeakFinderOrder[channelName] = n;}
  void SetEraPeakFinderOrderIonDefault(UInt_t n){fEraPeakFinderOrderIon_default = n;}
  void SetEraPeakFinderOrderHeatDefault(UInt_t n){fEraPeakFinderOrderHeat_default = n;}

  void SetEraPeakFinderNumRms(const char* channelName, Double_t n){fEraPeakFinderNumRms[channelName] = n;}
  void SetEraPeakFinderNumRmsIonDefault(Double_t n){fEraPeakFinderNumRmsIon_default = n;}
  void SetEraPeakFinderNumRmsHeatDefault(Double_t n){fEraPeakFinderNumRmsHeat_default = n;}

  Int_t GetEraPeakFinderOrder(const char* channelName) const;
  Double_t GetEraPeakFinderNumRms(const char* channelName) const;
  std::map<std::string, std::vector<Int_t> >& GetNoiseEventSambaEventNumberList() {return fNoiseEventSambaEventNumberList;}

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

  KOptimalKamper* GetOptimalKamper(const char* channelName);
  KOptimalKamper* GetOptimalKamper(KRawBoloPulseRecord *rawBoloPulse);
  KRealToHalfComplexDFT& GetRealToHalfComplexDFT(void){return fR2Hc;}
  KHalfComplexPower& GetHalfComplexPower(void){return fHc2P;}
  KEraPeakFinder& GetHeatPeakDetector(void){return fHeatPeakDetector;}
  KEraPeakFinder& GetIonPeakDetector(void){return fIonPeakDetector;}
  KPulseShifter& GetPulseTemplateShifter(void){return fPulseTemplateShifter;}
  KOrderThresholdFinder& GetOrderThresholdFinder(void){return fOrderThresholdFinder;}
  UInt_t GetOperaGlitchNumBinsThreshold(void){return fOperaGlitchNumBinsThreshold;}
  void SetOperaGlitchNumBinsThreshold(UInt_t val){fOperaGlitchNumBinsThreshold = val;}

  Int_t GetHeatPulsePeakSearchRangeMin(void){return fHeatPulsePeakSearchRangeMin;}
  void SetHeatPulsePeakSearchRangeMin(Int_t val){fHeatPulsePeakSearchRangeMin = val;}
  Int_t GetHeatPulsePeakSearchRangeMax(void){return fHeatPulsePeakSearchRangeMax;}
  void SetHeatPulsePeakSearchRangeMax(Int_t val){fHeatPulsePeakSearchRangeMax = val;}

  Int_t GetIonPulsePeakSearchRangeMin(void){return fIonPulsePeakSearchRangeMin;}
  void SetIonPulsePeakSearchRangeMin(Int_t val){fIonPulsePeakSearchRangeMin = val;}
  Int_t GetIonPulsePeakSearchRangeMax(void){return fIonPulsePeakSearchRangeMax;}
  void SetIonPulsePeakSearchRangeMax(Int_t val){fIonPulsePeakSearchRangeMax = val;}

  void CreateHeatWindow(unsigned int pulseSize, double tukeyWindowParam = 0.3);
  void CreateIonWindow(unsigned int pulseSize, double tukeyWindowParam = 0.25);
  std::set<int>& GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  std::set<int>& GetHeatPulseStampWidths(const char* channelName); 
  unsigned int GetHeatPulseStampWidthsSize(const char* channelName) const;

private:

  void FillResults(KPulseAnalysisRecord* rec, std::map<std::string, KResult> &resMap, KRawBoloPulseRecord *pRaw);

  
  KEraPeakFinder fHeatPeakDetector;  //ERA Peak Finder
  KEraPeakFinder fIonPeakDetector;  //ERA Peak Finder
  KOrderThresholdFinder fOrderThresholdFinder; //used to detect "Opera glitches" in the raw pulses. 
  UInt_t fOperaGlitchNumBinsThreshold; 

  Bool_t fScoutData;

  KPulseAnalysisChain* fHeatPreProcessor;  //by default is just a baseline removal
  KPulseAnalysisChain* fBBv1IonPreProcessor;  //by default is a pulse analysis chain with baseline removal + 2x pattern removal. 
  KPulseAnalysisChain* fBBv2IonPreProcessor;  //by default is a pulse analysis chain with linear removal + pattern removal. 
  KWindow* fHeatWindow;
  KWindow* fIonWindow;
  
  KRealToHalfComplexDFT fR2Hc;
  KHalfComplexPower fHc2P;
  KPulseShifter fPulseTemplateShifter;
  

  //todo - need to change all types to ROOT Types...
  //
  std::map<std::string, int> fTemplateShift;
  std::map<std::string, unsigned int> fNoiseEventCounts;  //this is kept in a separate field for the case where a user wants to provide the noise spectra explicitly, but still wants this class to know how many noise events were used to produce the spectra
  std::map<std::string, std::vector<double> > fNoiseSpectra;
  std::map<std::string, std::vector<double> > fTemplateSpectra;
  std::map<std::string, std::set<int> > fHeatPulseStampWidths;
  
  std::map<std::string, std::vector<Int_t> > fNoiseEventSambaEventNumberList;

  std::map<std::string, UInt_t> fEraPeakFinderOrder;
  std::map<std::string, Double_t> fEraPeakFinderNumRms;
  UInt_t fEraPeakFinderOrderIon_default;
  Double_t fEraPeakFinderNumRmsIon_default;
  UInt_t fEraPeakFinderOrderHeat_default;
  Double_t fEraPeakFinderNumRmsHeat_default;

  Int_t fHeatPulsePeakSearchRangeMin;  //relative to the trigger position of the pulse. Can be negative. Default = -25
  Int_t fHeatPulsePeakSearchRangeMax; //relative to the trigger position of the pulse. Default = 100
  Int_t fIonPulsePeakSearchRangeMin;  //relative to the trigger position of the pulse. Can be negative. Default = -1000
  Int_t fIonPulsePeakSearchRangeMax; //relative to the trigger position of the pulse. Default = 2000


  std::map<std::string, KOptimalKamper* > fOptKampers;
  // Bool_t fFirstPulse; 
  
};


#endif // __KCHAMONIXKAMPSITE_H_
