//
// KFilterChainBestCorrelationKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Extra fields:
// 0 : RMS of the first 40% of the processed pulse
// 1 : Chi^2 of the fit
// 2 : NDF of the fit
// 3 : fit result
// 4 : RMS of preprocessed pulse
// 5 : RMS of processed pulse

#include "KFilterChainBestCorrelationKamper.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KIIRFilter.h"
#include "KIIRFirstOrder.h"
#include "KIIRSecondOrder.h"
#include "KIIRThirdOrder.h"
#include "KIIRFourthOrder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KRawBolometerRecord.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TVirtualFitter.h"

#include <stdexcept>
#include <math.h>


using namespace std;

KFilterChainBestCorrelationKamper::KFilterChainBestCorrelationKamper(void)
{

  SetName("KFilterChainBestCorrelationKamper");
  

  //fHeatPulseStampWidths.reserve(6);
  
  fPreprocessor = 0;
  fProcessorChain = 0;
  fTemplate.resize(0);
  fProcessedPulse.resize(0);
  fPreprocessedPulse.resize(0);
  fCorrelatedPulse.resize(0);
  fPosRangeMin = 0;
  fPosRangeMax = 0;
}

KFilterChainBestCorrelationKamper::~KFilterChainBestCorrelationKamper(void)
{
  
}

Bool_t KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec )
{
  return MakeKamp(rawPulseRecord, rec, -1);
}


Bool_t KFilterChainBestCorrelationKamper::MakeBaseKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{
  Bool_t theRet;
  if(pRec->GetIsHeatPulse())
    theRet = MakeKamp(pRec, rec, 260);
  else
    theRet = MakeKamp(pRec, rec, 4100); 
  rec->SetIsBaseline(true);
  return theRet;
}

Bool_t KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
  //cout << "MakeKamp start"<<endl;
  rec->SetIsBaseline(false); 
  rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0){
    //cerr << "KFilterChainBestCorrelationKamper::MakeKamp. Pulse Length is zero." << endl;
    rec->SetPeakPosition(-1);
    rec->SetAmp(-99999);
    return false;
  }

  
  if(pRec->GetTrace().size() == 0){
      rec->SetPeakPosition(-1);
      rec->SetAmp(-99999);
      return false;
    }
  
  

  if(!fPreprocessor)
  {cout << "fPreprocessor isn't set!" << endl; return false;}
  
  if(!fProcessorChain)
    {cout << "fProcessorChain isn't set!" << endl; return false;}
  
  if(fTemplate.size() == 0)
    {cout << "fTemplate isn't set!" << endl; return false;}

  double PeakPos = -1.0;
  
  // pre-processing
  fPreprocessor->SetInputPulse((std::vector<short> &)pRec->GetTrace());
  
  // debug output
//   cout << "input pulse:"<<endl;
//   for(int i=0; i<fPreprocessor->GetInputPulseSize(); i++)
//     cout<<fPreprocessor->GetInputPulse()[i]<<",";
//   cout<< endl;
  
  if(!fPreprocessor->RunProcess())
    {cout << "fPreprocessor failed" << endl; return false;}
  //debug output
  fPreprocessedPulse.resize(fPreprocessor->GetOutputPulseSize());
  for(unsigned int i=0; i<fPreprocessor->GetOutputPulseSize();i++)
    fPreprocessedPulse[i]=*(fPreprocessor->GetOutputPulse()+i);
  
  // processing
  fProcessorChain->SetInputPulse(fPreprocessor->GetOutputPulse(), fPreprocessor->GetOutputPulseSize());
  if(!fProcessorChain->RunProcess())
    {cout << "fProcessorChain failed" << endl; return false;}
  
  
  fProcessedPulse.resize(fProcessorChain->GetOutputPulseSize());
  for(unsigned int j = 0; j < fProcessorChain->GetOutputPulseSize() ;j++){
    fProcessedPulse[j] = *(fProcessorChain->GetOutputPulse()+j);
  }
  
  //debugging
