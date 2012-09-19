//_____________________________________________
// KMultiTrapKamperProto.cxx  
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Implementation of using just trapezoidal filters to measure the pulse peak and amplitude.
// This is called the "Prototype" because it does nothing smart with memory management. Thus
// it is not optimized for performance, but rather optimized for fewer bugs! 
//
//  output from MakeKamp
//
//  fExtra map name  |  Description of value
//  ---------------------------------------
//  amp                       estimated pulse amplitude
//  peakPosition              estimated peak position
//  baselineRemoved           average baseline amplitude in the first 40% of pulse (removed before amplitude estimate)
//  trapRiseTime             Trapezoidal Filter for Pulse Amplitude - rise time
//  trapFlatTopWidth         Trapezoidal Filter for Pulse Amplitude - flat top width
//  trapDecayTime            Trapezoidal Filter for Pulse Amplitude - fast decay slope
//  trapDecayTime2           Trapezoidal Filter for Pulse Amplitude - slow decay slope (zero if only one exponential is used)
//  pileUpFound               this is set to 1 if pileups are found, 0 otherwise.
//  numPeaks                  Number of found peaks
//  peak_N                    Positions of found peaks (max N = 8)
//  coef_N                    Corresponding correlation coefficient to the peak position (max N = 8)


#include "KMultiTrapKamperProto.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KPulsePolarityCalculator.h"
#include "KRawBolometerRecord.h"

#include <stdexcept>
#include <sstream>

//#include <math>

#define HEAT_RISE 10.
#define HEAT_FLAT_TOP 100.
#define ION_RISE 50.
#define ION_FLAT_TOP 200.

using namespace std;

KMultiTrapKamperProto::KMultiTrapKamperProto(void)
{

  SetName("KMultiTrapKamperProto");
  
  fDefaultTrapHeatAmplitudeParameters.push_back(20.0); //fast decay constant
  fDefaultTrapHeatAmplitudeParameters.push_back(0.3); //coefficient of the second exponential
  fDefaultTrapHeatAmplitudeParameters.push_back(60.0); //slow decay constant
  fDefaultTrapHeatAmplitudeParameters.push_back(HEAT_RISE); //rise time
  fDefaultTrapHeatAmplitudeParameters.push_back(HEAT_FLAT_TOP); //width
  fTrapAmplitudeHeat1.SetParams(fDefaultTrapHeatAmplitudeParameters[0], 
                              fDefaultTrapHeatAmplitudeParameters[3],
                              fDefaultTrapHeatAmplitudeParameters[4]);
  fTrapAmplitudeHeat1.SetParams(fDefaultTrapHeatAmplitudeParameters[2], 
                              fDefaultTrapHeatAmplitudeParameters[3],
                              fDefaultTrapHeatAmplitudeParameters[4]);

  fDefaultTrapIonAmplitudeParameters.push_back(1000.0); //decay constant
  fDefaultTrapIonAmplitudeParameters.push_back(0.0); // place holder for the second decay constant
  fDefaultTrapIonAmplitudeParameters.push_back(0.0); // place holder for the second exponential's coefficient
  fDefaultTrapIonAmplitudeParameters.push_back(ION_RISE); //rise time
  fDefaultTrapIonAmplitudeParameters.push_back(ION_FLAT_TOP); //width
  fTrapAmplitudeIon.SetParams(fDefaultTrapIonAmplitudeParameters[0], 
                              fDefaultTrapIonAmplitudeParameters[3],
                              fDefaultTrapIonAmplitudeParameters[4]);

  fHeatPulseStampWidths.reserve(6);
  
  fIonPeakDetector.AddFilteringProcess(100, 0, 0.7,  1,  1, false,   true, false);
  fIonPeakDetector.AddFilteringProcess(50, 0, 0.7, 1, 1, false, true, false);
  fIonPeakDetector.AddFilteringProcess(10, 0, 1.0, 1, 0, false, true,  true);
  fIonPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true,  true);
  
  fMultipleHeatPeakDetector.AddFilteringProcess(20, 0, 0.7,  1,  1, true,   true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(10, 0, 0.7, 1, 1, false, true, false);
  fMultipleHeatPeakDetector.AddFilteringProcess(3, 0, 1.0, 1, 0, false, true, true);
}

KMultiTrapKamperProto::~KMultiTrapKamperProto(void)
{
  
}

