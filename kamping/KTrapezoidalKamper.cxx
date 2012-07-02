//
// KTrapezoidalKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
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



#include "KTrapezoidalKamper.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

using namespace std;

ClassImp(KTrapezoidalKamper);

KTrapezoidalKamper::KTrapezoidalKamper(void)
{

  SetName("KTrapezoidalKamper");
  fHeatPulseSize = 512;
  fIonPulseSize = 8192;
  fHeatPulse = new double[fHeatPulseSize];
  fHeatPulse2 = new double[fHeatPulseSize];
  fIonPulse =  new double[fIonPulseSize];
  fIonPulse2 =  new double[fIonPulseSize];
  fOrderPulseHeat1 = new double[fHeatPulseSize];
  fOrderPulseHeat2 = new double[fHeatPulseSize];
  fOrderPulseIon1 = new double[fIonPulseSize];
  fOrderPulseIon2 = new double[fIonPulseSize];

  
  fTrapHeatTime.SetParams(50., 3, 20);
  fTrapHeatAmplitude.SetParams(50., 10, 50);
  fPeakPositionResultHeatSize = fHeatPulseSize - 2*fTrapHeatTime.GetRiseTime() - fTrapHeatTime.GetFlatTopWidth();
  fPeakPositionResultHeat = new double[fPeakPositionResultHeatSize];

  fTrapIonTime.SetParams(1100., 3, 20);
  fTrapIonAmplitude.SetParams(1100., 100, 100);
  fPeakPositionResultIonSize = fIonPulseSize - 2*fTrapIonTime.GetRiseTime() - fTrapIonTime.GetFlatTopWidth();
  fPeakPositionResultIon = new double[fPeakPositionResultHeatSize];

  fOrderFilter1Heat.SetOrder(1);
  fOrderFilter2Heat.SetOrder(1);
  fOrderFilter1Ion.SetOrder(1);
  fOrderFilter2Ion.SetOrder(1);

  fOrderFilter1Heat.SetInitOutputValue(0.0);
  fOrderFilter2Heat.SetInitOutputValue(0.0);
  fOrderFilter1Ion.SetInitOutputValue(0.0);
  fOrderFilter2Ion.SetInitOutputValue(0.0);

  SetHeatPointers();
  SetIonPointers();
  
  //fDebugMode = false;
  //fDebugResults.clear();
  //fDebugSteps.clear();
}

KTrapezoidalKamper::~KTrapezoidalKamper(void)
{
  if(fHeatPulse) delete [] fHeatPulse;
  if(fHeatPulse2) delete [] fHeatPulse2;
  if(fIonPulse) delete [] fIonPulse;
  if(fIonPulse2) delete [] fIonPulse2;
  if(fOrderPulseHeat1) delete [] fOrderPulseHeat1;
  if(fOrderPulseHeat2) delete [] fOrderPulseHeat2;
  if(fOrderPulseIon1) delete [] fOrderPulseIon1;
  if(fOrderPulseIon2) delete [] fOrderPulseIon2;
  if(fPeakPositionResultHeat) delete [] fPeakPositionResultHeat;
  if(fPeakPositionResultIon) delete [] fPeakPositionResultIon;
}

void KTrapezoidalKamper::SetHeatPointers()
{
  fBaseRemovalHeat.SetInputPulse(fHeatPulse);
  fBaseRemovalHeat.SetInputPulseSize(fHeatPulseSize);
  fBaseRemovalHeat.SetOutputPulse(fHeatPulse); //this calculation can be done in-place;
  fBaseRemovalHeat.SetOutputPulseSize(fHeatPulseSize);

  fTrapHeatTime.SetInputPulse(fHeatPulse);
  fTrapHeatTime.SetInputPulseSize(fHeatPulseSize);
  fTrapHeatTime.SetOutputPulse(fHeatPulse2);
  fTrapHeatTime.SetOutputPulseSize(fHeatPulseSize);

  fTrapHeatAmplitude.SetInputPulse(fHeatPulse);
  fTrapHeatAmplitude.SetInputPulseSize(fHeatPulseSize);
  fTrapHeatAmplitude.SetOutputPulse(fHeatPulse2);
  fTrapHeatAmplitude.SetOutputPulseSize(fHeatPulseSize);

  fOrderFilter1Heat.SetInputPulse(fHeatPulse2);
  fOrderFilter1Heat.SetInputPulseSize(fHeatPulseSize);
  fOrderFilter1Heat.SetOutputPulse(fOrderPulseHeat1);
  fOrderFilter1Heat.SetOutputPulseSize(fHeatPulseSize);

  fOrderFilter2Heat.SetInputPulse(fOrderPulseHeat1);
  fOrderFilter2Heat.SetInputPulseSize(fHeatPulseSize);
  fOrderFilter2Heat.SetOutputPulse(fOrderPulseHeat2);
  fOrderFilter2Heat.SetOutputPulseSize(fHeatPulseSize);

}

