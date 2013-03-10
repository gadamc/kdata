//_____________________________________________
// KTrapKamperProto1.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Implementation of using just trapezoidal filters to measure the pulse peak and amplitude.
// This is called the "Prototype" because it does nothing smart with memory management. Thus
// it is not optimized for performance, but rather optimized for fewer bugs! 
//
// returned by MakeKamp method
//
// map<string, KResult>
//
//  string        :    description
// amp                pulse height amplitude estimate
// peakPosition       pulse peak position estimate
// baselineRemoved    baseline DC offset
// trapDecayTime      decay time of the trapezoidal filter
// trapRiseTime       rise time of the filter
// trapFlatTopWidth   width of output trapezoid
// pileup             indicator for potential pile up. 1 if likely pileup, 0 if not
//



#include "KTrapKamperProto1.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KPulsePolarityCalculator.h"
#include "KRawBolometerRecord.h"

#include <stdexcept>

using namespace std;

KTrapKamperProto1::KTrapKamperProto1(void)
{

  SetName("KTrapKamperProto1");
  
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
			   (unsigned int)fDefaultTrapHeatAmplitudeParameters[1],
			   (unsigned int)fDefaultTrapHeatAmplitudeParameters[2]);

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
			   (unsigned int)fDefaultTrapIonAmplitudeParameters[1],
			   (unsigned int)fDefaultTrapIonAmplitudeParameters[2]);

  fOrderFilter1Heat.SetOrder(1);
  fOrderFilter2Heat.SetOrder(1);
  fOrderFilter1Ion.SetOrder(1);
  fOrderFilter2Ion.SetOrder(1);

  fOrderFilter1Heat.SetInitOutputValue(0);
  fOrderFilter2Heat.SetInitOutputValue(0);
  fOrderFilter1Ion.SetInitOutputValue(0);
  fOrderFilter2Ion.SetInitOutputValue(0);

  fPeakPositionSearchAmplifier = 2.7;
  
  fHeatPulseStampWidths.reserve(6);
  
  fMaxIonPeakDetector.AddFilteringProcess(100, 0, 0.5,  1,  1, false,   true, false);
  fMaxIonPeakDetector.AddFilteringProcess(50, 0, 0.5, 1, 1, false, true, false);
  fMaxIonPeakDetector.AddFilteringProcess(10, 0, 1.0, 1, 0, false, true,  true);
  fMaxIonPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true,  true);
  
  fMultipleIonPeakDetector.AddFilteringProcess(100, 0, 0.5,  1,  1, true,   true, false);
  fMultipleIonPeakDetector.AddFilteringProcess(50, 0, 0.5, 1, 1, false, true, false);
  fMultipleIonPeakDetector.AddFilteringProcess(10, 0, 1.0, 1, 0, false, true,  true);
  fMultipleIonPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true,  true);
  
  fMaxHeatPeakDetector.AddFilteringProcess(20, 0, 0.5,  1,  1, false,   true, false);
  fMaxHeatPeakDetector.AddFilteringProcess(10, 0, 0.5, 1, 1, false, true, false);
  fMaxHeatPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true, true);
  
  fMultipleHeatPeakDetector.AddFilteringProcess(20, 0, 0.5,  1,  1, true,   true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(10, 0, 0.5, 1, 1, false, true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true, true);
}

KTrapKamperProto1::~KTrapKamperProto1(void)
{
  
}

std::map<std::string, KResult>& KTrapKamperProto1::MakeKamp(KRawBoloPulseRecord * rawPulseRecord )
{
  return MakeKamp(rawPulseRecord, -1);
}


std::map<std::string, KResult>& KTrapKamperProto1::MakeBaseKamp(KRawBoloPulseRecord * pRec)
{
  return MakeKamp(pRec,  0);
}

