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
  
  virtual void SetPreprocessor(KPulseAnalysisChain* p){fPreprocessor = p;}
  virtual void SetProcessorChain(KPulseAnalysisChain* p){fProcessorChain = p;}
  virtual void SetPeakPositionSearchRange(unsigned int min, unsigned int max){fPosRangeMin = min; fPosRangeMax = max;}
  virtual void SetDoFit(Bool_t value){fDoFit = value;}
  
  
  
  virtual KPulseAnalysisChain* GetPreprocessor(void){return fPreprocessor;}

  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  virtual void SetTemplate(std::vector<double> templ, double AmpEstimatorTimeInTemplate, double PulseStartTimeInTemplate);

  virtual KPulseAnalysisChain* GetProcessorChain(){return fProcessorChain;}
  virtual std::vector<double> GetPreProcessedPulse(){return fPreprocessedPulse;}
  virtual std::vector<double> GetProcessedPulse(){return fProcessedPulse;}
  virtual std::vector<double> GetCorrelatedPulse(){return fCorrelatedPulse;}
  virtual unsigned int GetPositionOfMaxAbsValue(double* input, unsigned int size, unsigned int from = 0, unsigned int to = 0);
  virtual Bool_t GetDoFit(void){return fDoFit;}
  
  virtual Double_t TemplateFitFunction(Double_t *x, Double_t *par);
  
  
  
//  std::vector<int>& GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  
private:
  std::string fName;
//  std::vector<int> fHeatPulseStampWidths;
  
  KPulseAnalysisChain* fPreprocessor;
  KPulseAnalysisChain* fProcessorChain;
  KCorrelation fCorrelation;
  double fAmpEstimatorTimeInTemplate;
  double fPulseStartTimeInTemplate;
  double fMaxAbsValueInTemplate;
  std::vector<double> fTemplate;
  unsigned int fPosRangeMin;
  unsigned int fPosRangeMax;
  
  //debug
  double fPeakPos;
  double fAmpEstPos;
  std::vector<double> fPreprocessedPulse;
  std::vector<double> fProcessedPulse;
  std::vector<double> fCorrelatedPulse;
  
  Bool_t fDoFit;
  
  
  
  
  
  
  
  
};


#endif // __KFILTERCHAINBESTCORRELATIONKAMPER_H_
