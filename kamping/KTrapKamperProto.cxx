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
#include "KPulsePolarityCalculator.h"
#include "KRawBolometerRecord.h"

#include <stdexcept>

using namespace std;

KTrapKamperProto::KTrapKamperProto(void)
{

  SetName("KTrapKamperProto");
  
  //its important to use different rise-times here, but keep the same flattop width
  //AddTrapHeatTime(20., 3, 25);
  //AddTrapHeatTime(20., 7, 25);
  //AddTrapHeatTime(20., 15, 0);
  //AddTrapHeatTime(25., 3, 0);
  //AddTrapHeatTime(25., 7, 0);
  AddTrapHeatTime(25., 17, 0);
  AddTrapHeatTime(25., 29, 0); 
  fDefaultTrapHeatAmplitudeParameters.push_back(20.0); //decay constant
  fDefaultTrapHeatAmplitudeParameters.push_back(10.0); //rise time
  fDefaultTrapHeatAmplitudeParameters.push_back(30.0); //width
  fTrapAmplitude.SetParams(fDefaultTrapHeatAmplitudeParameters[0], 
                              fDefaultTrapHeatAmplitudeParameters[1],
                              fDefaultTrapHeatAmplitudeParameters[2]);

  //AddTrapIonTime(400., 3, 40);
  //AddTrapIonTime(400., 7, 40);
  //AddTrapIonTime(400., 15, 40);
  //AddTrapIonTime(900., 3, 70);
  //AddTrapIonTime(900., 7, 0);
  AddTrapIonTime(800., 23, 0);
  AddTrapIonTime(800., 31, 0);
  
  fDefaultTrapIonAmplitudeParameters.push_back(1000.0); //decay constant
  fDefaultTrapIonAmplitudeParameters.push_back(50.0); //rise time
  fDefaultTrapIonAmplitudeParameters.push_back(200.0); //width
  fTrapAmplitude.SetParams(fDefaultTrapIonAmplitudeParameters[0], 
                              fDefaultTrapIonAmplitudeParameters[1],
                              fDefaultTrapIonAmplitudeParameters[2]);

  fOrderFilter1Heat.SetOrder(1);
  fOrderFilter2Heat.SetOrder(1);
  fOrderFilter1Ion.SetOrder(1);
  fOrderFilter2Ion.SetOrder(1);

  fOrderFilter1Heat.SetInitOutputValue(0.0);
  fOrderFilter2Heat.SetInitOutputValue(0.0);
  fOrderFilter1Ion.SetInitOutputValue(0.0);
  fOrderFilter2Ion.SetInitOutputValue(0.0);

  fPeakPositionSearchAmplifier = 2.7;
  
  fHeatPulseStampWidths.reserve(6);
}

KTrapKamperProto::~KTrapKamperProto(void)
{
  
}

Bool_t KTrapKamperProto::MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec )
{
  return MakeKamp(rawPulseRecord, rec, -1);
}


Bool_t KTrapKamperProto::MakeBaseKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
{
  Bool_t theRet = MakeKamp(pRec, rec, -2);
  rec->SetIsBaseline(true);
  return theRet;
}

