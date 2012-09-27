//_____________________________________________
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
#include "KLinearRemoval.h"
#include "KPatternRemoval.h"
#include "KIIRFilter.h"
#include "KIIRFirstOrder.h"
#include "KIIRSecondOrder.h"
#include "KIIRThirdOrder.h"
#include "KIIRFourthOrder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KRawBolometerRecord.h"
#include "KCorrelation.h"
#include "KPulseAnalysisChain.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
#include "TVirtualFitter.h"
#include "KResult.h"

#include <stdexcept>

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "TH1D.h"
#include "TF1.h" 



class KFilterChainBestCorrelationKamper : KAmper {

public:
  KFilterChainBestCorrelationKamper(void);
  virtual ~KFilterChainBestCorrelationKamper(void);
  std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord){return MakeKamp(rawPulseRecord, -1.0);}
  std::map<std::string, KResult>  MakeKamp(KRawBoloPulseRecord * rawPulseRecord, double fixPeakPosition);
  template <class T> std::map<std::string, KResult>  MakeKamp(const std::vector<T> &aPulse, double fixPeakPosition = -1.0, const char* channelName = "");


  void SetName(const char* name){fName = name;}
  const char* GetName(void){return fName.c_str();}
  
  void SetPreprocessor(KPtaProcessor* p){fPreProcessor = p;}
  void SetPostProcessor(KPtaProcessor* p){fPostProcessor = p;}
  void SetPeakPositionSearchRange(unsigned int min, unsigned int max){fPosRangeMin = min; fPosRangeMax = max;}
  void SetBaselinePosition(unsigned int pos){fBaselinePosition = pos;}
  
  KPtaProcessor* GetPreprocessor(void){return fPreProcessor;}
  KCorrelation& GetCorrelation(void){return fCorrelation;}
  
  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  void SetTemplate(std::vector<double>& templ, double AmpEstimatorTimeInTemplate, double PulseStartTimeInTemplate, double maxValue);

  KPtaProcessor* GetPostProcessor(){return fPostProcessor;}

  unsigned int GetPositionOfMaxAbsValue(double* input, unsigned int size, unsigned int from = 0, unsigned int to = 0);
	unsigned int GetBaselinePosition(){return fBaselinePosition;}
  
  Double_t TemplateFitFunction(Double_t *x, Double_t *par);
  
  TF1* GetFitFunction(void){return fFitFunc;}
  TH1D* GetFitHistogram(void){return fFitHist;}
  std::vector<double>& GetTemplate(void){return fTemplate;}
  
  
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
 
  TF1 *fFitFunc;
  TH1D *fFitHist;
  TF1 *fFitBase;
  

  

};


