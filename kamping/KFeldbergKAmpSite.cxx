//
// KFeldbergKAmpSite.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KFeldbergKAmpSite.h"
#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KIIRFilter.h"
#include "KIIRFirstOrder.h"
#include "KIIRSecondOrder.h"
#include "KIIRThirdOrder.h"
#include "KIIRFourthOrder.h"
#include "TString.h"
#include <iostream>
#include <math.h>


using namespace std;


KFeldbergKAmpSite::KFeldbergKAmpSite(void)
{
  SetName("KFeldbergKAmpSite");
  fHeatPulseStampWidths.reserve(6);
  fHeatPreprocessor.SetIsOwner(true);
  fIonPreprocessor.SetIsOwner(true);
  fHeatPreprocessor.AddProcessor(new KBaselineRemoval());
  //fDoFit = false;
  //cout<<"KFeldbergKAmpSite() done"<<endl;
  
}

KFeldbergKAmpSite::~KFeldbergKAmpSite(void)
{

}

Bool_t KFeldbergKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  //cout<<"RunKampSite start"<<endl;
  double PeakPos = -1;
  double maxPeak = 0.;
  int precNum = -1;
  // create KPulseAnalysisRecords for ionisation pulses and
  // look for the largest pulse in ionisation channels to fix the position of the heat pulse
   for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
     KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
     if(!pRaw->GetIsHeatPulse()){
       // Create KPulseAnalysisRecord and a valid KAmpEvent
       //cout << "channel:"<< pRaw->GetChannelName()<<endl;
       KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
       KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
       fFCKamp.SetName("FeldbergKAmper-Ion");
       SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
       if(!SetupFCKamp(pRaw))
         {cout<<"SetupFCKamp for "<< pRaw->GetChannelName() << " failed"<<endl;return false;}
       fFCKamp.MakeKamp(pRaw, rec);
       // create  KPulseAnalysisRecord for baseline amplitude estimation
       KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
       fFCKamp.SetName("FeldbergKAmper-Ion");
       SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
       fFCKamp.MakeBaseKamp(pRaw, recBase);
       
       
       if((fabs(rec->GetAmp()) > maxPeak) and rec->GetAmp()!=-99999){
         maxPeak = fabs(rec->GetAmp());
         PeakPos = rec->GetPeakPosition();
         precNum = k;
       }
     }
   }
  //cout << "Starting with heat pulses" << endl;
  // create KPulseAnalysisRecords for heat pulses, use largest ionisation pulse to fix the peak position
  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    if(pRaw->GetIsHeatPulse()){
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
      //cout << "KFeldbergKAmpSite heat channel branch for " << pRaw->GetChannelName()<<endl;
      // with peak position fixed by ionisation
      if(!SetupFCKamp(pRaw))
         {cout<<"SetupFCKamp for "<< pRaw->GetChannelName() << "failed"<<endl;return false;}
      if(precNum != -1){
        // Create KPulseAnalysisRecord and a valid KAmpEvent
        
        KPulseAnalysisRecord *recFixed  =  ee->AddPulseAnalysisRecord();
        fFCKamp.SetName("FeldbergKAmper-Heat-Fixed");
        SetTRefLinksForKAmpEvent(recFixed, boloAmp,pAmp); 
       
      
        
      
        KRawBoloPulseRecord *pRawIon = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(precNum);
        if(PeakPos != -1){
          PeakPos = (double) pRaw->GetPretriggerSize()+(pRawIon->GetPulseTimeWidth()*(PeakPos - (double)pRawIon->GetPretriggerSize())/((double)pRaw->GetPulseTimeWidth()));
        }
        fFCKamp.MakeKamp(pRaw, recFixed, PeakPos);
      } 
      
      // without fixed peak position
      KPulseAnalysisRecord *recFree  =  ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Heat-Not-Fixed");
      SetTRefLinksForKAmpEvent(recFree, boloAmp, pAmp);
      fFCKamp.MakeKamp(pRaw, recFree); 
      
      // create  KPulseAnalysisRecord for baseline amplitude estimation
      KPulseAnalysisRecord *recBase = ee->AddPulseAnalysisRecord();
      fFCKamp.SetName("FeldbergKAmper-Heat");
      SetTRefLinksForKAmpEvent(recBase, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      fFCKamp.MakeBaseKamp(pRaw, recBase);
    }
  } 
  return true;
}  

Bool_t KFeldbergKAmpSite::ScoutKampSite(KRawBoloPulseRecord* /*pRaw*/, KRawEvent* /*e*/)
{
  return true;
}  

