//
// KTrapKamperProto.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Implementation of using just trapezoidal filters to measure the pulse peak and amplitude.
// This is called the "Prototype" because it does nothing smart with memory management. Thus
// it is not optimized for performance, but rather optimized for fewer bugs! 
//
// This class stores some data in the fExtra space in the KPulseAnalysisRecord. In these extra
// spaces, it stores some of the settings used in the calculation of the pulse.
//
//  fExtra Index  |  Description of value
//  ---------------------------------------
//  0              Trapezoidal Filter for Pulse Time - decay slope
//  1              Trapezoidal Filter for Pulse Time - rise time
//  2              Trapezoidal Filter for Pulse Time - flat top width
//  3              Trapezoidal Filter for Pulse Amplitude - decay slope
//  4              Trapezoidal Filter for Pulse Amplitude - rise time
//  5              Trapezoidal Filter for Pulse Amplitude - flat top width

#include "KTrapKamperProto.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

using namespace std;

KTrapKamperProto::KTrapKamperProto(void)
{

  SetName("KTrapKamperProto");
  
  fTrapHeatTime.SetParams(50., 3, 20);
  fTrapHeatAmplitude.SetParams(50., 10, 50);

  fTrapIonTime.SetParams(1100., 3, 20);
  fTrapIonAmplitude.SetParams(1100., 100, 100);

  fOrderFilter1Heat.SetOrder(1);
  fOrderFilter2Heat.SetOrder(1);
  fOrderFilter1Ion.SetOrder(1);
  fOrderFilter2Ion.SetOrder(1);

  fOrderFilter1Heat.SetInitOutputValue(0.0);
  fOrderFilter2Heat.SetInitOutputValue(0.0);
  fOrderFilter1Ion.SetInitOutputValue(0.0);
  fOrderFilter2Ion.SetInitOutputValue(0.0);

  
}

KTrapKamperProto::~KTrapKamperProto(void)
{
  
}

