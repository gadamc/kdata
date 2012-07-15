//
// KSimpleKamper1.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KSimpleKamper1.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

//ClassImp(KSimpleKamper1);

KSimpleKamper1::KSimpleKamper1(void) : 
  //bandpass iir filter between 0.001 and 0.01 of the nyquist frequncy
  //for heat signals, the nyquist frequency is typically 500 Hz
  //for ion signals, its is 50 kHz. 
  // so the bandpass for heat (ion) is between 0.5 (50) and 5 (500) Hz
fIir4Heat(-3.95982379,  5.88045441, -3.88143355,  0.96080294, 0.00019593,  0.        , -0.00039186,  0.        ,  0.00019593),
fIir4Ion(-3.95982379,  5.88045441, -3.88143355,  0.96080294, 0.00019593,  0.        , -0.00039186,  0.        ,  0.00019593)
{
  
  SetName("KSimpleKamper1");
  fHeatPulseSize = 512;
  fIonPulseSize = 8196;
  fHeatPulse = new double[fHeatPulseSize];
  fHeatPulse2 = new double[fHeatPulseSize];
  fIonPulse =  new double[fIonPulseSize];
  fIonPulse2 =  new double[fIonPulseSize];

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
  if(fHeatPulse2) delete [] fHeatPulse2;
  if(fIonPulse2) delete [] fIonPulse2;
}

void KSimpleKamper1::SetHeatPointers()
{
  fLinRemovalHeat.SetInputPulse(fHeatPulse);
  fLinRemovalHeat.SetInputPulseSize(fHeatPulseSize);
  fLinRemovalHeat.SetOutputPulse(fHeatPulse); //this calculation can be done in-place;
  fLinRemovalHeat.SetOutputPulseSize(fHeatPulseSize);
 
  fIir4Heat.SetInputPulse(fHeatPulse);
  fIir4Heat.SetInputPulseSize(fHeatPulseSize);
  fIir4Heat.SetOutputPulse(fHeatPulse2);
  fIir4Heat.SetOutputPulseSize(fHeatPulseSize);
 
  fPeakFindHeat.SetInputPulse(fHeatPulse2);
  fPeakFindHeat.SetInputPulseSize(fHeatPulseSize);
  fPeakFindHeat.SetOutputPulse(fHeatPulse2);
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
  
  fIir4Ion.SetInputPulse(fIonPulse);
  fIir4Ion.SetInputPulseSize(fIonPulseSize);
  fIir4Ion.SetOutputPulse(fIonPulse2);
  fIir4Ion.SetOutputPulseSize(fIonPulseSize);
  
  fPeakFindIon.SetInputPulse(fIonPulse2);
  fPeakFindIon.SetInputPulseSize(fIonPulseSize);
  fPeakFindIon.SetOutputPulse(fIonPulse2);
  fPeakFindIon.SetOutputPulseSize(fIonPulseSize);
}


void KSimpleKamper1::CheckMemory(KRawBoloPulseRecord *pRec)
{
  if(pRec->GetIsHeatPulse()){
    if(pRec->GetPulseLength() != fHeatPulseSize) {
      if(fHeatPulse) delete [] fHeatPulse;
      if(fHeatPulse2) delete [] fHeatPulse2;
      fHeatPulseSize = pRec->GetPulseLength();
      fHeatPulse = new double[fHeatPulseSize];
      fHeatPulse2 = new double[fHeatPulseSize];
      SetHeatPointers();
    }
  }
  else{
    if(pRec->GetPulseLength() != fIonPulseSize) {
      if(fIonPulse) delete [] fIonPulse;
      if(fIonPulse2) delete [] fIonPulse2;
      fIonPulseSize = pRec->GetPulseLength();
      fIonPulse = new double[fIonPulseSize];
      fIonPulse2 = new double[fIonPulseSize];
      SetIonPointers();
    }
  }
}