void KTrapezoidalKamper::SetIonPointers()
{
  fBaseRemovalIon.SetInputPulse(fIonPulse);
  fBaseRemovalIon.SetInputPulseSize(fIonPulseSize);
  fBaseRemovalIon.SetOutputPulse(fIonPulse); //this calculation can be done in-place;
  fBaseRemovalIon.SetOutputPulseSize(fIonPulseSize);

  fPatRemoval.SetInputPulse(fIonPulse);
  fPatRemoval.SetInputPulseSize(fIonPulseSize);
  fPatRemoval.SetOutputPulse(fIonPulse);
  fPatRemoval.SetOutputPulseSize(fIonPulseSize);


  fTrapIonTime.SetInputPulse(fIonPulse);
  fTrapIonTime.SetInputPulseSize(fIonPulseSize);
  fTrapIonTime.SetOutputPulse(fIonPulse2);
  fTrapIonTime.SetOutputPulseSize(fIonPulseSize);

  fTrapIonAmplitude.SetInputPulse(fIonPulse);
  fTrapIonAmplitude.SetInputPulseSize(fIonPulseSize);
  fTrapIonAmplitude.SetOutputPulse(fIonPulse2);
  fTrapIonAmplitude.SetOutputPulseSize(fIonPulseSize);

  fOrderFilter1Ion.SetInputPulse(fIonPulse2);
  fOrderFilter1Ion.SetInputPulseSize(fIonPulseSize);
  fOrderFilter1Ion.SetOutputPulse(fOrderPulseIon1);
  fOrderFilter1Ion.SetOutputPulseSize(fIonPulseSize);

  fOrderFilter2Ion.SetInputPulse(fOrderPulseIon1);
  fOrderFilter2Ion.SetInputPulseSize(fIonPulseSize);
  fOrderFilter2Ion.SetOutputPulse(fOrderPulseIon2);
  fOrderFilter2Ion.SetOutputPulseSize(fIonPulseSize);

}


void KTrapezoidalKamper::CheckMemory(KRawBoloPulseRecord *pRec)
{
  if(pRec->GetIsHeatPulse()){
    if(pRec->GetPulseLength() != fHeatPulseSize) {
      delete [] fHeatPulse;
      delete [] fHeatPulse2;
      delete [] fOrderPulseHeat1;
      delete [] fOrderPulseHeat2;
      delete [] fPeakPositionResultHeat;
      fHeatPulseSize = pRec->GetPulseLength();
      fPeakPositionResultHeatSize = fHeatPulseSize - 2*fTrapHeatTime.GetRiseTime() - fTrapHeatTime.GetFlatTopWidth();
      fPeakPositionResultHeat = new double[fPeakPositionResultHeatSize];
      fHeatPulse = new double[fHeatPulseSize];
      fHeatPulse2 = new double[fHeatPulseSize];
      fOrderPulseHeat1 = new double[fHeatPulseSize];
      fOrderPulseHeat2 = new double[fHeatPulseSize];
      SetHeatPointers();
    }
  }
  else{
    if(pRec->GetPulseLength() != fIonPulseSize) {      
      delete [] fIonPulse;
      delete [] fIonPulse2;
      delete [] fOrderPulseIon1;
      delete [] fOrderPulseIon2;
      delete [] fPeakPositionResultIon;
      fIonPulseSize = pRec->GetPulseLength();
      fPeakPositionResultIonSize = fIonPulseSize - 2*fTrapIonTime.GetRiseTime() - fTrapIonTime.GetFlatTopWidth();
      fPeakPositionResultIon = new double[fPeakPositionResultHeatSize];
      fIonPulse = new double[fIonPulseSize];
      fIonPulse2 = new double[fIonPulseSize];
      fOrderPulseIon1 = new double[fIonPulseSize];
      fOrderPulseIon2 = new double[fIonPulseSize];
      SetIonPointers();
    }
  }
}