std::map<std::string, KResult>& KTrapKamperProto1::MakeKamp(KRawBoloPulseRecord * pRec, double fixPeakPosition)
{

  //rec->SetName(GetName());
  //rec->SetUnit(0);


  if(pRec->GetPulseLength() == 0){
    //cerr << "KTrapKamperProto1::MakeKamp. Pulse Length is zero." << endl;
    fPeakPositionResult.clear();
    return fResults;
  }
    

  double maxPeakPos = 0;
  std::vector< std::vector<double> > remainingPeaks;


  FillTrapAmplitudeParameters(pRec->GetChannelName(), pRec->GetIsHeatPulse());
  //record the settings of the trapezoidal filters in the pulseAnalysisRecord.  This MUST be documented
  //at the beginning of this class.
  fResults["trapDecayTime"] = KResult("trapDecayTime", fTrapAmplitude.GetDecayTimeConstant(), "bin");
  fResults["trapRiseTime"] = KResult("trapRiseTime", fTrapAmplitude.GetRiseTime(), "bin");
  fResults["trapFlatTopWidth"] = KResult("trapFlatTopWidth", fTrapAmplitude.GetFlatTopWidth(), "bin");
  //rec->SetExtra(fTrapAmplitude.GetDecayTimeConstant(), 0);
  //rec->SetExtra(fTrapAmplitude.GetRiseTime() ,1);
  //rec->SetExtra(fTrapAmplitude.GetFlatTopWidth(), 2);
  
  
  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    

    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if( !fBaseRemovalHeat.RunProcess())
      {cout << "fBaseRemovalHeat failed" << endl; return fResults;}
      
    if(fixPeakPosition == -1){
      // Maximal peak detection
      fMaxHeatPeakDetector.SetDecayTimeConstant(fTrapAmplitude.GetDecayTimeConstant());
      fMaxHeatPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
      fMaxHeatPeakDetector.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if( !fMaxHeatPeakDetector.RunProcess())
        {cout << "fMaxHeatPeakDetector failed" << endl; return fResults;}
      remainingPeaks = fMaxHeatPeakDetector.GetRemainingPeaks();
      maxPeakPos = (remainingPeaks.size() == 1) ? remainingPeaks[0][0] : -1;
    }      
    else maxPeakPos = fixPeakPosition;
    
    fTrapAmplitude.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
    if( !fTrapAmplitude.RunProcess())
      {cout << "fTrapAmplitude failed" << endl; return fResults;}
    
     //Pile-up detection
    fMultipleHeatPeakDetector.SetDecayTimeConstant(fTrapAmplitude.GetDecayTimeConstant());
    fMultipleHeatPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
    fMultipleHeatPeakDetector.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
    if( !fMultipleHeatPeakDetector.RunProcess())
      {cout << "fMultipleHeatPeakDetector failed" << endl; return fResults;}
    remainingPeaks = fMultipleHeatPeakDetector.GetRemainingPeaks();
    if(remainingPeaks.size() > 1)
      fResults["pileUp"] = KResult("pileUp",1);
      //rec->SetExtra(1,12);
    else
      fResults["pileUp"] = KResult("pileUp",0);
      //rec->SetExtra(0,12);
    
    double mean = 0;
    if(maxPeakPos != -1)
      mean = GetMean((int)maxPeakPos + fTrapAmplitude.GetRiseTime(), (int)maxPeakPos + fTrapAmplitude.GetRiseTime() + 
        fTrapAmplitude.GetFlatTopWidth()/2, fTrapAmplitude.GetOutputPulse(), fTrapAmplitude.GetOutputPulseSize(), -1);
    fResults["amp"] = KResult("amp", mean, "ADU");
    fResults["peakPosition"] = KResult("peakPosition", maxPeakPos,"bin");
    fResults["baselineRemoved"] = KResult("baselineRemoved", fBaseRemovalHeat.GetBaselineOffset(), "ADU");
    //rec->SetPeakPosition(maxPeakPos);
    //rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
    //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
          
    return fResults;
  }

  //do ion pulse analysis
  else{
    
    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    
    GetHeatPulseStampWidths(pRec); //this fills a local vector<double>
    for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){
      fPatRemoval.SetPatternLength(1*fHeatPulseStampWidths.at(h)); 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return fResults;}
    }
    
    fPatRemoval.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
    
    for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){  //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      fPatRemoval.SetPatternLength(2*fHeatPulseStampWidths.at(h)); 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return fResults;}
    }
    
        
    fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
    if( !fBaseRemovalIon.RunProcess())
      {cout << "fBaseRemovalIon failed" << endl; return fResults;}

    if(fixPeakPosition == -1){
      // Maximal peak detection
      fMaxIonPeakDetector.SetDecayTimeConstant(fTrapAmplitude.GetDecayTimeConstant());
      fMaxIonPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
      fMaxIonPeakDetector.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
      if( !fMaxIonPeakDetector.RunProcess())
        {cout << "fMaxIonPeakDetector failed" << endl; return fResults;}
      remainingPeaks = fMaxIonPeakDetector.GetRemainingPeaks();
      maxPeakPos = (remainingPeaks.size() == 1) ? remainingPeaks[0][0] : -1;
    }
    else maxPeakPos = fixPeakPosition;
      
    fTrapAmplitude.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
    if( !fTrapAmplitude.RunProcess())
      {cout << "fTrapAmplitude failed" << endl; return fResults;}
                
    //Pile-up detection
    fMultipleIonPeakDetector.SetDecayTimeConstant(fTrapAmplitude.GetDecayTimeConstant());
    fMultipleIonPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
    fMultipleIonPeakDetector.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
    if( !fMultipleIonPeakDetector.RunProcess())
      {cout << "fMultipleIonPeakDetector failed" << endl; return fResults;}
    remainingPeaks = fMultipleIonPeakDetector.GetRemainingPeaks();
    if(remainingPeaks.size() > 1)
      fResults["pileUp"] = KResult("pileUp",1);
      //rec->SetExtra(1,12);
    else
      fResults["pileUp"] = KResult("pileUp",0);
      //rec->SetExtra(0,12);
    
    double mean = 0;
    if(maxPeakPos != -1)
      mean = GetMean((int)maxPeakPos + fTrapAmplitude.GetRiseTime(), (int)maxPeakPos + fTrapAmplitude.GetRiseTime() + 
        fTrapAmplitude.GetFlatTopWidth()/2, fTrapAmplitude.GetOutputPulse(), fTrapAmplitude.GetOutputPulseSize(), 
        KPulsePolarityCalculator::GetExpectedPolarity(pRec) );

    fResults["amp"] = KResult("amp", mean, "ADU");
    fResults["peakPosition"] = KResult("peakPosition", maxPeakPos,"bin");
    fResults["baselineRemoved"] = KResult("baselineRemoved", fBaseRemovalIon.GetBaselineOffset(), "ADU");
  
    
    return fResults;
   }

}