//  if(pRec->GetIsHeatPulse()){
//    cout << "Channel: "<<pRec->GetChannelName()<<endl;
//    cout << "fPreprocessor output:" << endl;
//    for(unsigned int i = 0; i < fPreprocessor->GetOutputPulseSize(); i++)
//      cout<<*(fPreprocessor->GetOutputPulse()+i)<<",";
//    cout<<endl;
//   cout << "fProcessorChain contains following processors:"<<endl;
//   for(unsigned int i = 0; i < fProcessorChain->GetNumProcessors();i++){
//     cout << fProcessorChain->GetProcessor(i)->GetName() << endl;
//     //cout << ((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefASize()<<" "<<((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefBSize()<<endl;
//     for(unsigned int j = 0; j < ((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefASize();j++)
//       cout<<((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefA()[j]<<" ";
//     cout<<endl;
//     for(unsigned int j = 0; j < ((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefBSize();j++)
//       cout<<((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefB()[j]<<" ";
//     cout<<endl;
//   }
  //
  
//    cout << "fProcessorChain output:" << endl;
//   for(unsigned int i = 0; i < fProcessorChain->GetOutputPulseSize(); i++)
//     cout<<*(fProcessorChain->GetOutputPulse()+i)<<",";
//   cout<<endl;//}
//   

  if(fixPeakPosition == -1){
    fCorrelation.SetInputPulse(fProcessorChain->GetOutputPulse(), fProcessorChain->GetOutputPulseSize());
    
    fCorrelation.SetResponse(fTemplate);
    if( !fCorrelation.RunProcess() ){
        cout << "fCorrelation failed" <<endl; return false;
      }
      
    fCorrelatedPulse.resize(fCorrelation.GetOutputPulseSize());
    for(unsigned int i=0; i<fCorrelation.GetOutputPulseSize();i++)
      fCorrelatedPulse[i]=*(fCorrelation.GetOutputPulse()+i);
    
      //debugging
//  cout << "Correlation output:" << endl;
//  for(unsigned int i = 0; i < fCorrelation.GetOutputPulseSize(); i++)
//    cout<<*(fCorrelation.GetOutputPulse()+i)<<",";
//  cout<<endl;
      //
   
      //cout << "Range:"<<fPosRangeMin<<","<<fPosRangeMax<<endl;
      PeakPos = (double) GetPositionOfMaxAbsValue(fCorrelation.GetOutputPulse(),fCorrelation.GetOutputPulseSize(),fPosRangeMin,fPosRangeMax)+fPulseStartTimeInTemplate;
      
      //cout<< "PeakPos:" << PeakPos << endl;
    }      
    else
      PeakPos = fixPeakPosition;
    fPeakPos = PeakPos;
    fAmpEstPos = PeakPos+(fAmpEstimatorTimeInTemplate-fPulseStartTimeInTemplate);
    
    //Baseline RMS of the first 40% of the processed pulse
    double Baseline_RMS = 0.0;
    unsigned int sample = 0;
    for(;sample < 0.4*fProcessorChain->GetOutputPulseSize();sample++)
      Baseline_RMS += fProcessorChain->GetOutputPulse()[sample]*fProcessorChain->GetOutputPulse()[sample];
    Baseline_RMS /= (double) sample;
    Baseline_RMS = sqrt(Baseline_RMS);
    rec->SetExtra(Baseline_RMS,0);
    //--------------------
    
    //RMS of the preprocessed pulse
    double Preprocessed_RMS = 0.0;
    sample = 0;
    for(;sample < fPreprocessor->GetOutputPulseSize();sample++)
      Preprocessed_RMS += fPreprocessor->GetOutputPulse()[sample]*fPreprocessor->GetOutputPulse()[sample];
    Preprocessed_RMS /= (double) sample;
    Preprocessed_RMS = sqrt(Preprocessed_RMS);
    rec->SetExtra(Preprocessed_RMS,4);
    //-----------------------
    
    //RMS of the processed pulse
    double Processed_RMS = 0.0;
    sample = 0;
    for(;sample < fProcessorChain->GetOutputPulseSize();sample++)
      Processed_RMS += fProcessorChain->GetOutputPulse()[sample]*fProcessorChain->GetOutputPulse()[sample];
    Processed_RMS /= (double) sample;
    Processed_RMS = sqrt(Processed_RMS);
    rec->SetExtra(Processed_RMS,5);
    //-----------------------------
    
    if(!fDoFit){
      if((PeakPos)>0){
        rec->SetAmp(*(fProcessorChain->GetOutputPulse()+(int)fAmpEstPos));
        rec->SetPeakPosition(PeakPos);
        //if (fixPeakPosition != -1)
          //cout<<pRec->GetChannelName()<<" baseline:\t"<<rec->GetAmp()<<endl;
  //      cout << "AmpEstimation for " << pRec->GetChannelName()<<" Amp:"<<rec->GetAmp()<<" Pos:"<<rec->GetPeakPosition()<<endl;
  //      cout << "fAmpEstimatorTimeInTemplate:"<<fAmpEstimatorTimeInTemplate<<" fPulseStartTimeInTemplate:"<<fPulseStartTimeInTemplate<<endl; 
        //cout<<*(fProcessorChain->GetOutputPulse()+(int)(PeakPos+fAmpEstimatorTimeInTemplate))<<endl;
      }
      else{
        rec->SetAmp(-99999);
        rec->SetPeakPosition(-1);
        return false;
      }
    }
    else{
      //Fitting
      //TVirtualFitter::SetDefaultFitter("Fumili");
      TF1 *fitfunc = new TF1("fitfunc",this, &KFilterChainBestCorrelationKamper::TemplateFitFunction,(double) (PeakPos - fPulseStartTimeInTemplate - 5),(double) (PeakPos - fPulseStartTimeInTemplate + fTemplate.size() - 5),2,"KFilterChainBestCorrelationKamper","TemplateFitFunction");
      //TF1 *fitfunc2 = new TF1("fitfunc2",this, &KFilterChainBestCorrelationKamper::TemplateFitFunction,(double) (PeakPos - fPulseStartTimeInTemplate - 20),(double) (PeakPos - fPulseStartTimeInTemplate + fTemplate.size()),2,"KFilterChainBestCorrelationKamper","TemplateFitFunction");
      fitfunc->SetParameters(*(fProcessorChain->GetOutputPulse()+(int)fAmpEstPos)/fMaxAbsValueInTemplate,PeakPos - fPulseStartTimeInTemplate);
      if (fixPeakPosition == -1)
        fitfunc->SetParLimits(1,PeakPos - fPulseStartTimeInTemplate - 5,PeakPos - fPulseStartTimeInTemplate + 5);
      else
        fitfunc->FixParameter(1,PeakPos - fPulseStartTimeInTemplate);
      //fitfunc->SetParLimits(0,-1000,0);
      TH1D *hist = new TH1D("hist",pRec->GetChannelName(),fProcessorChain->GetOutputPulseSize(),0,fProcessorChain->GetOutputPulseSize()-1);
      for(unsigned int i = 0; i < fProcessorChain->GetOutputPulseSize();i++){
        hist->SetBinContent(i+1,fProcessorChain->GetOutputPulse()[i]);
        hist->SetBinError(i+1,Baseline_RMS);
      }
      Int_t fitres = hist->Fit("fitfunc","RQNS");
      //fitfunc2->SetParameters(fitfunc->GetParameters());
      //hist->Fit("fitfunc2","RQ");
      //cout<<pRec->GetChannelName()<<" before fitting:"<<*(fProcessorChain->GetOutputPulse()+(int)fAmpEstPos)<<","<<PeakPos<<" after fitting:"<<fitfunc->GetParameter(0)<<","<<fitfunc->GetParameter(1)+fPulseStartTimeInTemplate<<endl;
      rec->SetAmp(fitfunc->GetParameter(0));
      rec->SetPeakPosition((double)fitfunc->GetParameter(1)+fPulseStartTimeInTemplate);
      rec->SetExtra(fitfunc->GetChisquare(),1);
      rec->SetExtra(fitfunc->GetNDF(),2);
      rec->SetExtra(fitres,3);
      //if(fitres != 0)
        //cout << "fit is not valid "<<fitres << endl;
      
      delete fitfunc;
      delete hist;
    }
    return true;

}