std::map<std::string, KResult>  KMultiTrapKamperProto::MakeKamp(KRawBoloPulseRecord * rawPulseRecord )
{
  return MakeKamp(rawPulseRecord,  -1);
}


std::map<std::string, KResult>  KMultiTrapKamperProto::MakeBaseKamp(KRawBoloPulseRecord * pRec)
{
  
  if(pRec->GetIsHeatPulse())
    return MakeKamp(pRec, 2*HEAT_RISE+HEAT_FLAT_TOP);
  else
    return MakeKamp(pRec, 2*ION_RISE+ION_FLAT_TOP);

}

std::map<std::string, KResult>  KMultiTrapKamperProto::MakeKamp(KRawBoloPulseRecord * pRec, double fixPeakPosition)
{

  std::map<std::string, KResult> myResults;

  if(pRec->GetPulseLength() == 0){
    //cerr << "KMultiTrapKamperProto::MakeKamp. Pulse Length is zero." << endl;
    return myResults;
  }
    


  double PeakPos = -1.;
  std::vector< std::vector<double> > remainingPeaks;
  
  if(pRec->GetTrace().size() == 0){
      myResults["peakPosition"] = KResult("peakPosition", -1, "bin");
      myResults["amp"] = KResult("amp", -99999, "ADU");
      //rec->SetPeakPosition(-1);
      //rec->SetAmp(-99999);
      return myResults;
    }


  FillTrapAmplitudeParameters(pRec->GetChannelName(), pRec->GetIsHeatPulse());
  //record the settings of the trapezoidal filters in the pulseAnalysisRecord.  This MUST be documented
  //at the beginning of this class.

  
  
  //do heat pulse analysis
  if(pRec->GetIsHeatPulse()){
    myResults["trapDecayTime"] = KResult("trapDecayTime", fTrapAmplitudeHeat1.GetDecayTimeConstant(), "bin");
    myResults["trapDecayTime2"] = KResult("trapDecayTime2", fTrapAmplitudeHeat2.GetDecayTimeConstant(), "bin");
    myResults["trapRiseTime"] = KResult("trapRiseTime", fTrapAmplitudeHeat1.GetRiseTime(), "bin");
    myResults["trapFlatTopWidth"] = KResult("trapFlatTopWidth", fTrapAmplitudeHeat1.GetFlatTopWidth(), "bin");

    //rec->SetExtra(fTrapAmplitudeHeat1.GetDecayTimeConstant(), 0);
    //rec->SetExtra(fTrapAmplitudeHeat2.GetDecayTimeConstant(), 1);
    //rec->SetExtra(fTrapAmplitudeHeat1.GetRiseTime() ,2);
    //rec->SetExtra(fTrapAmplitudeHeat1.GetFlatTopWidth(), 3);
    
    fBaseRemovalHeat.SetInputPulse( (std::vector<short> &)pRec->GetTrace());

    if( !fBaseRemovalHeat.RunProcess())
      {cout << "fBaseRemovalHeat failed" << endl; return myResults;}
    
    if(fixPeakPosition == -1){
      // central peak detection
      fMultipleHeatPeakDetector.SetDecayTimeConstant(fTrapAmplitudeHeat1.GetDecayTimeConstant());
      fMultipleHeatPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
      fMultipleHeatPeakDetector.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if( !fMultipleHeatPeakDetector.RunProcess())
        {cout << "fMultipleHeatPeakDetector failed" << endl; return myResults;}
      remainingPeaks = fMultipleHeatPeakDetector.GetRemainingPeaks();
      double diff = (double) fBaseRemovalHeat.GetOutputPulseSize();
      double center = (double) fBaseRemovalHeat.GetOutputPulseSize()/2;
      for(unsigned int i = 0; i < remainingPeaks.size();i++){
        if(fabs(remainingPeaks[i][0] - center) < diff){
          diff = fabs(remainingPeaks[i][0] - center);
          PeakPos = remainingPeaks[i][0];
        }
      }
      
      //Pile-up detection
      if(remainingPeaks.size() > 1){
        myResults["pileUpFound"] = KResult("pileUpFound", 1);
        //rec->SetExtra(1, 12);
        myResults["numPeaks"] = KResult("numPeaks", (double) remainingPeaks.size());
        //rec->SetExtra((double) remainingPeaks.size(),5);
 
        unsigned int numStoredPileUps = (remainingPeaks.size()<8) ? remainingPeaks.size() : 8;
        for(unsigned int i = 0; i < numStoredPileUps; i=i+2){  
          string name_index = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
          string namePos = "peak_" + name_index;
          string nameCoef = "coef_" + name_index;
          myResults[namePos.c_str()] = KResult(namePos.c_str(), remainingPeaks[i][0], "bin");
          myResults[nameCoef.c_str()] = KResult(nameCoef.c_str(), remainingPeaks[i][1]);

          //rec->SetExtra(remainingPeaks[i][0],i+6);
          //rec->SetExtra(remainingPeaks[i][1],i+7);
        }
      }
      else
        myResults["pileUpFound"] = KResult("pileUpFound", 0);
        //rec->SetExtra(0, 12);
    }      
    else PeakPos = fixPeakPosition;
    
     //Pile-up detection
    if(fixPeakPosition != -1){
      fMultipleHeatPeakDetector.SetDecayTimeConstant(fTrapAmplitudeHeat1.GetDecayTimeConstant());
      fMultipleHeatPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
      fMultipleHeatPeakDetector.SetInputPulse(fBaseRemovalHeat.GetOutputPulse(), fBaseRemovalHeat.GetOutputPulseSize());
      if( !fMultipleHeatPeakDetector.RunProcess())
        {cout << "fMultipleHeatPeakDetector failed" << endl; return myResults;}
      remainingPeaks = fMultipleHeatPeakDetector.GetRemainingPeaks();
      if(remainingPeaks.size() > 1){
        myResults["pileUpFound"] = KResult("pileUpFound", 1);
        //rec->SetExtra(1, 12);
        myResults["numPeaks"] = KResult("numPeaks", (double) remainingPeaks.size());
        //rec->SetExtra((double) remainingPeaks.size(),5);
 
        unsigned int numStoredPileUps = (remainingPeaks.size()<8) ? remainingPeaks.size() : 8;
        for(unsigned int i = 0; i < numStoredPileUps; i=i+2){     
          string name_index = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
          string namePos = "peak_" + name_index;
          string nameCoef = "coef_" + name_index;
          myResults[namePos.c_str()] = KResult(namePos.c_str(), remainingPeaks[i][0], "bin");
          myResults[nameCoef.c_str()] = KResult(nameCoef.c_str(), remainingPeaks[i][1]);
          //rec->SetExtra(remainingPeaks[i][0],i+6);
          //rec->SetExtra(remainingPeaks[i][1],i+7);
        }
      }
      else
        myResults["pileUpFound"] = KResult("pileUpFound", 0);
        //rec->SetExtra(0,12);
    };
    
    if(PeakPos != -1){
      // extracting the two exponentials from the signal, set every bin before peak and after full
      // width of trapezoidal filter to zero
      std::vector<double> FastDecayPulse(fBaseRemovalHeat.GetOutputPulseSize(), 0.);
      std::vector<double> SlowDecayPulse(fBaseRemovalHeat.GetOutputPulseSize(), 0.);
      double* signal = fBaseRemovalHeat.GetOutputPulse();
      try{
        for(int i = (int)PeakPos; (i < (int)(PeakPos+fDefaultTrapHeatAmplitudeParameters[4]+2*fDefaultTrapHeatAmplitudeParameters[3])) && (i < (int)fBaseRemovalHeat.GetOutputPulseSize()); i++){
            FastDecayPulse[i] = signal[i]*exp((double)(-(i-PeakPos)/fHeat1Decay))/(exp((double)(-(i-PeakPos)/fHeat1Decay))+fHeat2Coeff*exp((double)(-(i-PeakPos)/fHeat2Decay)));
            SlowDecayPulse[i] = signal[i]*exp((double)(-(i-PeakPos)/fHeat2Decay))/(exp((double)(-(i-PeakPos)/fHeat2Decay))+exp((double)(-(i-PeakPos)/fHeat1Decay))/fHeat2Coeff);
        }
      }
      catch (out_of_range& oor) {
        // this shouldn't happen
        cerr << "Heat amplitude estimation: out of Range error: " << oor.what() << endl;
        throw oor;
      }
      
      
      
      fTrapAmplitudeHeat1.SetInputPulse(FastDecayPulse);
      if( !fTrapAmplitudeHeat1.RunProcess())
        {cout << "fTrapAmplitudeHeat1 failed" << endl; return myResults;}
      
      fTrapAmplitudeHeat2.SetInputPulse(SlowDecayPulse);
      if( !fTrapAmplitudeHeat2.RunProcess())
        {cout << "fTrapAmplitudeHeat2 failed" << endl; return myResults;}
      
      double mean = GetMean((int)PeakPos + fTrapAmplitudeHeat1.GetRiseTime()+0.2*fTrapAmplitudeHeat1.GetFlatTopWidth(), (int)PeakPos + fTrapAmplitudeHeat1.GetRiseTime() + 
        fTrapAmplitudeHeat1.GetFlatTopWidth(), fTrapAmplitudeHeat1.GetOutputPulse(), fTrapAmplitudeHeat1.GetOutputPulseSize(), -1)/(fHeat1Decay*fTrapAmplitudeHeat1.GetRiseTime()) 
         +
        GetMean((int)PeakPos + fTrapAmplitudeHeat2.GetRiseTime()+0.2*fTrapAmplitudeHeat2.GetFlatTopWidth(), (int)PeakPos + fTrapAmplitudeHeat2.GetRiseTime() + 
        fTrapAmplitudeHeat2.GetFlatTopWidth(), fTrapAmplitudeHeat2.GetOutputPulse(), fTrapAmplitudeHeat2.GetOutputPulseSize(), -1)/(fHeat2Decay*fTrapAmplitudeHeat2.GetRiseTime());

      myResults["amp"] = KResult("amp", mean, "ADU");
      //rec->SetAmp(mean);
    }
    else
      myResults["amp"] = KResult("amp", 0.0, "ADU");


      //rec->SetAmp(0);
    myResults["peakPosition"] = KResult("peakPosition", PeakPos, "bin");
    //rec->SetPeakPosition(PeakPos);
    myResults["baselineRemoved"] = KResult("baselineRemoved", fBaseRemovalHeat.GetBaselineOffset(), "ADU");
    //rec->SetBaselineRemoved(fBaseRemovalHeat.GetBaselineOffset());
    //rec->SetSlopeRemoved(fBaseRemovalHeat.GetSlope());
    
    return myResults;
  }

  //do ion pulse analysis
  else{
    myResults["trapDecayTime"] = KResult("trapDecayTime", fTrapAmplitudeIon.GetDecayTimeConstant(), "bin");
    myResults["trapRiseTime"] = KResult("trapRiseTime", fTrapAmplitudeIon.GetRiseTime(), "bin");
    myResults["trapFlatTopWidth"] = KResult("trapFlatTopWidth", fTrapAmplitudeIon.GetFlatTopWidth(), "bin");

    //rec->SetExtra(fTrapAmplitudeIon.GetRiseTime() ,0);
    //rec->SetExtra(fTrapAmplitudeIon.GetFlatTopWidth(), 1);
    //rec->SetExtra(fTrapAmplitudeIon.GetDecayTimeConstant(), 2);
    
    fPatRemoval.SetInputPulse((std::vector<short> &)pRec->GetTrace());
    
    GetHeatPulseStampWidths(pRec); //this fills a local vector<double>
    for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){
      fPatRemoval.SetPatternLength(1*fHeatPulseStampWidths.at(h)); 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return myResults;}
    }
    
    fPatRemoval.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
    
    for(unsigned int h = 0; h < fHeatPulseStampWidths.size(); h++){  //reports from Eric Armengaud is that its better to remove a pattern that is twice as long.
      fPatRemoval.SetPatternLength(2*fHeatPulseStampWidths.at(h)); 
      if( !fPatRemoval.RunProcess())
        {cout << "fPatRemoval failed" << endl; return myResults;}
    }
    
        
    fBaseRemovalIon.SetInputPulse(fPatRemoval.GetOutputPulse(), fPatRemoval.GetOutputPulseSize());
    if( !fBaseRemovalIon.RunProcess())
      {cout << "fBaseRemovalIon failed" << endl; return myResults;}

    if(fixPeakPosition == -1){
      // maximal correlation peak detection
      fIonPeakDetector.SetDecayTimeConstant(fTrapAmplitudeIon.GetDecayTimeConstant());
      fIonPeakDetector.SetPolarity(KPulsePolarityCalculator::GetExpectedPolarity(pRec));
      fIonPeakDetector.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
      if( !fIonPeakDetector.RunProcess())
        {cout << "fMultipleIonPeakDetector failed" << endl; return myResults;}
      remainingPeaks = fIonPeakDetector.GetRemainingPeaks();
      PeakPos = (remainingPeaks.size() == 1) ? remainingPeaks[0][0] : -1;
      /*
      //Pile-up detection
      if(remainingPeaks.size() > 1){
        rec->SetPileUpDetected(true);
        rec->SetExtra((double) remainingPeaks.size(),5);
        for(unsigned int i = 0; i < remainingPeaks.size(); i++)
          rec->SetExtra(remainingPeaks[i][0],i+6);
      }
      else
        rec->SetPileUpDetected(myResults);
      */
    }
    else PeakPos = fixPeakPosition;
      
    fTrapAmplitudeIon.SetInputPulse(fBaseRemovalIon.GetOutputPulse(), fBaseRemovalIon.GetOutputPulseSize());
    if( !fTrapAmplitudeIon.RunProcess())
      {cout << "fTrapAmplitudeIon failed" << endl; return myResults;}
                   
    double mean = 0; 
    if(PeakPos != -1)
      mean = (GetMean((int)PeakPos + fTrapAmplitudeIon.GetRiseTime()+0.2*fTrapAmplitudeIon.GetFlatTopWidth(), (int)PeakPos + fTrapAmplitudeIon.GetRiseTime() + 
        fTrapAmplitudeIon.GetFlatTopWidth(), fTrapAmplitudeIon.GetOutputPulse(), fTrapAmplitudeIon.GetOutputPulseSize(), 
        KPulsePolarityCalculator::GetExpectedPolarity(pRec) ))/(fTrapAmplitudeIon.GetDecayTimeConstant()*fTrapAmplitudeIon.GetRiseTime());
    
    myResults["amp"] = KResult("amp", mean, "ADU");
    //rec->SetAmp(mean);
    myResults["peakPosition"] = KResult("peakPosition", PeakPos, "bin");
    //rec->SetPeakPosition(PeakPos);
    myResults["baselineRemoved"] = KResult("baselineRemoved", fBaseRemovalIon.GetBaselineOffset(), "ADU");
    //rec->SetBaselineRemoved(fBaseRemovalIon.GetBaselineOffset());
    //rec->SetSlopeRemoved(fBaseRemovalIon.GetSlope());
    
    return myResults;
   }

}