void KTrapKamperProto1::FillTrapAmplitudeParameters(const char* channelName, bool isHeatPulse)
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
  
  fTrapAmplitude.SetParams(decayConst, (unsigned int)riseTime, (unsigned int)width);
}

double KTrapKamperProto1::GetTrapAmplitudeDecayConstant(const char* channelName) const
{
  if (fTrapAmplitudeDecayConstants.find(channelName) != fTrapAmplitudeDecayConstants.end()){
    return fTrapAmplitudeDecayConstants.find(channelName)->second;
  }
  else return -1;
} 


void KTrapKamperProto1::FillPeakPositionResult(KOrderFilter& fOrderFilter, KTrapezoidalFilter* trap, int polarity)
{
  //polarity is the expected direction of the pulse. -1 for negative-going pulse and +1 for positive going.
  double *secondOrderPulse = fOrderFilter.GetOutputPulse();
  unsigned int secondOrderPulseSize = fOrderFilter.GetOutputPulseSize();
  unsigned int riseTime = trap->GetRiseTime();
  unsigned int flatTopTime = trap->GetFlatTopWidth();
  
  double amp = polarity * fPeakPositionSearchAmplifier * fRms.GetStdDev(secondOrderPulse, 0, (unsigned int)(secondOrderPulseSize/3.));
  
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

unsigned int KTrapKamperProto1::FindPeak(vector<double>& pulse, unsigned int maxPosition)
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

double KTrapKamperProto1::GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double mean = 0;
  unsigned int i = first;

  for( ; i < last+1 && i < pulseLength; i++)
    mean += pulse[i];

  return (i > first) ? mean/(i-first) : -1*polarity*99999;
}