// std::vector<int>& KFilterChainBestCorrelationKamper::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
// {
//   KRawBolometerRecord *bolo = pRec->GetBolometerRecord();
//   unsigned int counter = 0;
//   fHeatPulseStampWidths.clear();
//   
//   for(int i = 0; i < bolo->GetNumPulseRecords(); i++){
//     KRawBoloPulseRecord *p = bolo->GetPulseRecord(i);
//     if(p->GetIsHeatPulse()){
//       counter++;
//       bool addValue = true;
//       //search through the vector to find out if this stamp width is unique
//       for(unsigned int j = 0; j < fHeatPulseStampWidths.size(); j++){
//         if(fHeatPulseStampWidths.at(j) == (int)(p->GetHeatPulseStampWidth()) )
//           addValue = false;
//       }
//       
//       if(addValue){
//         if(fHeatPulseStampWidths.size() < counter)
//           fHeatPulseStampWidths.push_back( (int)(p->GetHeatPulseStampWidth()) );
//         else
//           fHeatPulseStampWidths.at(counter) = (int)(p->GetHeatPulseStampWidth()) ;
//       }
//   
//     }
//   }
//   return fHeatPulseStampWidths;
// }



unsigned int KFilterChainBestCorrelationKamper::GetPositionOfMaxAbsValue(double* input, unsigned int size,unsigned int from, unsigned int to)
{
  unsigned int pos = 0;
  double max = 0.0;
  unsigned int i = (from < size)? from : 0;
  unsigned int stop = ((to > 0)&&(to < size)) ? to : size;
  for(; i < stop; i++)
    if( fabs(*(input+i)) > max ){
      pos = i;
      max = fabs(*(input+i));
    }
  return pos;
}

