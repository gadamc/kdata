//_____________________________________________
// KFeldbergKAmpSite.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//
// Extra field definition for output KPulseAnalysisRecord:
// 0 : NDF of the fit -- fit with floating start time
// 1 : ROOT's TMinuit Fit return output - 0=good fit.
//
// 2 : RMS of the first 40% of the processed pulse
// 3 : RMS of preprocessed pulse
// 4 : RMS of processed pulse
//
// 5: order of lowpass filter -- butterworth
// 6: corner frequency of lowpass
// 7: order of highpass filter
// 8: corner frequency of highpass
//
// 9 : Baseline estimation value - amplitude estimate of the fit with a fixed start time set to fBaselinePosition.
// 10 : fBaselinePosition - the fixed position of the pulse peak time in the Baseline amplitude estimate fit..
// 11 : baseline fit result -- ROOT's TMinuit Fit return output for baseline fit
//
// 12 : Heat channel amplitude estimate from the fit with the peak time fixed to the estimated Ionization pulse time. Heat channel only. This will be empty for ionization channels.
// 13 : Fixed pulse time -- the estimated Ionization pulse time used in the fit. This will be empty for ionization channels.
// 14 : fixed pulse time fit result -- ROOT's TMinuit Fit return output for Heat channel fit with the peak time fixed to the Ionization peak time. This will be empty for ionization channels.

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
#include "KLinearRemoval.h"
#include "KIIRFilter.h"
#include "KIIRFirstOrder.h"
#include "KIIRSecondOrder.h"
#include "KIIRThirdOrder.h"
#include "KIIRFourthOrder.h"
#include "TString.h"
#include "KResult.h"
#include <iostream>
#include <math.h>


using namespace std;


KFeldbergKAmpSite::KFeldbergKAmpSite(void)
{
  SetName("KFeldbergKAmpSite");
  SetTitle("KFeldbergKAmpSite");
  fHeatPreprocessor.SetIsOwner(true);
  fHeatPreprocessor.AddProcessor(new KBaselineRemoval());
  fFCKamp.SetName("KFeldbergKAmpSite");

}

KFeldbergKAmpSite::~KFeldbergKAmpSite(void)
{
  //

}

Bool_t KFeldbergKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{

  double PeakPos = -1;
  double maxPeak = 0.;
  int precNum = -1;

  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
   KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
   if(!pRaw->GetIsHeatPulse() && pRaw->GetPulseLength() != 0){

     if(!SetupFCKamp(pRaw)) continue;

     KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);
     KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
     SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);
     map<string, KResult> kampResults = fFCKamp.MakeKamp(pRaw);

     FillResults(rec, kampResults);


     if((fabs(rec->GetAmp()) > maxPeak) and rec->GetAmp()!=-99999){
       maxPeak = fabs(rec->GetAmp());
       PeakPos = rec->GetPeakPosition();
       precNum = k;
     }

   }
 }


 for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
  KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
  if(pRaw->GetIsHeatPulse() && pRaw->GetPulseLength() != 0){

    if(!SetupFCKamp(pRaw)) continue;

    KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp);

    int heatPeakPos = -1;

    if(precNum != -1){      
      KRawBoloPulseRecord *pRawIon = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(precNum);
      if(PeakPos != -1){
        heatPeakPos = (int)(  (double) pRaw->GetPretriggerSize()+(pRawIon->GetPulseTimeWidth()*(PeakPos - (double)pRawIon->GetPretriggerSize())/((double)pRaw->GetPulseTimeWidth()))  );
      }
    } 

    KPulseAnalysisRecord *rec  =  ee->AddPulseAnalysisRecord();
    SetTRefLinksForKAmpEvent(rec, boloAmp, pAmp);

    map<string, KResult> kampResults = fFCKamp.MakeKamp(pRaw, heatPeakPos); 

    FillResults(rec, kampResults);

  }
} 
return true;
}  