void KFeldbergKAmpSite::AddIIRFilter(const char* channel, double* a, unsigned int asize, double* b, unsigned int bsize)
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
  fProcessorChain[channel].AddProcessor((KPtaProcessor*) iir); 
  //cout<<"AddIIRFilter done"<<endl;
        
}

Bool_t KFeldbergKAmpSite::SetupFCKamp(KRawBoloPulseRecord* pRaw)
{
  //cout<<"SetupFCKamp start"<<endl;
  if(pRaw->GetIsHeatPulse()){
    // setting up for heat analysis
    //cout << "fHeatPreprocessor for "<< pRaw->GetChannelName()<< "contains " << fHeatPreprocessor.GetNumProcessors() << " processors"<<endl;
    fFCKamp.SetPreprocessor(&fHeatPreprocessor);
    
    if(fProcessorChain.find(pRaw->GetChannelName()) != fProcessorChain.end()){
      //cout << "fProcessorChain for "<< pRaw->GetChannelName()<< "contains " << fProcessorChain[pRaw->GetChannelName()].GetNumProcessors() << " processors"<<endl;
      fFCKamp.SetProcessorChain(&fProcessorChain[pRaw->GetChannelName()]);
    }
    else
      {cout<<"ProcessorChain for "<< pRaw->GetChannelName() << "was not found!" << endl; return false;}
      
    if(fTemplate.find(pRaw->GetChannelName()) != fTemplate.end())
      fFCKamp.SetTemplate(fTemplate[pRaw->GetChannelName()],fAmpEstimatorTimeInTemplate[pRaw->GetChannelName()],fPulseStartTimeInTemplate[pRaw->GetChannelName()]);
    else
      {cout<<"Template for "<<pRaw->GetChannelName()<<" was not found!"<< endl; return false;}    
  }
  else{
    // setting up for ionisation analysis
    
    // checking the modulation frequencies and setting up an ion. preprocessor
    //cout << "ion pulse"<<endl;
    if(fHeatPulseStampWidths != GetHeatPulseStampWidths(pRaw))
    {
      fHeatPulseStampWidths = GetHeatPulseStampWidths(pRaw);
      fIonPreprocessor.DeleteProcessors();
      fIonPreprocessor.AddProcessor(new KBaselineRemoval());
      for(unsigned int i = 0; i < fHeatPulseStampWidths.size(); i++)
      {
        KPatternRemoval* tempPattRv = new KPatternRemoval();
        KPatternRemoval* tempPattRv1 = new KPatternRemoval();
        tempPattRv->SetPatternLength(fHeatPulseStampWidths.at(i));
        tempPattRv1->SetPatternLength(2*fHeatPulseStampWidths.at(i));
        fIonPreprocessor.AddProcessor(tempPattRv);
        fIonPreprocessor.AddProcessor(tempPattRv1);
      }
    }
    
    fFCKamp.SetPreprocessor(&fIonPreprocessor);
    
    if(fProcessorChain.find(pRaw->GetChannelName()) != fProcessorChain.end())
      fFCKamp.SetProcessorChain(&fProcessorChain[pRaw->GetChannelName()]);
    else
      {cout<<"ProcessorChain for "<< pRaw->GetChannelName() << "was not found!" << endl; return false;}
      
    if(fTemplate.find(pRaw->GetChannelName()) != fTemplate.end())
      fFCKamp.SetTemplate(fTemplate[pRaw->GetChannelName()],fAmpEstimatorTimeInTemplate[pRaw->GetChannelName()],fPulseStartTimeInTemplate[pRaw->GetChannelName()]);
    else
      {cout<<"Template for "<<pRaw->GetChannelName()<<" was not found!" << endl; return false;} 
  }
  if((fPosRangeMin.find(pRaw->GetChannelName()) != fPosRangeMin.end()) && (fPosRangeMax.find(pRaw->GetChannelName()) != fPosRangeMax.end()))
    fFCKamp.SetPeakPositionSearchRange(fPosRangeMin[pRaw->GetChannelName()],fPosRangeMax[pRaw->GetChannelName()]);
  
  fFCKamp.SetDoFit(fDoFit);
  //cout<<"SetupFCKamp done"<<endl;
  return true;
}

