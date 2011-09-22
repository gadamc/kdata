//
// KSimpleKamper1.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KSimpleKamper1.h"

#include "KLinearRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

ClassImp(KSimpleKamper1);

KSimpleKamper1::KSimpleKamper1(void)
{
  
  SetName("KSimpleKamper1");
  fHeatPulseSize = 512;
  fIonPulseSize = 8196;
  fHeatPulse = new double[fHeatPulseSize];
  fIonPulse =  new double[fIonPulseSize];
  SetHeatPointers();
  SetIonPointers();
  fPeakFindHeat.SetHeatDefault();
  fPeakFindHeat.SetPolarity(0);
  fPeakFindIon.SetIonDefault();
  
}

KSimpleKamper1::~KSimpleKamper1(void)
{
  if(fHeatPulse) delete [] fHeatPulse;
  if(fIonPulse) delete [] fIonPulse;
}

void KSimpleKamper1::SetHeatPointers()
{
  fLinRemovalHeat.SetInputPulse(fHeatPulse);
  fLinRemovalHeat.SetInputPulseSize(fHeatPulseSize);
  fLinRemovalHeat.SetOutputPulse(fHeatPulse); //this calculation can be done in-place;
  fLinRemovalHeat.SetOutputPulseSize(fHeatPulseSize);
 
  fPeakFindHeat.SetInputPulse(fHeatPulse);
  fPeakFindHeat.SetInputPulseSize(fHeatPulseSize);
  fPeakFindHeat.SetOutputPulse(fHeatPulse);
  fPeakFindHeat.SetOutputPulseSize(fHeatPulseSize);
}

void KSimpleKamper1::SetIonPointers()
{
  fLinRemovalIon.SetInputPulse(fIonPulse);
  fLinRemovalIon.SetInputPulseSize(fIonPulseSize);
  fLinRemovalIon.SetOutputPulse(fIonPulse); //this calculation can be done in-place;
  fLinRemovalIon.SetOutputPulseSize(fIonPulseSize);
  
  fPatRemoval.SetInputPulse(fIonPulse);
  fPatRemoval.SetInputPulseSize(fIonPulseSize);
  fPatRemoval.SetOutputPulse(fIonPulse);
  fPatRemoval.SetOutputPulseSize(fIonPulseSize);
  
  fPeakFindIon.SetInputPulse(fIonPulse);
  fPeakFindIon.SetInputPulseSize(fIonPulseSize);
  fPeakFindIon.SetOutputPulse(fIonPulse);
  fPeakFindIon.SetOutputPulseSize(fIonPulseSize);
}


void KSimpleKamper1::CheckMemory(KRawBoloPulseRecord *pRec)
{
  if(pRec->GetIsHeatPulse()){
    if(pRec->GetPulseLength() != fHeatPulseSize) {
      delete [] fHeatPulse;
      fHeatPulseSize = pRec->GetPulseLength();
      fHeatPulse = new double[fHeatPulseSize];
      SetHeatPointers();
    }
  }
  else{
    if(pRec->GetPulseLength() != fIonPulseSize) {
      delete [] fIonPulse;
      fIonPulseSize = pRec->GetPulseLength();
      fIonPulse = new double[fIonPulseSize];
      SetIonPointers();
    }
  }
}

Bool_t KSimpleKamper1::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{
  
  CheckMemory(pRec);
  
  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    pRec->GetTrace(fHeatPulse);
    if(fLinRemovalHeat.RunProcess()){
      if(fPeakFindHeat.RunProcess()){
        double maxAmp = 99999.;
        int peakPos = -1;
        for(unsigned int i = 0; i < fPeakFindHeat.GetNumExtraWeakPeaks(); i++){
          if (*(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i)) < maxAmp){
            maxAmp =  *(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i));
            peakPos = fPeakFindHeat.GetExtraWeakPosition(i);
          }
        }
        rec->SetAmp(maxAmp);
        rec->SetPeakPosition(peakPos);  
      }
      else {cerr << "ksimplekamper1 - peak find heat fail." << endl; return false;}
    }
    else {cerr << "ksimplekamper1 - linear removal heat fail." << endl; return false;}
    
  }
  
  //do ion pulse analysis
  else{
    pRec->GetTrace(fIonPulse);
    fPeakFindIon.SetPolarity(pRec->GetPolarity() >= 0 ? 0 : 1);
    
    if(fLinRemovalHeat.RunProcess()){
      if(fPatRemoval.RunProcess()){
        if(fPeakFindHeat.RunProcess()){
          
          double maxAmp = 99999.;
          int peakPos = -1;
          
          if(fPeakFindIon.GetPolarity() == 0){
            for(unsigned int i = 0; i < fPeakFindHeat.GetNumExtraWeakPeaks(); i++){
              if (*(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i)) < maxAmp){
                maxAmp =  *(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i));
                peakPos = fPeakFindHeat.GetExtraWeakPosition(i);
              }
            }
          }
          else {
            maxAmp = -99999.;
            for(unsigned int i = 0; i < fPeakFindHeat.GetNumExtraWeakPeaks(); i++){
              if (*(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i)) > maxAmp){
                maxAmp = *(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i));
                peakPos = fPeakFindHeat.GetExtraWeakPosition(i);
              }
            }
          }
          
          rec->SetAmp(maxAmp);
          rec->SetPeakPosition(peakPos);  
        }
        else {cerr << "ksimplekamper1 - peak find ion fail." << endl; return false;}
      }
      else {cerr << "ksimplekamper1 - pattern removal ion fail." << endl; return false;}
    }
    else {cerr << "ksimplekamper1 - linear removal ion fail." << endl; return false;}
  }
  
  
  return true;
}