double KTrapKamperProto1::GetMax(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double max = 0;
  unsigned int i = first;
  
  for( ; i < last+1 && i < pulseLength; i++){
    if(polarity*pulse[i] > max) max = polarity*pulse[i];
  }

  return (i > first) ? polarity*max : -1*polarity*99999;
}



KTrapezoidalFilter* KTrapKamperProto1::AddTrapIonTime(double decay, unsigned int rise, unsigned int flat)
{
  return AddTrapTime(fTrapIonTime, decay, rise, flat);
  
} 

KTrapezoidalFilter* KTrapKamperProto1::AddTrapHeatTime(double decay, unsigned int rise, unsigned int flat)
{
  return AddTrapTime(fTrapHeatTime, decay, rise, flat);

} 

KTrapezoidalFilter* KTrapKamperProto1::AddTrapTime(vector<KTrapezoidalFilter *>& trapVec, double decay, unsigned int rise, unsigned int flat)
{
  KTrapezoidalFilter* n = new KTrapezoidalFilter();
  n->SetParams(decay, rise, flat);
  trapVec.push_back(n);
  return n;
}

unsigned int KTrapKamperProto1::RunHeatPulseStartTime(void)
{
  return RunPulseStartTime(fTrapHeatTime, fOrderFilter1Heat, fOrderFilter2Heat, fBaseRemovalHeat, -1);
}

unsigned int KTrapKamperProto1::RunIonPulseStartTime(int polarity)
{
  return RunPulseStartTime(fTrapIonTime, fOrderFilter1Ion, fOrderFilter2Ion, fBaseRemovalIon, polarity);   
}

unsigned int KTrapKamperProto1::RunPulseStartTime(vector<KTrapezoidalFilter *>& trapVec, KOrderFilter& ord1, KOrderFilter& ord2, KPtaProcessor& fromProcessor, int polarity)
{
  
  vector<KTrapezoidalFilter* >::iterator it;
  fPeakPositionResult.clear();
  fPeakPositionResult.resize(fromProcessor.GetOutputPulseSize());
  unsigned int maxPeakSize = fPeakPositionResult.size();
  
  for( it = trapVec.begin(); it < trapVec.end(); it++){
    (*it)->SetInputPulse(fromProcessor.GetOutputPulse(), fromProcessor.GetOutputPulseSize());
    if( !(*it)->RunProcess() ){
      cerr << "KTrapKamperProto1::RunPulseStartTime. A KTrapezoidal Filter Failed (decay, rise, flat): " 
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

std::vector<int>& KTrapKamperProto1::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
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


// 
// unsigned int KTrapKamperProto1::PileUpDetection(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec)
// {
//   int polarity = KPulsePolarityCalculator::GetExpectedPolarity(pRec);
//   
//   unsigned int mPeakBin  = FindPeak(fPeakPositionResult, fPeakPositionResult.size());
//   if (mPeakBin > 0){
//     for (unsigned int i = 10; i < fPeakPositionResult.size() && (i < mPeakBin - 10 || i > mPeakBin + 10) ; i++)
//       
//   }
// }

