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
//  0              Trapezoidal Filter for Pulse Amplitude - decay slope
//  1              Trapezoidal Filter for Pulse Amplitude - rise time
//  2              Trapezoidal Filter for Pulse Amplitude - flat top width

#include "KTrapKamperProto.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

#include <stdexcept>


using namespace std;

KTrapKamperProto::KTrapKamperProto(void)
{

  SetName("KTrapKamperProto");
  
  AddTrapHeatTime(50., 3, 5);
  AddTrapHeatTime(50., 5, 5);
  AddTrapHeatTime(50., 7, 5);
  
  fTrapHeatAmplitude.SetParams(50., 10, 50);

  AddTrapIonTime(1100., 3, 10);
  AddTrapIonTime(1100., 5, 10);
  AddTrapIonTime(1100., 7, 10);
  
  fTrapIonAmplitude.SetParams(1100., 100, 400);

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
    rec->SetExtra(fTrapHeatAmplitude.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapHeatAmplitude.GetRiseTime() ,1);
    rec->SetExtra(fTrapHeatAmplitude.GetFlatTopWidth(), 2);

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if(fBaseRemovalHeat.RunProcess()){
      
      maxPeakPos = RunHeatPulseStartTime();
        
      fTrapHeatAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if(fTrapHeatAmplitude.RunProcess()){

        rec->SetAmp(GetMean(maxPeakPos + fTrapHeatAmplitude.GetRiseTime(), maxPeakPos + fTrapHeatAmplitude.GetRiseTime() + 
            fTrapHeatAmplitude.GetFlatTopWidth()/2, fTrapHeatAmplitude.GetOutputPulse(), fTrapHeatAmplitude.GetOutputPulseSize(), -1) );
        rec->SetPeakPosition(maxPeakPos);
        rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
        rec->SetIsBaseline(false);
        //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
          
      } else {cout << "fTrapHeatAmplitude  fail" << endl; return false;}
    }  else {cout << "fBaseRemovalHeat  fail" << endl; return false;}
  }

  //do ion pulse analysis
  else{
    rec->SetExtra(fTrapIonAmplitude.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapIonAmplitude.GetRiseTime(), 1);
    rec->SetExtra(fTrapIonAmplitude.GetFlatTopWidth(), 2);

    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    fPatRemoval.SetPatternLength(1*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.

    if( fPatRemoval.RunProcess()){

      fPatRemoval.SetPatternLength(2*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      if( fPatRemoval.RunProcess()){
        
        fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
        if(fBaseRemovalIon.RunProcess()){

          maxPeakPos = RunIonPulseStartTime(pRec->GetPolarity() > 0 ? -1 : 1);
            
          fTrapIonAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
          if(fTrapIonAmplitude.RunProcess()){

            rec->SetAmp(GetMean(maxPeakPos + fTrapIonAmplitude.GetRiseTime(), maxPeakPos + fTrapIonAmplitude.GetRiseTime() + 
                fTrapIonAmplitude.GetFlatTopWidth()/2, fTrapIonAmplitude.GetOutputPulse(), fTrapIonAmplitude.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1) );
            rec->SetPeakPosition(maxPeakPos);
            rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
            rec->SetIsBaseline(false);
            //rec->SetSlopeRemoved(fBaseRemovalIon.GetSlope());
          } else {cout << "fTrapIonAmplitude  fail" << endl; return false;}
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
    rec->SetExtra(fTrapHeatAmplitude.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapHeatAmplitude.GetRiseTime() ,1);
    rec->SetExtra(fTrapHeatAmplitude.GetFlatTopWidth(), 2);

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if(fBaseRemovalHeat.RunProcess()){
          
      maxPeakPos = 0;
        
      fTrapHeatAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if(fTrapHeatAmplitude.RunProcess()){

        rec->SetAmp(GetMean(maxPeakPos + fTrapHeatAmplitude.GetRiseTime(), maxPeakPos + fTrapHeatAmplitude.GetRiseTime() + 
            fTrapHeatAmplitude.GetFlatTopWidth()/2, fTrapHeatAmplitude.GetOutputPulse(), fTrapHeatAmplitude.GetOutputPulseSize(), -1) );
        rec->SetPeakPosition(maxPeakPos);
        rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
        rec->SetIsBaseline(true);
        //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
          
      } else {cout << "fTrapHeatAmplitude  fail" << endl; return false;}
    }  else {cout << "fBaseRemovalHeat  fail" << endl; return false;}
  }

  //do ion pulse analysis
  else{
    rec->SetExtra(fTrapIonAmplitude.GetDecayTimeConstant(), 0);
    rec->SetExtra(fTrapIonAmplitude.GetRiseTime(), 1);
    rec->SetExtra(fTrapIonAmplitude.GetFlatTopWidth(), 2);
    

    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    fPatRemoval.SetPatternLength(1*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.

    if( fPatRemoval.RunProcess()){

      fPatRemoval.SetPatternLength(2*pRec->GetHeatPulseStampWidth()); //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      if( fPatRemoval.RunProcess()){
        
        fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
        if(fBaseRemovalIon.RunProcess()){

          maxPeakPos = 0;
            
          fTrapIonAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
          if(fTrapIonAmplitude.RunProcess()){

            rec->SetAmp(GetMean(maxPeakPos + fTrapIonAmplitude.GetRiseTime(), maxPeakPos + fTrapIonAmplitude.GetRiseTime() + 
                fTrapIonAmplitude.GetFlatTopWidth()/2, fTrapIonAmplitude.GetOutputPulse(), fTrapIonAmplitude.GetOutputPulseSize(), pRec->GetPolarity() > 0 ? -1 : 1) );
            rec->SetPeakPosition(maxPeakPos);
            rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
            rec->SetIsBaseline(true);
            //rec->SetSlopeRemoved(fBaseRemovalIon.GetSlope());
          } else {cout << "fTrapIonAmplitude  fail" << endl; return false;}
        } else {cout << "fBaseRemovalIon  fail" << endl; return false;}
      } else {cout << "fPatRemoval  fail" << endl; return false;}
    } else {cout << "fPatRemoval  fail" << endl; return false;}
  }


  return true;
}

void KTrapKamperProto::FillPeakPositionResult(KOrderFilter& fOrderFilter, KTrapezoidalFilter* trap, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  double *secondOrderPulse = fOrderFilter.GetOutputPulse();
  unsigned int secondOrderPulseSize = fOrderFilter.GetOutputPulseSize();
  unsigned int riseTime = trap->GetRiseTime();
  unsigned int flatTopTime = trap->GetFlatTopWidth();
  
  double amp = polarity* 1.*fRms.GetRms(secondOrderPulse, 0, secondOrderPulseSize);
  
  unsigned int resultSize = secondOrderPulseSize - 2*riseTime- flatTopTime;
  
  try{
    for(unsigned int i = 0; i < resultSize; i++){
      if ((secondOrderPulse[i]<amp) && (secondOrderPulse[i+riseTime]>-amp) && (secondOrderPulse[i+riseTime+flatTopTime]>-amp) 
        && (secondOrderPulse[i+2*riseTime+flatTopTime]<amp))
        fPeakPositionResult.at(i) += trap->GetOutputPulse()[i+riseTime+flatTopTime/2] * secondOrderPulse[i+riseTime];  
      else  
        fPeakPositionResult.at(i) += 0.;
    }
  }
  catch (out_of_range& oor) {
    cerr << "Out of Range error: " << oor.what() << endl;
    throw oor;
  }
} 

unsigned int KTrapKamperProto::FindMaxPeak(vector<double>& pulse, unsigned int maxPosition)
{

  
  double maxValue = 0;
  unsigned int maxPos = 0;

  for(unsigned int i = 0; i < pulse.size() && i < maxPosition; i++){
    if( TMath::Abs(pulse[i]) > maxValue){
      maxValue = TMath::Abs(pulse[i]);
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

KTrapezoidalFilter* KTrapKamperProto::AddTrapIonTime(double decay, unsigned int rise, unsigned int flat)
{
  return AddTrapTime(fTrapIonTime, decay, rise, flat);
  
} 

KTrapezoidalFilter* KTrapKamperProto::AddTrapHeatTime(double decay, unsigned int rise, unsigned int flat)
{
  return AddTrapTime(fTrapHeatTime, decay, rise, flat);

} 

KTrapezoidalFilter* KTrapKamperProto::AddTrapTime(vector<KTrapezoidalFilter *>& trapVec, double decay, unsigned int rise, unsigned int flat)
{
  KTrapezoidalFilter* n = new KTrapezoidalFilter;
  n->SetParams(decay, rise, flat);
  trapVec.push_back(n);
  return n;
}

unsigned int KTrapKamperProto::RunHeatPulseStartTime(void)
{
  return RunPulseStartTime(fTrapHeatTime, fOrderFilter1Heat, fOrderFilter2Heat, fBaseRemovalHeat, -1);
}

unsigned int KTrapKamperProto::RunIonPulseStartTime(int polarity)
{
  return RunPulseStartTime(fTrapIonTime, fOrderFilter1Ion, fOrderFilter2Ion, fBaseRemovalIon, polarity);   
}

unsigned int KTrapKamperProto::RunPulseStartTime(vector<KTrapezoidalFilter *>& trapVec, KOrderFilter& ord1, KOrderFilter& ord2, KPtaProcessor& fromProcessor, int polarity)
{
  
  vector<KTrapezoidalFilter* >::iterator it;
  fPeakPositionResult.clear();
  fPeakPositionResult.resize(fromProcessor.GetOutputPulseSize());
  unsigned int maxPeakSize = fPeakPositionResult.size();
  
  for( it = trapVec.begin(); it < trapVec.end(); it++){
    (*it)->SetInputPulse(fromProcessor.GetOutputPulse(), fromProcessor.GetOutputPulseSize());
    if( !(*it)->RunProcess() ){
      cerr << "KTrapKamperProto::RunPulseStartTime. A KTrapezoidal Filter Failed (decay, rise, flat): " 
        << (*it)->GetDecayTimeConstant() << "," << (*it)->GetRiseTime() << "," << (*it)->GetFlatTopWidth() << endl;
      return 0;
    }
    else{
      
      if (maxPeakSize > (*it)->GetOutputPulseSize() - (*it)->GetRiseTime() - (*it)->GetFlatTopWidth())
        maxPeakSize = (*it)->GetOutputPulseSize() - (*it)->GetRiseTime() - (*it)->GetFlatTopWidth();
       
      //using the "OrderFilter" to calculate the second derivative (there could be a faster way)
      ord1.SetInputPulse((*it)->GetOutputPulse(), (*it)->GetOutputPulseSize());
      ord1.RunProcess();

      ord2.SetInputPulse(ord1.GetOutputPulse(), ord1.GetOutputPulseSize());
      ord2.RunProcess();
      try{
        FillPeakPositionResult(ord2, (*it), polarity);
      }
      catch (out_of_range& oor) {
        return 0;
      }
      
    }
  }
  
  
  return FindMaxPeak(fPeakPositionResult, maxPeakSize);
    
}