Bool_t KTrapKamperProto::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
  rec->SetIsBaseline(false); 
  //rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0){
    //cerr << "KTrapKamperProto::MakeKamp. Pulse Length is zero." << endl;
    fPeakPositionResult.clear();
    return false;
  }
    

  double maxPeakPos = 0;


  FillTrapAmplitudeParameters(pRec->GetChannelName(), pRec->GetIsHeatPulse());
  //record the settings of the trapezoidal filters in the pulseAnalysisRecord.  This MUST be documented
  //at the beginning of this class.
  rec->SetExtra(fTrapAmplitude.GetDecayTimeConstant(), 0);
  rec->SetExtra(fTrapAmplitude.GetRiseTime() ,1);
  rec->SetExtra(fTrapAmplitude.GetFlatTopWidth(), 2);
  
  
  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if( !fBaseRemovalHeat.RunProcess())
      {cout << "fBaseRemovalHeat failed" << endl; return false;}
      
    if(fixPeakPosition == -1)
      maxPeakPos = RunHeatPulseStartTime();
    else if (fixPeakPosition == -2) {
      maxPeakPos = fTrapAmplitude.GetRiseTime()*2 + fTrapAmplitude.GetFlatTopWidth();
    }
    else maxPeakPos = fixPeakPosition;
    
    fTrapAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
    if( !fTrapAmplitude.RunProcess())
      {cout << "fTrapAmplitude failed" << endl; return false;}

     
    rec->SetAmp(GetMean((int)maxPeakPos + fTrapAmplitude.GetRiseTime(), (int)maxPeakPos + fTrapAmplitude.GetRiseTime() + 
        fTrapAmplitude.GetFlatTopWidth()/2, fTrapAmplitude.GetOutputPulse(), fTrapAmplitude.GetOutputPulseSize(), -1) );
   
    rec->SetPeakPosition(maxPeakPos);
    rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
          
    return true;
  }

  //do ion pulse analysis
  else{
    
    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    if (pRec->GetBoloBoxVersion() < 2.0){
      GetHeatPulseStampWidths(pRec); //this fills a local vector<double>
      for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){
        fPatRemoval.SetPatternLength(1*fHeatPulseStampWidths.at(h)); 
        if( !fPatRemoval.RunProcess())
          {cout << "fPatRemoval failed" << endl; return false;}
      }
    }
    else {
      fPatRemoval.SetPatternLength(10);  //for some reason, the bbv2 have a "pattern" of 10  bins in period. Some very strong 10 kHz noise 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return false;}
    }
    
    fPatRemoval.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
    
    for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){  //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      fPatRemoval.SetPatternLength(2*fHeatPulseStampWidths.at(h)); 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return false;}
    }
    
    
    if (pRec->GetBoloBoxVersion() < 2.0){
      fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
      if( !fBaseRemovalIon.RunProcess())
        {cout << "fBaseRemovalIon failed" << endl; return false;}
      fTrapAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
      rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
    }
    else {
      fLineRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
      if( !fLineRemovalIon.RunProcess())
        {cout << "fLineRemovalIon failed" << endl; return false;}
      fTrapAmplitude.SetInputPulse(fLineRemovalIon.GetOutputPulse(), fLineRemovalIon.GetOutputPulseSize());
      rec->SetBaselineRemoved(fLineRemovalIon.GetOffset());
      rec->SetExtra(fLineRemovalIon.GetSlope(), 3);
    }
    
    
    if(fixPeakPosition == -1)
      if ( pRec->GetBoloBoxVersion() < 2.0) 
        maxPeakPos = RunIonPulseStartTime( fBaseRemovalIon, KPulsePolarityCalculator::GetExpectedPolarity(pRec) );
      else
        maxPeakPos = RunIonPulseStartTime( fLineRemovalIon, KPulsePolarityCalculator::GetExpectedPolarity(pRec) );
    else if (fixPeakPosition == -2) {
      maxPeakPos = fTrapAmplitude.GetRiseTime()*2 + fTrapAmplitude.GetFlatTopWidth();
    }
    else maxPeakPos = fixPeakPosition;
      
    
    if( !fTrapAmplitude.RunProcess())
      {cout << "fTrapAmplitude failed" << endl; return false;}
                    
    rec->SetAmp(GetMean((int)maxPeakPos + fTrapAmplitude.GetRiseTime(), (int)maxPeakPos + fTrapAmplitude.GetRiseTime() + 
      fTrapAmplitude.GetFlatTopWidth()/2, fTrapAmplitude.GetOutputPulse(), fTrapAmplitude.GetOutputPulseSize(), 
       KPulsePolarityCalculator::GetExpectedPolarity(pRec) ));
    
    rec->SetPeakPosition(maxPeakPos);
    
    return true;
   
   }

}

void KTrapKamperProto::FillTrapAmplitudeParameters(const char* channelName, bool isHeatPulse)
{
  double decayConst, riseTime, width;
  
  if(isHeatPulse){
    decayConst = fDefaultTrapHeatAmplitudeParameters[0];
    riseTime = fDefaultTrapHeatAmplitudeParameters[1];
    width = fDefaultTrapHeatAmplitudeParameters[2];
  }
  else{
    decayConst = fDefaultTrapHeatAmplitudeParameters[0];
    riseTime = fDefaultTrapHeatAmplitudeParameters[1];
    width = fDefaultTrapHeatAmplitudeParameters[2];
  }
  
  if(GetTrapAmplitudeDecayConstant(channelName) != -1){
    decayConst = GetTrapAmplitudeDecayConstant(channelName);
  }
  
  fTrapAmplitude.SetParams(decayConst,riseTime, width);
}

