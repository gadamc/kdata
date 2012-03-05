//
// KFilterChainBestCorrelationKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


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

#include <stdexcept>
#include <math.h>


using namespace std;

KFilterChainBestCorrelationKamper::KFilterChainBestCorrelationKamper(void)
{

  SetName("KFilterChainBestCorrelationKamper");
  

  fHeatPulseStampWidths.reserve(6);
  
  fHeatPreprocessor = 0;
  fIonPreprocessor = 0;
  
  
  
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
  theRet = MakeKamp(pRec, rec, 50);
  rec->SetIsBaseline(true);
  return theRet;
}

Bool_t KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
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



  double PeakPos = -1.0;
  
  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){

    
    fHeatPreprocessor->SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if( !fHeatPreprocessor->RunProcess())
      {cout << "fHeatPreprocessor failed" << endl; return false;}
    
    if( fPtaProcessorChain.find(pRec->GetChannelName()) != fPtaProcessorChain.end()){
      fPtaProcessorChain[pRec->GetChannelName()].SetInputPulse(fHeatPreprocessor->GetOutputPulse(), fHeatPreprocessor->GetOutputPulseSize());
      if( !fPtaProcessorChain[pRec->GetChannelName()].RunProcess() ){
        cout << "fPtaProcessorChain for "<<pRec->GetChannelName()<< " failed" << endl; return false;
      }
      fCorrelation.SetInputPulse(fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulse(),fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulseSize());
    }
    else
      fCorrelation.SetInputPulse(fHeatPreprocessor->GetOutputPulse(),fHeatPreprocessor->GetOutputPulseSize());
    if( fTemplate.find(pRec->GetChannelName()) != fTemplate.end())
      fCorrelation.SetResponse(fTemplate[pRec->GetChannelName()]);
    else{
      cout << "fTemplate for " << pRec->GetChannelName() << " is missing"<< endl; return false;
    }
    
    if(fixPeakPosition == -1){
      if( !fCorrelation.RunProcess() ){
        cout << "fCorrelation failed" <<endl; return false;
      } 
      PeakPos = (double) GetPositionOfMaxAbsValue(fCorrelation.GetOutputPulse(),fCorrelation.GetOutputPulseSize());       
    }      
    else
      PeakPos = fixPeakPosition;
    
   
    if(fPtaProcessorChain.find(pRec->GetChannelName()) != fPtaProcessorChain.end()){
      if((PeakPos+fAmpEstimatorTimeInTemplate[pRec->GetChannelName()])>0)
        rec->SetAmp(*(fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulse()+(int)(PeakPos+fAmpEstimatorTimeInTemplate[pRec->GetChannelName()])));
      else{
        rec->SetAmp(-99999);
        rec->SetPeakPosition(-1);
        return false;
      }
    }
    rec->SetPeakPosition(PeakPos);
        
    return true;
  }

  //do ion pulse analysis
  else{
    
    fIonPreprocessor->SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if( !fIonPreprocessor->RunProcess())
      {cout << "fIonPreprocessor failed" << endl; return false;}
    
    if( fPtaProcessorChain.find(pRec->GetChannelName()) != fPtaProcessorChain.end()){
      fPtaProcessorChain[pRec->GetChannelName()].SetInputPulse(fIonPreprocessor->GetOutputPulse(), fIonPreprocessor->GetOutputPulseSize());
      if( !fPtaProcessorChain[pRec->GetChannelName()].RunProcess() ){
        cout << "fPtaProcessorChain for "<<pRec->GetChannelName()<< " failed" << endl; return false;
      }
      fCorrelation.SetInputPulse(fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulse(),fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulseSize());
    }
    else
      fCorrelation.SetInputPulse(fIonPreprocessor->GetOutputPulse(),fIonPreprocessor->GetOutputPulseSize());
    if( fTemplate.find(pRec->GetChannelName()) != fTemplate.end())
      fCorrelation.SetResponse(fTemplate[pRec->GetChannelName()]);
    else{
      cout << "fTemplate for " << pRec->GetChannelName() << " is missing"<< endl; return false;
    }
    
    if(fixPeakPosition == -1){
      if( !fCorrelation.RunProcess() ){
        cout << "fCorrelation failed" <<endl; return false;
      } 
      PeakPos = (double) GetPositionOfMaxAbsValue(fCorrelation.GetOutputPulse(),fCorrelation.GetOutputPulseSize());       
    }      
    else
      PeakPos = fixPeakPosition;
    
   
    if(fPtaProcessorChain.find(pRec->GetChannelName()) != fPtaProcessorChain.end()){
      if((PeakPos+fAmpEstimatorTimeInTemplate[pRec->GetChannelName()])>0)
        rec->SetAmp(*(fPtaProcessorChain[pRec->GetChannelName()].GetOutputPulse()+(int)(PeakPos+fAmpEstimatorTimeInTemplate[pRec->GetChannelName()])));
      else{
        rec->SetAmp(-99999);
        rec->SetPeakPosition(-1);
        return false;
      }
    }
    rec->SetPeakPosition(PeakPos);
    
    return true;
   }

}