Bool_t KTrapKamperProto::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{
  rec->SetIsBaseline(false); 
  rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0) 
    return true;

  unsigned int maxPeakPos = 0;

  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    //record the settings of the trapezoidal filters in the pulseAnalysisRecord.  This MUST be documented
    //at the beginning of this class.
    rec->SetExtra(fTrapHeatTime.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapHeatTime.GetRiseTime(), 1);
    rec->SetExtra(fTrapHeatTime.GetFlatTopWidth(), 2);
    rec->SetExtra(fTrapHeatAmplitude.GetDecayTimeConstant(), 3);
    rec->SetExtra(fTrapHeatAmplitude.GetRiseTime() ,4);
    rec->SetExtra(fTrapHeatAmplitude.GetFlatTopWidth(), 5);

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if(fBaseRemovalHeat.RunProcess()){
      
      fTrapHeatTime.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if(fTrapHeatTime.RunProcess()){

        //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
        fOrderFilter1Heat.SetInputPulse(fTrapHeatTime.GetOutputPulse(), fTrapHeatTime.GetOutputPulseSize());
        fOrderFilter1Heat.RunProcess();
        
        fOrderFilter2Heat.SetInputPulse(fOrderFilter1Heat.GetOutputPulse(), fOrderFilter1Heat.GetOutputPulseSize());
        fOrderFilter2Heat.RunProcess();
       

        maxPeakPos = FindMaxPeak(fOrderFilter2Heat.GetOutputPulseSize(), fOrderFilter2Heat.GetOutputPulse(),
          fTrapHeatTime.GetRiseTime(), fTrapHeatTime.GetFlatTopWidth(), fTrapHeatTime.GetOutputPulse(), fTrapHeatTime.GetOutputPulseSize(), -1);
        
        fTrapHeatAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
        if(fTrapHeatAmplitude.RunProcess()){

          rec->SetAmp(GetMean(maxPeakPos + fTrapHeatAmplitude.GetRiseTime(), maxPeakPos + fTrapHeatAmplitude.GetRiseTime() + 
            fTrapHeatAmplitude.GetFlatTopWidth()/2., fTrapHeatAmplitude.GetOutputPulse(), fTrapHeatAmplitude.GetOutputPulseSize(), -1) );
          rec->SetPeakPosition(maxPeakPos);
          rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
          //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
          
        } else {cout << "fTrapHeatAmplitude  fail" << endl; return false;}
      } else {cout << "fTrapHeatTime  fail" << endl; return false;}  
    }  else {cout << "fBaseRemovalHeat  fail" << endl; return false;}
  }

  //do ion pulse analysis
  else{
    rec->SetExtra(fTrapIonTime.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapIonTime.GetRiseTime(), 1);
    rec->SetExtra(fTrapIonTime.GetFlatTopWidth(), 2);
    rec->SetExtra(fTrapIonAmplitude.GetDecayTimeConstant(), 3);
    rec->SetExtra(fTrapIonAmplitude.GetRiseTime(), 4);
    rec->SetExtra(fTrapIonAmplitude.GetFlatTopWidth(), 5);

    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    fPatRemoval.SetPatternLength(pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.

    if( fPatRemoval.RunProcess()){

      fPatRemoval.SetPatternLength(2.*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      if( fPatRemoval.RunProcess()){
        
        fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
        if(fBaseRemovalIon.RunProcess()){

          fTrapIonTime.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
          if(fTrapIonTime.RunProcess()){

            //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
            fOrderFilter1Ion.SetInputPulse(fTrapIonTime.GetOutputPulse(), fTrapIonTime.GetOutputPulseSize());
            fOrderFilter1Ion.RunProcess();
            
            fOrderFilter2Ion.SetInputPulse(fOrderFilter1Ion.GetOutputPulse(), fOrderFilter1Ion.GetOutputPulseSize());
            fOrderFilter2Ion.RunProcess();

            maxPeakPos = FindMaxPeak(fOrderFilter2Ion.GetOutputPulseSize(), fOrderFilter2Ion.GetOutputPulse(),
              fTrapIonTime.GetRiseTime(), fTrapIonTime.GetFlatTopWidth(),fTrapIonTime.GetOutputPulse(), 
              fTrapIonTime.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1);
              
            fTrapIonAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
            if(fTrapIonAmplitude.RunProcess()){

              rec->SetAmp(GetMean(maxPeakPos + fTrapIonAmplitude.GetRiseTime(), maxPeakPos + fTrapIonAmplitude.GetRiseTime() + 
                fTrapIonAmplitude.GetFlatTopWidth()/2., fTrapIonAmplitude.GetOutputPulse(), fTrapIonAmplitude.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1) );
              rec->SetPeakPosition(maxPeakPos);
              rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
              //rec->SetSlopeRemoved(fBaseRemovalIon.GetSlope());
            } else {cout << "fTrapIonAmplitude  fail" << endl; return false;}
          } else {cout << "fTrapIonTime  fail" << endl; return false;}
        } else {cout << "fBaseRemovalIon  fail" << endl; return false;}
      } else {cout << "fPatRemoval  fail" << endl; return false;}
    } else {cout << "fPatRemoval  fail" << endl; return false;}
  }


  return true;
}

Bool_t KTrapKamperProto::MakeBaseKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{
  rec->SetIsBaseline(false); 
  rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0) 
    return true;

  unsigned int maxPeakPos = 0;

  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    //record the settings of the trapezoidal filters in the pulseAnalysisRecord.  This MUST be documented
    //at the beginning of this class.
    rec->SetExtra(fTrapHeatTime.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapHeatTime.GetRiseTime(), 1);
    rec->SetExtra(fTrapHeatTime.GetFlatTopWidth(), 2);
    rec->SetExtra(fTrapHeatAmplitude.GetDecayTimeConstant(), 3);
    rec->SetExtra(fTrapHeatAmplitude.GetRiseTime() ,4);
    rec->SetExtra(fTrapHeatAmplitude.GetFlatTopWidth(), 5);

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if(fBaseRemovalHeat.RunProcess()){
      
      fTrapHeatTime.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if(fTrapHeatTime.RunProcess()){

        //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
        fOrderFilter1Heat.SetInputPulse(fTrapHeatTime.GetOutputPulse(), fTrapHeatTime.GetOutputPulseSize());
        fOrderFilter1Heat.RunProcess();
        
        fOrderFilter2Heat.SetInputPulse(fOrderFilter1Heat.GetOutputPulse(), fOrderFilter1Heat.GetOutputPulseSize());
        fOrderFilter2Heat.RunProcess();
       

        maxPeakPos = 0;
        
        fTrapHeatAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
        if(fTrapHeatAmplitude.RunProcess()){

          rec->SetAmp(GetMean(maxPeakPos + fTrapHeatAmplitude.GetRiseTime(), maxPeakPos + fTrapHeatAmplitude.GetRiseTime() + 
            fTrapHeatAmplitude.GetFlatTopWidth()/2., fTrapHeatAmplitude.GetOutputPulse(), fTrapHeatAmplitude.GetOutputPulseSize(), -1) );
          rec->SetPeakPosition(maxPeakPos);
          rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
          //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
          
        } else {cout << "fTrapHeatAmplitude  fail" << endl; return false;}
      } else {cout << "fTrapHeatTime  fail" << endl; return false;}  
    }  else {cout << "fBaseRemovalHeat  fail" << endl; return false;}
  }

  //do ion pulse analysis
  else{
    rec->SetExtra(fTrapIonTime.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapIonTime.GetRiseTime(), 1);
    rec->SetExtra(fTrapIonTime.GetFlatTopWidth(), 2);
    rec->SetExtra(fTrapIonAmplitude.GetDecayTimeConstant(), 3);
    rec->SetExtra(fTrapIonAmplitude.GetRiseTime(), 4);
    rec->SetExtra(fTrapIonAmplitude.GetFlatTopWidth(), 5);

    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    fPatRemoval.SetPatternLength(pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.

    if( fPatRemoval.RunProcess()){

      fPatRemoval.SetPatternLength(2.*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      if( fPatRemoval.RunProcess()){
        
        fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
        if(fBaseRemovalIon.RunProcess()){

          fTrapIonTime.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
          if(fTrapIonTime.RunProcess()){

            //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
            fOrderFilter1Ion.SetInputPulse(fTrapIonTime.GetOutputPulse(), fTrapIonTime.GetOutputPulseSize());
            fOrderFilter1Ion.RunProcess();
            
            fOrderFilter2Ion.SetInputPulse(fOrderFilter1Ion.GetOutputPulse(), fOrderFilter1Ion.GetOutputPulseSize());
            fOrderFilter2Ion.RunProcess();

            maxPeakPos = 0;
            
            fTrapIonAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
            if(fTrapIonAmplitude.RunProcess()){

              rec->SetAmp(GetMean(maxPeakPos + fTrapIonAmplitude.GetRiseTime(), maxPeakPos + fTrapIonAmplitude.GetRiseTime() + 
                fTrapIonAmplitude.GetFlatTopWidth()/2., fTrapIonAmplitude.GetOutputPulse(), fTrapIonAmplitude.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1) );
              rec->SetPeakPosition(maxPeakPos);
              rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
              //rec->SetSlopeRemoved(fBaseRemovalIon.GetSlope());
            } else {cout << "fTrapIonAmplitude  fail" << endl; return false;}
          } else {cout << "fTrapIonTime  fail" << endl; return false;}
        } else {cout << "fBaseRemovalIon  fail" << endl; return false;}
      } else {cout << "fPatRemoval  fail" << endl; return false;}
    } else {cout << "fPatRemoval  fail" << endl; return false;}
  }


  return true;
}

void KTrapKamperProto::FindPeaks(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
  unsigned int flatTopTime, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  
  double amp = polarity* 1.*fRms.GetRms(secondOrderPulse, 0, secondOrderPulseLength);
  fPeakPositionResult.resize(0);
  
  unsigned int resultSize = secondOrderPulseLength - 2*riseTime- flatTopTime;
  
  for(unsigned int i = 0; i < resultSize; i++){
    if ((secondOrderPulse[i]<amp) && (secondOrderPulse[i+riseTime]>-amp) && (secondOrderPulse[i+riseTime+flatTopTime]>-amp) 
      && (secondOrderPulse[i+2*riseTime+flatTopTime]<amp))
      fPeakPositionResult.push_back(secondOrderPulse[i+riseTime]);  
    else  
      fPeakPositionResult.push_back(0.);
  }
} 

unsigned int KTrapKamperProto::FindMaxPeak(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
  unsigned int flatTopTime, double* trapOut, unsigned int trapOutSize, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  
  FindPeaks(secondOrderPulseLength, secondOrderPulse, riseTime, flatTopTime, polarity);

  double maxValue = 0;
  unsigned int maxPos = 0;

  for(unsigned int i = 0; i < fPeakPositionResult.size() && i < trapOutSize-(riseTime+flatTopTime); i++){
    if( TMath::Abs(fPeakPositionResult[i] * trapOut[i+riseTime+flatTopTime]) > maxValue){
      maxValue = TMath::Abs(fPeakPositionResult[i] * trapOut[i+riseTime+flatTopTime]);
      maxPos = i;
    }
  }

  return maxPos;
}

double KTrapKamperProto::GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double mean = 0;
  unsigned int i = first;

  for( ; i < last+1 && i < pulseLength; i++)
    mean += pulse[i];

  return (i > first) ? mean/(i-first) : -1*polarity*99999;
}

