//
// KFeldbergKAmpSite.h
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KFELDBERGKAMPSITE_H_
#define __KFELDBERGKAMPSITE_H_

#include "KAmpSite.h"
#include "KFilterChainBestCorrelationKamper.h"
#include <vector>
#include <string>
#include <map>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KFeldbergKAmpSite : public KAmpSite {

public:
  KFeldbergKAmpSite(void);
  virtual ~KFeldbergKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return false;}
  
  KFilterChainBestCorrelationKamper& GetKamper(void){return fFCKamp;}
  virtual Bool_t SetTemplate(const char* channel, std::vector<double> templ, unsigned int pretrigger, unsigned int delta_t, unsigned int width, Bool_t force = false);//{ fTemplate[channel] = templ; fPretrigger[channel] = pretrigger; fDelta_t[channel] = delta_t; fTemplateWidth[channel] = width;}
  virtual void SetPeakPositionSearchRange(const char* channel, unsigned int min, unsigned int max){fPosRangeMin[channel] = min; fPosRangeMax[channel] = max;}
  virtual void AddProcessor(const char* channel,KPtaProcessor *p){fProcessorChain[channel].AddProcessor(p);}
  virtual void AddIIRFilter(const char* channel,double* a, unsigned int asize, double* b, unsigned int bsize); 
  virtual Bool_t SetupFCKamp(KRawBoloPulseRecord* pRaw);
  
  
  virtual void SetNormalizeTemplate(Bool_t value){fNormalizeTemplate = value;}
  virtual Bool_t GetNormalizeTemplate(void){return fNormalizeTemplate;}
  
private:
  std::vector<int> GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  
  KFilterChainBestCorrelationKamper fFCKamp;
  
  std::map<std::string, std::vector<double> > fTemplate;
  std::map<std::string, unsigned int> fPretrigger;
  std::map<std::string, unsigned int> fDelta_t;
  std::map<std::string, unsigned int> fTemplateWidth;
  
  std::map<std::string, double> fAmpEstimatorTimeInTemplate;
  std::map<std::string, double> fPulseStartTimeInTemplate;
  std::map<std::string, unsigned int> fPosRangeMin;
  std::map<std::string, unsigned int> fPosRangeMax;
  
  std::map<std::string, KPulseAnalysisChain> fProcessorChain;
  
  KPulseAnalysisChain fHeatPreprocessor;
  KPulseAnalysisChain fIonPreprocessor;
  std::vector<int> fHeatPulseStampWidths;
  
  Bool_t fNormalizeTemplate;
  
  
};


#endif // __KFELDBERGKAMPSITE_H_