void KMultiTrapKamperProto::FillTrapAmplitudeParameters(const char* channelName, bool isHeatPulse)
{
  double decayConst, riseTime, width;
  std::vector< double > params;
  
  if(isHeatPulse){
    riseTime = fDefaultTrapHeatAmplitudeParameters[3];
    width = fDefaultTrapHeatAmplitudeParameters[4];
    fHeat1Decay = fDefaultTrapHeatAmplitudeParameters[0];
    fHeat2Coeff = fDefaultTrapHeatAmplitudeParameters[1];
    fHeat2Decay = fDefaultTrapHeatAmplitudeParameters[2];
    
    if(GetParams(channelName).size() > 0){
      params = GetParams(channelName);
      fHeat1Decay = params[0];
      fHeat2Coeff = params[1];
      fHeat2Decay = params[2];
    }
    fTrapAmplitudeHeat1.SetParams(fHeat1Decay, riseTime, width);
    fTrapAmplitudeHeat2.SetParams(fHeat2Decay, riseTime, width);
  }
  else{
    riseTime = fDefaultTrapIonAmplitudeParameters[3];
    width = fDefaultTrapIonAmplitudeParameters[4];
    decayConst = fDefaultTrapIonAmplitudeParameters[0];
    
    if(GetParams(channelName).size() > 0){
      params = GetParams(channelName);
      decayConst = params[0];      
    }
    fTrapAmplitudeIon.SetParams(decayConst, riseTime, width);
  }
}

std::vector<double> KMultiTrapKamperProto::GetParams(const char* channelName) const
{
  if (fParams.find(channelName) != fParams.end()){
    return fParams.find(channelName)->second;
  }
  else {
    std::vector<double> empty;
    empty.clear();
    return empty;
  };
} 

double KMultiTrapKamperProto::GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity)
{
  double mean = 0;
  unsigned int i = first;

  for( ; i < last+1 && i < pulseLength; i++)
    mean += pulse[i];

  return (i > first) ? mean/(i-first) : -1*polarity*9999999;
}

std::vector<int>& KMultiTrapKamperProto::GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec)
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