Bool_t KTrapezoidalKamper::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{

  rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0) 
    return true;


  //if (IsDebugMode()){
  //  fDebugSteps.clear();
  //  fDebugResults.clear();
  //}


  CheckMemory(pRec);
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

    pRec->GetTrace(fHeatPulse);

    if(fBaseRemovalHeat.RunProcess()){
      
      //if (IsDebugMode()) 
      //  FillKamperDebugResults(fBaseRemovalHeat);

      if(fTrapHeatTime.RunProcess()){

        //if (IsDebugMode()) 
        //  FillKamperDebugResults(fTrapHeatTime);

        //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
        fOrderFilter1Heat.RunProcess();
        //if (IsDebugMode()) 
        //  FillKamperDebugResults(fOrderFilter1Heat);

        fOrderFilter2Heat.RunProcess();
        //if (IsDebugMode()) 
        //  FillKamperDebugResults(fOrderFilter2Heat);

        maxPeakPos = FindMaxPeak(fOrderFilter2Heat.GetOutputPulseSize(), fOrderFilter2Heat.GetOutputPulse(),
          fTrapHeatTime.GetRiseTime(), fTrapHeatTime.GetFlatTopWidth(),
          fPeakPositionResultHeat, fPeakPositionResultHeatSize, fTrapHeatTime.GetOutputPulse(), 
          fTrapHeatTime.GetOutputPulseSize(), -1);

        if(fTrapHeatAmplitude.RunProcess()){
          //if (IsDebugMode()) 
          //  FillKamperDebugResults(fTrapHeatAmplitude);

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

    pRec->GetTrace(fIonPulse);

    //break me - need to do something different to get the heat pulse width
    fPatRemoval.SetPatternLength(pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.

    if( fPatRemoval.RunProcess()){
      //if (IsDebugMode()) 
      //  FillKamperDebugResults(fPatRemoval);

      //break me - need to do something different to get the heat pulse width

      fPatRemoval.SetPatternLength(2.*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      if( fPatRemoval.RunProcess()){
        //if (IsDebugMode()) 
        //  FillKamperDebugResults(fPatRemoval);

        if(fBaseRemovalIon.RunProcess()){
          //if (IsDebugMode()) 
          //  FillKamperDebugResults(fBaseRemovalIon);

          if(fTrapIonTime.RunProcess()){
            //if (IsDebugMode()) 
            //  FillKamperDebugResults(fTrapIonTime);

          //using the "OrderFilter" to calculate the second derivative (is there a faster way?)
            fOrderFilter1Ion.RunProcess();
            //if (IsDebugMode()) 
            //  FillKamperDebugResults(fOrderFilter1Ion);

            fOrderFilter2Ion.RunProcess();
            //if (IsDebugMode()) 
            //  FillKamperDebugResults(fOrderFilter2Ion);

            maxPeakPos = FindMaxPeak(fOrderFilter2Ion.GetOutputPulseSize(), fOrderFilter2Ion.GetOutputPulse(),
              fTrapIonTime.GetRiseTime(), fTrapIonTime.GetFlatTopWidth(),
              fPeakPositionResultIon, fPeakPositionResultIonSize, fTrapIonTime.GetOutputPulse(), 
              fTrapIonTime.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1);

            if(fTrapIonAmplitude.RunProcess()){
              //if (IsDebugMode()) 
              //  FillKamperDebugResults(fTrapIonAmplitude);

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

void KTrapezoidalKamper::FindPeaks(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
  unsigned int flatTopTime, double* result, unsigned int resultSize, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  
  double amp = polarity* 1.*fRms.GetRms(secondOrderPulse, 0, secondOrderPulseLength);

  for(unsigned int i = 0; i < resultSize; i++){
    if ((secondOrderPulse[i]<amp) && (secondOrderPulse[i+riseTime]>-amp) && (secondOrderPulse[i+riseTime+flatTopTime]>-amp) 
      && (secondOrderPulse[i+2*riseTime+flatTopTime]<amp))
      result[i] = secondOrderPulse[i+riseTime];  
    else  
      result[i] = 0.;
  }
} 

unsigned int KTrapezoidalKamper::FindMaxPeak(unsigned int secondOrderPulseLength, double* secondOrderPulse, unsigned int riseTime, 
  unsigned int flatTopTime, double* result, unsigned int resultSize, double* trapOut, unsigned int trapOutSize, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  
  FindPeaks(secondOrderPulseLength, secondOrderPulse, riseTime, flatTopTime, result, resultSize, polarity);

  double maxValue = 0;
  unsigned int maxPos = 0;

  for(unsigned int i = 0; i < resultSize && i < trapOutSize-(riseTime+flatTopTime); i++){
    if( TMath::Abs(result[i] * trapOut[i+riseTime+flatTopTime]) > maxValue){
      maxValue = TMath::Abs(result[i] * trapOut[i+riseTime+flatTopTime]);
      maxPos = i;
    }
  }

  return maxPos;
}

double KTrapezoidalKamper::GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double mean = 0;
  unsigned int i = first;

  for( ; i < last+1 && i < pulseLength; i++)
    mean += pulse[i];

  return (i > first) ? mean/(i-first) : -1*polarity*99999;
}
// 
// void KTrapezoidalKamper::FillKamperDebugResults(KPtaProcessor &mProcessor)
// {
//   vector<double > linRemoved;
//   fDebugSteps.push_back(mProcessor.GetName());
//   cout << "filling debug results for process: " << mProcessor.GetName() << endl;
//   for(unsigned int i = 0; i < mProcessor.GetOutputPulseSize(); i++)
//     linRemoved.push_back(mProcessor.GetOutputPulse()[i]);
//   fDebugResults.push_back(linRemoved);
// }
