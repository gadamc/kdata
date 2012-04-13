//
// KFilterChainBestCorrelationKamper.h
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KFILTERCHAINBESTCORRELATIONKAMPER_H_
#define __KFILTERCHAINBESTCORRELATIONKAMPER_H_

#include "KAmper.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPulseAnalysisChain.h"
#include "KIIRFilter.h"
#include "KCorrelation.h"
#include <string>
#include <vector>
#include <map>

class KFilterChainBestCorrelationKamper : KAmper {

public:
  KFilterChainBestCorrelationKamper(void);
  virtual ~KFilterChainBestCorrelationKamper(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
  virtual void SetHeatPreprocessor(KPulseAnalysisChain* p){fHeatPreprocessor = p;}
  virtual void SetIonPreprocessor(KPulseAnalysisChain* p){fIonPreprocessor = p;}
  
  virtual KPulseAnalysisChain* GetHeatPreprocessor(void){return fHeatPreprocessor;}
  virtual KPulseAnalysisChain* GetBaselineRemovalIon(void){return fIonPreprocessor;}

  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  virtual Bool_t SetTemplate(const char* channelName, std::vector<double> templ, double starttime);//{ fTemplate[channelName] = templ; fAmpEstimatorTimeInTemplate[channelName] = starttime;}
  virtual void AddProcessor(const char* channelName, KPtaProcessor *p){fPtaProcessorChain[channelName].AddProcessor(p);}
  virtual void AddIIRFilter(const char* channelName, double* a, unsigned int asize, double* b, unsigned int bsize);
  virtual KPulseAnalysisChain* GetPulseAnalysisChain(const char* channelName){return &fPtaProcessorChain[channelName];}
  
  
  
  std::vector<int>& GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  
private:
  std::string fName;
  std::vector<int> fHeatPulseStampWidths;
  
  KPulseAnalysisChain* fHeatPreprocessor;
  KPulseAnalysisChain* fIonPreprocessor;
  KCorrelation fCorrelation;

  
  std::map<std::string, std::vector<double> > fTemplate;
  std::map<std::string, double> fAmpEstimatorTimeInTemplate;
  std::map<std::string, KPulseAnalysisChain> fPtaProcessorChain;
  
  virtual unsigned int GetPositionOfMaxAbsValue(double* input, unsigned int size);
  
  
};


#endif // __KFILTERCHAINBESTCORRELATIONKAMPER_H_