map<string, KResult > KSimpleKamper1::MakeKamp(KRawBoloPulseRecord * pRec)
{
    
  CheckMemory(pRec);
  
  map<string, vector<double> > myResults;

  if(pRec->GetPulseLength() == 0)
    return myResults;

  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    pRec->GetTrace(fHeatPulse);
    if(fLinRemovalHeat.RunProcess()){
      if(fIir4Heat.RunProcess()){
        if(fPeakFindHeat.RunProcess()){
          double maxAmp = 99999.;
          int peakPos = -1;
          for(unsigned int i = 0; i < fPeakFindHeat.GetNumExtraWeakPeaks(); i++){
            if (*(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i)) < maxAmp){
              maxAmp =  *(fPeakFindHeat.GetOutputPulse()+fPeakFindHeat.GetExtraWeakPosition(i));
              peakPos = fPeakFindHeat.GetExtraWeakPosition(i);
            }
          }
          myResults["amp"] = KResult("amp", maxAmp, "ADU");
          myResults["peakPosition"] = KResult("peakPosition", peakPos, "bin");
          //rec->SetAmp(maxAmp);
          //rec->SetPeakPosition(peakPos); 
          //rec->SetName(GetName());
          //rec->SetUnit(0);
          myResults["baselineRemoved"] = KResult("baselineRemoved", fLinRemovalHeat.GetBaselineOffset(), "ADU");
          myResults["slopeRemoved"] = KResult("slopeRemoved", fLinRemovalHeat.GetSlope(), "ADU/bin");
          //rec->SetBaselineRemoved(fLinRemovalHeat.GetBaselineOffset());
          //rec->SetSlopeRemoved(fLinRemovalHeat.GetSlope());
        }
        else {cerr << "ksimplekamper1 - peak find heat fail." << endl; return myResults;}
      } 
      else {cerr << "ksimplekamper1 - iir filter heat fail." << endl; return myResults;}
    }
    else {cerr << "ksimplekamper1 - linear removal heat fail." << endl; return myResults;}
    
  }
  
  //do ion pulse analysis
  else{
    pRec->GetTrace(fIonPulse);
    fPeakFindIon.SetPolarity(pRec->GetPolarity() >= 0 ? 0 : 1);
    
    if(fLinRemovalIon.RunProcess()){
      if(fPatRemoval.RunProcess()){
        if(fIir4Ion.RunProcess()){
          if(fPeakFindIon.RunProcess()){

            double maxAmp = 99999.;
            int peakPos = -1;

            if(fPeakFindIon.GetPolarity() == 0){
              for(unsigned int i = 0; i < fPeakFindIon.GetNumExtraWeakPeaks(); i++){
                if (*(fPeakFindIon.GetOutputPulse()+fPeakFindIon.GetExtraWeakPosition(i)) < maxAmp){
                  maxAmp =  *(fPeakFindIon.GetOutputPulse()+fPeakFindIon.GetExtraWeakPosition(i));
                  peakPos = fPeakFindIon.GetExtraWeakPosition(i);
                }
              }
            }
            else {
              maxAmp = -99999.;
              for(unsigned int i = 0; i < fPeakFindIon.GetNumExtraWeakPeaks(); i++){
                if (*(fPeakFindIon.GetOutputPulse()+fPeakFindIon.GetExtraWeakPosition(i)) > maxAmp){
                  maxAmp = *(fPeakFindIon.GetOutputPulse()+fPeakFindIon.GetExtraWeakPosition(i));
                  peakPos = fPeakFindIon.GetExtraWeakPosition(i);
                }
              }
            }

            myResults["amp"] = KResult("amp", maxAmp, "ADU");
            myResults["peakPosition"] = KResult("peakPosition", peakPos, "bin");
            //rec->SetAmp(maxAmp);
            //rec->SetPeakPosition(peakPos); 
            //rec->SetName(GetName());
            //rec->SetUnit(0);
            myResults["baselineRemoved"] = KResult("baselineRemoved", fLinRemovalIon.GetBaselineOffset(), "ADU");
            myResults["slopeRemoved"] = KResult("slopeRemoved", fLinRemovalIon.GetSlope(), "ADU/bin");
            //rec->SetBaselineRemoved(fLinRemovalIon.GetBaselineOffset());
          }
          else {cerr << "ksimplekamper1 - peak find ion fail." << endl; return myResults;}
        }
        else {cerr << "ksimplekamper1 - iir filter ion fail." << endl; return myResults;}
      }
      else {cerr << "ksimplekamper1 - pattern removal ion fail." << endl; return myResults;}
    }
    else {cerr << "ksimplekamper1 - linear removal ion fail." << endl; return myResults;}
  }
  
  
  return myResults;
}