void KFilterChainBestCorrelationKamper::SetTemplate(std::vector<double> templ, double AmpEstimatorTimeInTemplate, double PulseStartTimeInTemplate)
{
  // the template has to be already preprocessed (no baseline, heat->ion crosstalk etc),
  // which is usually the case if you load a template from CouchDB
  fTemplate = templ;
  fAmpEstimatorTimeInTemplate = AmpEstimatorTimeInTemplate;
  fPulseStartTimeInTemplate = PulseStartTimeInTemplate;
  fMaxAbsValueInTemplate = fabs(templ[GetPositionOfMaxAbsValue(&templ[0],templ.size())]);
  //cout<<"Amp Estimator in Template pos: "<<AmpEstimatorTimeInTemplate<<", amp: "<< fTemplate[AmpEstimatorTimeInTemplate]<<endl;
  //for(unsigned int i=0; i < fTemplate.size(); i++)
  //  cout<<fTemplate[i]<<",";
  //cout<<endl;
  
}
    
//     //debug output
// //     cout<<"Template before processing"<<endl;
// //     for(int i = 0; i< templ.size(); i++)
// //       cout<<templ[i]<<",";
//     
//     fProcessorChain->SetInputPulse(templ);
//     if(!fProcessorChain->RunProcess()){
//       cout << "The template could not be processed!" << endl;
//       return false;
//     }
//     //debugging
//      cout<<"Template input after processing:"<<endl;
//      for(int i = 0; i < fProcessorChain->GetOutputPulseSize();i++)
//        cout<<*(fProcessorChain->GetOutputPulse()+i)<<",";
//      cout<<endl;
//      cout<<"pretrigger:"<<pretrigger<<" delta_t:"<<delta_t<<" width:"<<width<<endl;
//     //
//     std::vector<double> templvector;
//     templvector.resize(width);
//     if(fProcessorChain->GetOutputPulseSize() > (pretrigger - delta_t + width))
//       for(unsigned int i = pretrigger - delta_t; i < (pretrigger - delta_t + width); i++){
//         templvector[i-(pretrigger-delta_t)]=fProcessorChain->GetOutputPulse()[i];
//       }
//     else
//       { cout << "(Pretrigger + template width - delta_t) > size of the input template!" << endl; return false;}
//     
//     fAmpEstimatorTimeInTemplate = (double) GetPositionOfMaxAbsValue(&templvector[0], templvector.size());
//     fPulseStartTimeInTemplate = delta_t;
//     
//     //debugging
// //      cout<< "Processed template set to: "<<endl;
// //      for(unsigned int i = 0; i<templvector.size();i++)
// //        cout<<templvector[i]<<" ";
// //      cout<<endl<<"Amplitude Estimation at "<<fAmpEstimatorTimeInTemplate<<endl;
//     //
//     
//     fTemplate = templvector;
//     return true;
// }