std::vector<int> KFeldbergKAmpSite::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
{
  KRawBolometerRecord *bolo = pRec->GetBolometerRecord();
  unsigned int counter = 0;
  std::vector<int> HeatPulseStampWidths;
  HeatPulseStampWidths.reserve(6);  
  HeatPulseStampWidths.clear();
  
  for(int i = 0; i < bolo->GetNumPulseRecords(); i++){
    KRawBoloPulseRecord *p = bolo->GetPulseRecord(i);
    if(p->GetIsHeatPulse()){
      counter++;
      bool addValue = true;
      //search through the vector to find out if this stamp width is unique
      for(unsigned int j = 0; j < HeatPulseStampWidths.size(); j++){
        if(HeatPulseStampWidths.at(j) == (int)(p->GetHeatPulseStampWidth()) )
          addValue = false;
      }
      
      if(addValue){
        if(HeatPulseStampWidths.size() < counter)
          HeatPulseStampWidths.push_back( (int)(p->GetHeatPulseStampWidth()) );
        else
          HeatPulseStampWidths.at(counter) = (int)(p->GetHeatPulseStampWidth()) ;
      }
  
    }
  }
  return HeatPulseStampWidths;
}

Bool_t KFeldbergKAmpSite::SetTemplate(const char* channel, std::vector<double> templ, unsigned int pretrigger, unsigned int delta_t, unsigned int width, Bool_t force)
{
  //cout <<"KFeldbergKAmpSite::SetTemplate start"<<endl;
  if(fTemplate.find(channel) != fTemplate.end())
    if(!force)
      return true;
    
  if(fProcessorChain.find(channel) != fProcessorChain.end()){
    //debug
    cout << channel << " unprocessed template: ";
    for(unsigned int i = 0; i < templ.size(); i++)
      cout << templ[i] <<",";
    cout <<endl;
    
    fProcessorChain[channel].SetInputPulse(templ);
    //debug
//     for(unsigned int i = 0; i < fProcessorChain[channel].GetNumProcessors(); i++)
//       cout<<fProcessorChain[channel].GetProcessor(i)->GetName()<<endl;
//     cout << "fProcessorChain contains following processors:"<<endl;
//     for(unsigned int i = 0; i < fProcessorChain[channel].GetNumProcessors();i++){
//       //  cout << fProcessorChain->GetProcessor(i)->GetName() << endl;
//       //cout << ((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefASize()<<" "<<((KIIRFilter*) fProcessorChain->GetProcessor(i))->GetCoefBSize()<<endl;
//       for(unsigned int j = 0; j < ((KIIRFilter*) fProcessorChain[channel].GetProcessor(i))->GetCoefASize();j++)
//         cout<<((KIIRFilter*) fProcessorChain[channel].GetProcessor(i))->GetCoefA()[j]<<" ";
//       cout<<endl;
//       for(unsigned int j = 0; j < ((KIIRFilter*) fProcessorChain[channel].GetProcessor(i))->GetCoefBSize();j++)
//         cout<<((KIIRFilter*) fProcessorChain[channel].GetProcessor(i))->GetCoefB()[j]<<" ";
//       cout<<endl;
//     }
    //debug

    
    if(!fProcessorChain[channel].RunProcess()){
      cout<<"Processing the template failed!"<<endl; return false;}
    
    //debug
//     cout<<"Processed template:";
//     for(unsigned int j = 0; j<fProcessorChain[channel].GetOutputPulseSize(); j++)
//       cout<<fProcessorChain[channel].GetOutputPulse()[j]<<",";
//     cout<<endl;
    //debug
    
    std::vector<double> templvector;
    templvector.resize(width);
    if(fProcessorChain[channel].GetOutputPulseSize() > (pretrigger - delta_t + width))
      for(unsigned int i = pretrigger - delta_t; i < (pretrigger - delta_t + width); i++){
        templvector[i-(pretrigger-delta_t)]=*(fProcessorChain[channel].GetOutputPulse()+i);
      }
    else
      { cout << "(Pretrigger + template width - delta_t) > size of the input template!" << endl; return false;}
    
    unsigned int pos = fFCKamp.GetPositionOfMaxAbsValue(&templvector[0], templvector.size());
    fAmpEstimatorTimeInTemplate[channel] = (double) pos;
    fPulseStartTimeInTemplate[channel] = (double) delta_t;
    // make the template always with positive polarity
    double scale = (fNormalizeTemplate) ? templvector[pos] : (templvector[pos]>0) ? 1:-1;
    cout << "NormalizeTemplate:"<< fNormalizeTemplate <<", Scale:"<< scale << endl;
    cout << "processed,cut and scaled template: ";
    for(unsigned int i = 0; i< templvector.size();i++){
      templvector[i]/=scale;
      cout<<templvector[i]<<",";
    }
    cout<<endl;
    fTemplate[channel] = templvector;
    return true;
  }
  else{
    cout << "fProcessorChain not set! Template cannot be processed!"<<endl;
    return false;
  }
  
   
   
}