double KTrapKamperProto::GetTrapAmplitudeDecayConstant(const char* channelName) const
{
  if (fTrapAmplitudeDecayConstants.find(channelName) != fTrapAmplitudeDecayConstants.end()){
    return fTrapAmplitudeDecayConstants.find(channelName)->second;
  }
  else return -1;
} 


void KTrapKamperProto::FillPeakPositionResult(KOrderFilter& fOrderFilter, KTrapezoidalFilter* trap, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  double *secondOrderPulse = fOrderFilter.GetOutputPulse();
  unsigned int secondOrderPulseSize = fOrderFilter.GetOutputPulseSize();
  unsigned int riseTime = trap->GetRiseTime();
  unsigned int flatTopTime = trap->GetFlatTopWidth();
  
  double amp = polarity * fPeakPositionSearchAmplifier * fRms.GetRms(secondOrderPulse, 0, secondOrderPulseSize/3.);
  
  unsigned int resultSize = secondOrderPulseSize - 2*riseTime- flatTopTime;
  
  try{
    for(unsigned int i = 0; i < resultSize; i++){
      if(polarity < 0) {
        if ((secondOrderPulse[i]<amp) && (secondOrderPulse[i+riseTime]>-amp) && (secondOrderPulse[i+riseTime+flatTopTime]>-amp) 
        && (secondOrderPulse[i+2*riseTime+flatTopTime]<amp))
          //fPeakPositionResult.at(i) += trap->GetOutputPulse()[i+riseTime] * secondOrderPulse[i+riseTime];  
        fPeakPositionResult.at(i) += secondOrderPulse[i+riseTime] * -1.;
        else  
          fPeakPositionResult.at(i) += 0.;
      }
      else {
        if ((secondOrderPulse[i]>amp) && (secondOrderPulse[i+riseTime]<-amp) && (secondOrderPulse[i+riseTime+flatTopTime]<-amp) 
        && (secondOrderPulse[i+2*riseTime+flatTopTime]>amp))
          //fPeakPositionResult.at(i) += trap->GetOutputPulse()[i+riseTime] * secondOrderPulse[i+riseTime];  
          fPeakPositionResult.at(i) += secondOrderPulse[i+riseTime];  
        else  
          fPeakPositionResult.at(i) += 0.;
      }
    }
  }
  catch (out_of_range& oor) {
    cerr << "Out of Range error: " << oor.what() << endl;
    throw oor;
  }
} 

unsigned int KTrapKamperProto::FindPeak(vector<double>& pulse, unsigned int maxPosition)
{
  //returns the position of the minimum value point in the pulse
  
  double minValue = 0;
  unsigned int minPos = 0;

  for(unsigned int i = 0; i < pulse.size() && i < maxPosition; i++){
    if( pulse[i] < minValue){
      minValue = pulse[i];
      minPos = i;
    }
  }

  return minPos;
}

double KTrapKamperProto::GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double mean = 0;
  unsigned int i = first;

  for( ; i < last+1 && i < pulseLength; i++)
    mean += pulse[i];

  return (i > first) ? mean/(i-first) : -1*polarity*99999;
}

double KTrapKamperProto::GetMax(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double max = 0;
  unsigned int i = first;
  
  for( ; i < last+1 && i < pulseLength; i++){
    if(polarity*pulse[i] > max) max = polarity*pulse[i];
  }

  return (i > first) ? polarity*max : -1*polarity*99999;
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
  KTrapezoidalFilter* n = new KTrapezoidalFilter();
  n->SetParams(decay, rise, flat);
  trapVec.push_back(n);
  return n;
}

unsigned int KTrapKamperProto::RunHeatPulseStartTime(void)
{
  return RunPulseStartTime(fTrapHeatTime, fOrderFilter1Heat, fOrderFilter2Heat, fBaseRemovalHeat, -1);
}

unsigned int KTrapKamperProto::RunIonPulseStartTime(KPtaProcessor& fromProcessor, int polarity)
{
  return RunPulseStartTime(fTrapIonTime, fOrderFilter1Ion, fOrderFilter2Ion, fromProcessor, polarity);   
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
  
  
  return FindPeak(fPeakPositionResult, maxPeakSize);
    
}

std::vector<int>& KTrapKamperProto::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
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