Double_t KFilterChainBestCorrelationKamper::TemplateFitFunction(Double_t *x, Double_t *par)
{
  if (((x[0]-par[1])<1) || ((x[0]-par[1])>(fTemplate.size()-2)))
    return 0.0;
  // Sterling formula (does not work for some reason)
  /* int x0 = (int) floor(x[0] - par[1] + 0.5);
  return par[0]*(fTemplate[x0] + (fTemplate[x0+1]+fTemplate[x0-1])*(x[0]-x0)/2.0+(fTemplate[x0+1]-2*fTemplate[x0]+fTemplate[x0-1])*(x[0]-x0)*(x[0]-x0)/4.0); */
  
  
  Double_t xx = x[0] - par[1];
  bool quadr_interp = false;
  unsigned int x_l = (unsigned int) floor(xx);
  unsigned int x_m = 0;
  if( (fabs(fTemplate[x_l])<fabs(fTemplate[x_l+1]))&&(fabs(fTemplate[x_l+1])>fabs(fTemplate[x_l+2])) ){
    quadr_interp = true;
    x_m = x_l+1;
  }
  if( (fabs(fTemplate[x_l])>fabs(fTemplate[x_l-1]))&&(fabs(fTemplate[x_l])>fabs(fTemplate[x_l+1])) ){
    quadr_interp = true;
    x_m = x_l;
  }
  if(quadr_interp){
    // quadratic interpolation around template maximum
    double x0 = (double) x_m-1;
    double y0 = fTemplate[x0];
    double x1 = (double) x_m;
    double y1 = fTemplate[x1];
    double x2 = (double) x_m+1;
    double y2 = fTemplate[x2];
    
    double a = ((y2-y1)-((y0-y1)*(x1-x2))/(x1-x0))/(x2*x2 - x1*x1 - ((x0*x0-x1*x1)*(x1-x2)/(x2-x1)));
    double b = (y0-y1)/(2*a*(x1-x0))-(x0*x0-x1*x1)/(2*(x1-x0));
    double c = y0 - a*x0*x0 + 2*a*b*x0 - a*b*b;
    return (Double_t) par[0]*(a*(xx-b)*(xx-b)+c);
  }
  else{
    // linear interpolation
    unsigned int fl_x = (unsigned int) floor(xx);
    if (fl_x - xx == 0.0)
      return (Double_t) par[0]*fTemplate[(unsigned int) xx];
    return (Double_t) par[0]*(fTemplate[fl_x]*(1-fabs(fl_x - xx)) + fTemplate[fl_x+1]*(1 - fabs(fl_x + 1 - xx)));
  }
}
