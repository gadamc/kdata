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
  virtual std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord){return MakeKamp(rawPulseRecord, -1.0);}
  virtual std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord, double fixPeakPosition);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
  virtual void SetPreprocessor(KPtaProcessor* p){fPreProcessor = p;}
  virtual void SetPostProcessor(KPtaProcessor* p){fPostProcessor = p;}
  virtual void SetPeakPositionSearchRange(unsigned int min, unsigned int max){fPosRangeMin = min; fPosRangeMax = max;}
  virtual void SetBaselinePosition(unsigned int pos){fBaselinePosition = pos;}
  
  virtual KPtaProcessor* GetPreprocessor(void){return fPreProcessor;}

  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  virtual void SetTemplate(std::vector<double>& templ, double AmpEstimatorTimeInTemplate, double PulseStartTimeInTemplate, double maxValue);

  virtual KPtaProcessor* GetPostProcessor(){return fPostProcessor;}

  virtual unsigned int GetPositionOfMaxAbsValue(double* input, unsigned int size, unsigned int from = 0, unsigned int to = 0);
	virtual unsigned int GetBaselinePosition(){return fBaselinePosition;}
  
  virtual Double_t TemplateFitFunction(Double_t *x, Double_t *par);
  
    
private:
  std::string fName;
  
  KPtaProcessor* fPreProcessor;
  KPtaProcessor* fPostProcessor;

  KCorrelation fCorrelation;

  double fAmpEstimatorTimeInTemplate;
  double fPulseStartTimeInTemplate;
  double fMaxAbsValueInTemplate;
  std::vector<double> fTemplate;
  //double fPosOfTemplateWidth; 
  unsigned int fPosRangeMin;
  unsigned int fPosRangeMax;
	unsigned int fBaselinePosition;
  
  double fPeakPos;
  double fAmpEstPos;  
  
  
  
};


#endif // __KFILTERCHAINBESTCORRELATIONKAMPER_H_