std::vector<int>& KFilterChainBestCorrelationKamper::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
{
  KRawBolometerRecord *bolo = pRec->GetBolometerRecord();
  unsigned int counter = 0;
  fHeatPulseStampWidths.clear();
  
  for(int i = 0; i < bolo->GetNumPulseRecords(); i++){
    KRawBoloPulseRecord *p = bolo->GetPulseRecord(i);
    if(p->GetIsHeatPulse()){
      counter++;
      bool addValue = true;
      //search through the vector to find out if this stamp width is unique
      for(unsigned int j = 0; j < fHeatPulseStampWidths.size(); j++){
        if(fHeatPulseStampWidths.at(j) == (int)(p->GetHeatPulseStampWidth()) )
          addValue = false;
      }
      
      if(addValue){
        if(fHeatPulseStampWidths.size() < counter)
          fHeatPulseStampWidths.push_back( (int)(p->GetHeatPulseStampWidth()) );
        else
          fHeatPulseStampWidths.at(counter) = (int)(p->GetHeatPulseStampWidth()) ;
      }
  
    }
  }
  return fHeatPulseStampWidths;
}

void KFilterChainBestCorrelationKamper::AddIIRFilter(const char* channelName, double* a, unsigned int asize, double* b, unsigned int bsize)
{
  KIIRFilter* iir;
  if((asize == 1) && (bsize == 2))
    iir = (KIIRFilter*) new KIIRFirstOrder(a[0],b[0],b[1]);
  else
    if((asize == 2) && (bsize == 3))
      iir = (KIIRFilter*) new KIIRSecondOrder(a[0],a[1],b[0],b[1],b[2]);
    else
      if((asize == 3) && (bsize == 4))
        iir = (KIIRFilter*) new KIIRThirdOrder(a[0],a[1],a[2],b[0],b[1],b[2],b[3]);
      else
        if((asize == 4) && (bsize == 5))
          iir = (KIIRFilter*) new KIIRFourthOrder(a[0],a[1],a[2],a[3],b[0],b[1],b[2],b[3],b[4]);
        else{
          iir = new KIIRFilter();
          iir->SetCoefficients(a,asize,b,bsize);
        }
  fPtaProcessorChain[channelName].AddProcessor((KPtaProcessor*) iir); 
        
}

unsigned int KFilterChainBestCorrelationKamper::GetPositionOfMaxAbsValue(double* input, unsigned int size)
{
  unsigned int pos = 0;
  double max = 0.0;
  for(unsigned int i = 0; i < size; i++)
    if( fabs(*(input+i)) > max ){
      pos = i;
      max = fabs(*(input+i));
    }
  return pos;
}

Bool_t KFilterChainBestCorrelationKamper::SetTemplate(const char* channelName, std::vector<double> templ, double starttime)
{
  // the template has to be already preprocessed (no baseline, heat->ion crosstalk etc),
  // which is usually the case if you load a template from CouchDB
  
  if(fPtaProcessorChain.find(channelName) != fPtaProcessorChain.end()){
    fPtaProcessorChain[channelName].SetInputPulse(templ);
    if(!fPtaProcessorChain[channelName].RunProcess()){
      cout << "The template could not be processed!" << endl;
      return false;
    }
    std::vector<double> templvector;
    templvector.resize(fPtaProcessorChain[channelName].GetOutputPulseSize());
    for(int i=0; i < fPtaProcessorChain[channelName].GetOutputPulseSize(); i++){
      templvector[i]=*(fPtaProcessorChain[channelName].GetOutputPulse()+i);
    }
    fTemplate[channelName] = templvector;
    fAmpEstimatorTimeInTemplate[channelName] = starttime;
    return true;
  }
  else{
    cout << "WARNING: The PtaProcessorChain for channel " << channelName << " is not set, template will be set without any processing" << endl;
    fTemplate[channelName] = templ; 
    fAmpEstimatorTimeInTemplate[channelName] = starttime;
    return true;
  }
}