#if !defined(__CINT__)  //we have to hide the template function implementation from the CINT/rootcint in order to build the dictionary
template <class T> std::map<std::string, KResult>  KFilterChainBestCorrelationKamper::MakeKamp(const std::vector<T> &aPulse, double fixPeakPosition, const char* channelName)
{
  fPreProcessor->SetInputPulse(aPulse); 

  std::map<string, KResult> myResults;
  if(fPreProcessor->GetInputPulseSize() == 0){
    
    myResults["peakPosition"] = KResult("peakPosition", -1, "bin");
    //rec->SetPeakPosition(-1);
    myResults["amp"] = KResult("amp", -99999, "ADU");
    //rec->SetAmp(-99999);
    myResults["ndfFloat"] = KResult("ndfFloat", -99999);
    //rec->SetExtra(-99999,0);
    return myResults;
  }
  
  if(!fPreProcessor->RunProcess())
  {std::cerr << "KFilterChainBestCorrelationKamper: fPreProcessor failed" << std::endl; return myResults;}
  
  fPostProcessor->SetInputPulse(fPreProcessor);
  
  if(!fPostProcessor->RunProcess())
  {std::cerr << "KFilterChainBestCorrelationKamper: fPostProcessor failed" << std::endl; return myResults;}
  
  fCorrelation.SetInputPulse(fPostProcessor);
  
  fCorrelation.SetResponse(fTemplate);
  if( !fCorrelation.RunProcess() ){
    std::cerr << "KFilterChainBestCorrelationKamper: fCorrelation failed" <<std::endl; return myResults;
  }
  
  //std::cerr << "Range:"<<fPosRangeMin<<","<<fPosRangeMax<<std::endl;
  fPeakPos = (double) GetPositionOfMaxAbsValue(fCorrelation.GetOutputPulse(),fCorrelation.GetOutputPulseSize(),fPosRangeMin,fPosRangeMax)+fPulseStartTimeInTemplate;
  
  
  fAmpEstPos = fPeakPos+(fAmpEstimatorTimeInTemplate-fPulseStartTimeInTemplate);
  
  //Baseline RMS of the first 40% of the processed pulse
  double Baseline_RMS = 0.0;
  unsigned int sample = 0;
  for(;sample < 0.4*fPostProcessor->GetOutputPulseSize();sample++)
    Baseline_RMS += fPostProcessor->GetOutputPulse()[sample]*fPostProcessor->GetOutputPulse()[sample];
  Baseline_RMS /= (double) sample;
  Baseline_RMS = sqrt(Baseline_RMS);
  //rec->SetExtra(Baseline_RMS,2);
  myResults["baselineRmsPostProc"] = KResult("baselineRmsPostProc", Baseline_RMS, "ADU");
  //--------------------
  
  //RMS of the preprocessed pulse
  if(fPreProcessor){
    double Preprocessed_RMS = 0.0;
    sample = 0;
    for(;sample < fPreProcessor->GetOutputPulseSize();sample++)
      Preprocessed_RMS += fPreProcessor->GetOutputPulse()[sample]*fPreProcessor->GetOutputPulse()[sample];
    Preprocessed_RMS /= (double) sample;
    Preprocessed_RMS = sqrt(Preprocessed_RMS);
    //rec->SetExtra(Preprocessed_RMS,3);
    myResults["rmsPreProc"] = KResult("rmsPreProc", Preprocessed_RMS, "ADU");
  }
  //-----------------------
  
  //RMS of the processed pulse
  double Processed_RMS = 0.0;
  sample = 0;
  for(;sample < fPostProcessor->GetOutputPulseSize();sample++)
    Processed_RMS += fPostProcessor->GetOutputPulse()[sample]*fPostProcessor->GetOutputPulse()[sample];
  Processed_RMS /= (double) sample;
  Processed_RMS = sqrt(Processed_RMS);
  //rec->SetExtra(Processed_RMS,4);
  myResults["rmsPostProc"] = KResult("rmsPostProc", Processed_RMS, "ADU");
  //-----------------------------
  
  
  //Fitting
  //TVirtualFitter::SetDefaultFitter("Fumili");
  // Amplitude estimation
  if(fFitFunc) delete fFitFunc;
  if(fFitHist) delete fFitHist;
  if(fFitBase) delete fFitBase;
  
  fFitFunc = new TF1("fFitFunc",this, &KFilterChainBestCorrelationKamper::TemplateFitFunction,(double) (fPeakPos - fPulseStartTimeInTemplate - 5),(double) (fPeakPos - fPulseStartTimeInTemplate + fTemplate.size() - 5),2,"KFilterChainBestCorrelationKamper","TemplateFitFunction");
  fFitFunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fAmpEstPos)/fMaxAbsValueInTemplate,fPeakPos - fPulseStartTimeInTemplate);
  fFitFunc->SetParLimits(1,fPeakPos - fPulseStartTimeInTemplate - 5,fPeakPos - fPulseStartTimeInTemplate + 5);
  
  //fFitFunc->FixParameter(1,PeakPos - fPulseStartTimeInTemplate);
  //fFitFunc->SetParLimits(0,-1000,0);
  fFitHist = new TH1D("fFitHist",channelName,fPostProcessor->GetOutputPulseSize(),0,fPostProcessor->GetOutputPulseSize()-1);
  for(unsigned int i = 0; i < fPostProcessor->GetOutputPulseSize();i++){
    fFitHist->SetBinContent(i+1,fPostProcessor->GetOutputPulse()[i]);
    fFitHist->SetBinError(i+1,Baseline_RMS);
  }
  Int_t fitres = fFitHist->Fit("fFitFunc","RQNS");
  
  myResults["amp"] = KResult("amp", fFitFunc->GetParameter(0), "ADU");
  myResults["peakPosition"] = KResult("peakPosition", (double)fFitFunc->GetParameter(1) + fPulseStartTimeInTemplate, "bin");
  myResults["chiSq"] = KResult("chiSq", fFitFunc->GetChisquare());
  myResults["ndfFloat"] = KResult("ndfFloat", fFitFunc->GetNDF());
  myResults["fitResult"] = KResult("fitResult", fitres);
  
  // rec->SetAmp(fFitFunc->GetParameter(0));
  // rec->SetPeakPosition((double)fFitFunc->GetParameter(1)+fPulseStartTimeInTemplate);
  // rec->SetChiSq(fFitFunc->GetChisquare());
  // rec->SetExtra(fFitFunc->GetNDF(),0);
  // rec->SetExtra(fitres,1);
  //if(fitres != 0)
  //std::cout << "fit is not valid "<<fitres << std::endl;
  
  // Baseline value estimation
  fFitBase = new TF1("fFitBase",this, &KFilterChainBestCorrelationKamper::TemplateFitFunction,(double) (fBaselinePosition - 5),(double) (fBaselinePosition + fTemplate.size() - 5),2,"KFilterChainBestCorrelationKamper","TemplateFitFunction");
  fFitBase->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fBaselinePosition)/fMaxAbsValueInTemplate,fBaselinePosition);
  fFitBase->FixParameter(1,fBaselinePosition);
  fitres = fFitHist->Fit("fFitBase","RQNS");
  
  myResults["ampFixHeatPosition"] = KResult("ampFixHeat", fFitFunc->GetParameter(0), "ADU");
  myResults["fixHeatPosition"] = KResult("fixHeatPosition", fBaselinePosition, "bin");
  myResults["fitResults_fixHeatPosition"] = KResult("fitResults_fixHeatPosition", fitres);
  // rec->SetExtra(fFitFunc->GetParameter(0),9);
  //  rec->SetExtra(fBaselinePosition,10);
  //  rec->SetExtra(fitres,11);
  //-----------------
  
  // Heat pulse time fixed by ionisation
  if(fixPeakPosition > 0){
    
    fFitFunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fixPeakPosition)/fMaxAbsValueInTemplate,fixPeakPosition);
    fFitFunc->FixParameter(1,fixPeakPosition);
    fitres = fFitHist->Fit("fFitFunc","RQNS");
    myResults["ampFixedPositionFromIon"] = KResult("ampFixedPositionFromIon", fFitFunc->GetParameter(0), "ADU");
    myResults["fixedPositionFromIon"] = KResult("fixedPositionFromIon", fixPeakPosition, "bin");
    myResults["fitResults_fixedPositionFromIon"] = KResult("fitResults_fixedPositionFromIon", fitres);
    
    //rec->SetExtra(fFitFunc->GetParameter(0),12);
    //rec->SetExtra(fixPeakPosition,13);
    //rec->SetExtra(fitres,14);
  }
  else myResults["fitResults_fixedPositionFromIon"] =  KResult("fitResults_fixedPositionFromIon", -1);
  //rec->SetExtra(-1,14);
  
  for(unsigned int i = 0; i < fPreProcessor->GetNumProcessors(); i++){
    
    try{
      KLinearRemoval& mProc = dynamic_cast<KLinearRemoval &>( *fPreProcessor->GetProcessor(i) );
      myResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetOffset(), "ADU");
      myResults["slopeRemoved"] = KResult("slopeRemoved", mProc.GetSlope(), "ADU/bin");
      //rec->SetBaselineRemoved(mProc.GetOffset());
      //rec->SetSlopeRemoved(mProc.GetSlope());
    }
    catch(std::bad_cast){} //just do nothing.
    
    try{
      KBaselineRemoval& mProc = dynamic_cast<KBaselineRemoval &>( *fPreProcessor->GetProcessor(i) );
      myResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetBaselineOffset(), "ADU");
      //rec->SetBaselineRemoved(mProc.GetBaselineOffset());
    }
    catch(std::bad_cast){} //do nothing.
    
  }
  
  // //calculate the pulse rise time and pulse width.
  // double baselineValue = 0;
  // std::vector<Short_t>& rawPulse = pRec->GetTrace();
  
  // if(!mProc){
    //   //we didn't find a baseline removal type of processor above, so we have to calculate 
    //   //the baseline amplitude by hand. 
    //   double stop = 0.4*rawPulse.size();
    //   unsigned int start = 0;
    //   unsigned int i = start;
    //   for( ; i < stop; i++){
      //     baselineValue += rawPulse[i];
      //   }
      // baselineValue = baselineValue/double(i-start);
      // }
      
      //now calculate the area under the pulse template, given 
      
      
  return myResults;
      
}
#endif


#endif // __KFILTERCHAINBESTCORRELATIONKAMPER_H_