void KFeldbergKAmpSite::FillResults(KPulseAnalysisRecord* rec, map<string, KResult> &resMap)
{
  if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
  if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
  if(resMap.find("chiSq") != resMap.end())                  rec->SetChiSq(resMap["chiSq"].fValue); 
  if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);
  if(resMap.find("slopeRemoved") != resMap.end())           rec->SetSlopeRemoved(resMap["slopeRemoved"].fValue);

  if(resMap.find("ndfFloat") != resMap.end())               rec->SetExtra(resMap["ndfFloat"].fValue, 0);
  if(resMap.find("fitResult") != resMap.end())              rec->SetExtra(resMap["fitResult"].fValue, 1);
  if(resMap.find("baselineRmsPostProc") != resMap.end())    rec->SetExtra(resMap["baselineRmsPostProc"].fValue, 2);
  if(resMap.find("rmsPreProc") != resMap.end())             rec->SetExtra(resMap["rmsPreProc"].fValue, 3);
  if(resMap.find("rmsPostProc") != resMap.end())            rec->SetExtra(resMap["rmsPostProc"].fValue, 4);


  rec->SetExtra(fLowPassFilterOrder,5);
  rec->SetExtra(fLowPassFilterCorner, 6);
  rec->SetExtra(fHighPassFilterOrder,7);
  rec->SetExtra(fHighPassFilterCorner, 8);


  if(resMap.find("ampFixHeatPosition") != resMap.end())             rec->SetExtra(resMap["ampFixHeatPosition"].fValue, 9);
  if(resMap.find("fixHeatPosition") != resMap.end())                rec->SetExtra(resMap["fixHeatPosition"].fValue, 10);
  if(resMap.find("fitResults_fixHeatPosition") != resMap.end())     rec->SetExtra(resMap["fitResults_fixHeatPosition"].fValue, 11);

  if(resMap.find("ampFixedPositionFromIon") != resMap.end())        rec->SetExtra(resMap["ampFixedPositionFromIon"].fValue, 12);
  if(resMap.find("fixedPositionFromIon") != resMap.end())           rec->SetExtra(resMap["fixedPositionFromIon"].fValue, 13);
  if(resMap.find("fitResults_fixedPositionFromIon") != resMap.end())    rec->SetExtra(resMap["fitResults_fixedPositionFromIon"].fValue, 14);

  rec->SetName(fFCKamp.GetName());
  rec->SetUnit(0);
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
        fProcessorChain[channel].AddProcessor(iir);
  fProcessorChain[channel].SetIsOwner(true);  //make sure that the KPulseAnalysisChain deletes these objects

}

Bool_t KFeldbergKAmpSite::SetupFCKamp(KRawBoloPulseRecord* pRaw)
{

  if(pRaw->GetIsHeatPulse()){

    fFCKamp.SetPreprocessor(&fHeatPreprocessor);      
  }
  else{
    // setting up for ionisation analysis

    //get pulse stamp widths only if haven't already got them.
    if(fHeatPulseStampWidths.find(pRaw->GetChannelName()) == fHeatPulseStampWidths.end()){
      fHeatPulseStampWidths[pRaw->GetChannelName()] = GetHeatPulseStampWidths(pRaw);
    }

    //create a new ion preproc only if needed.
    if(fIonPreprocessor.find(pRaw->GetChannelName()) == fIonPreprocessor.end()){  
      if(pRaw->GetBoloBoxVersion()==2.0)
        fIonPreprocessor[pRaw->GetChannelName()].AddProcessor(new KLinearRemoval());
      else
        fIonPreprocessor[pRaw->GetChannelName()].AddProcessor(new KBaselineRemoval());

      set<int>& stampWidths = fHeatPulseStampWidths[pRaw->GetChannelName()];
      set<int>::iterator stampIter;
      for ( stampIter = stampWidths.begin() ; stampIter != stampWidths.end(); stampIter++ ){
        for(unsigned int ii = 1; ii <= 2; ii++){  //from experience we know that pattern removal of heat cross-talk 
                                                  //can be improved by also removing a pattern of 2x heat pulse width
          KPatternRemoval *temp = new KPatternRemoval();
          temp->SetPatternLength(*stampIter * ii);
          fIonPreprocessor[pRaw->GetChannelName()].AddProcessor(temp);
        }
      }
      fIonPreprocessor[pRaw->GetChannelName()].SetIsOwner(true);
    }  
    

    fFCKamp.SetPreprocessor(&fIonPreprocessor[pRaw->GetChannelName()]);      
  }

  if(fProcessorChain.find(pRaw->GetChannelName()) != fProcessorChain.end())
    fFCKamp.SetPostProcessor(&fProcessorChain[pRaw->GetChannelName()]);
  else  return false;

  if(fTemplate.find(pRaw->GetChannelName()) != fTemplate.end())
    fFCKamp.SetTemplate( fTemplate[pRaw->GetChannelName()], fAmpEstimatorTimeInTemplate[pRaw->GetChannelName()],
			 fPulseStartTimeInTemplate[pRaw->GetChannelName()], fTemplate[ pRaw->GetChannelName() ][ (int)fAmpEstimatorTimeInTemplate[pRaw->GetChannelName() ] ]);
  else return false; 

  if((fPosRangeMin.find(pRaw->GetChannelName()) != fPosRangeMin.end()) && (fPosRangeMax.find(pRaw->GetChannelName()) != fPosRangeMax.end()))
    fFCKamp.SetPeakPositionSearchRange(fPosRangeMin[pRaw->GetChannelName()],fPosRangeMax[pRaw->GetChannelName()]);
  else return false;

  if((fFixedPosition.find(pRaw->GetChannelName()) != fFixedPosition.end()))
    fFCKamp.SetBaselinePosition(fFixedPosition[pRaw->GetChannelName()]);
  else return false;
  
  return true;
}

std::set<int> KFeldbergKAmpSite::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
{
  //returns the set of heat pulse stamp widths. The reason for a set is that there can 
  //be two NTDs per bolometer and they could have the same heat pulse widths. Using a set
  //means that I don't have to check if the NTDs have the same pulse widths. 
  //so, this is general enough to return all of the unique heat pulse
  //widths for each NTD connected to each bolometer. This even works if there are more than
  //two NTDs per bolometer, which might be kinda cool... but probably not.

  KRawBolometerRecord *bolo = pRec->GetBolometerRecord();
  std::set<int> HeatPulseStampWidths;
  
  for(int i = 0; i < bolo->GetNumPulseRecords(); i++){
    KRawBoloPulseRecord *p = bolo->GetPulseRecord(i);
    if(p->GetIsHeatPulse())
      HeatPulseStampWidths.insert( (int)(p->GetHeatPulseStampWidth()) );  
  }
  return HeatPulseStampWidths;
}

Bool_t KFeldbergKAmpSite::SetTemplate(const char* channel, std::vector<double> templ, unsigned int pretrigger, unsigned int delta_t, unsigned int width, Bool_t force)
{
  //Sets the template pulse for a channel if it has not already been set. However, if the template for channel has already
  //been set, you and reset the template if you set force=true
  //
  //You MUST ONLY CALL this method after you have completely set up the fProcessorChain, which is set up by
  //adding all of the IIRFilters you wish to use.

  if(fTemplate.find(channel) != fTemplate.end())
    if(!force)
      return true;
    
    if(fProcessorChain.find(channel) != fProcessorChain.end()){

      fProcessorChain[channel].SetInputPulse(templ);

      if(!fProcessorChain[channel].RunProcess()){
        cout<<"Processing the template failed!"<<endl; return false;}

        fTemplate[channel].clear();
        fTemplate[channel].resize(width);

        if(fProcessorChain[channel].GetOutputPulseSize() > (pretrigger - delta_t + width)){
          for(unsigned int i = pretrigger - delta_t; i < (pretrigger - delta_t + width); i++){
            fTemplate[channel][i-(pretrigger-delta_t)]=*(fProcessorChain[channel].GetOutputPulse()+i);
          }
        }
        else
          { cout << "(Pretrigger + template width - delta_t) > size of the input template!" << endl; return false;}

        unsigned int pos = fFCKamp.GetPositionOfMaxAbsValue(&fTemplate[channel][0], fTemplate[channel].size());
        fAmpEstimatorTimeInTemplate[channel] = (double) pos;
        fPulseStartTimeInTemplate[channel] = (double) delta_t;

    // make the template always with positive polarity
        double scale = (fNormalizeTemplate) ? fTemplate[channel][pos] : (fTemplate[channel][pos]>0) ? 1:-1;

        for(unsigned int i = 0; i< fTemplate[channel].size();i++){
          fTemplate[channel][i]/=scale;
        }
        return true;
      }
      else{
        cout << "fProcessorChain not set! Template cannot be processed!"<<endl;
        return false;
      }



    }
