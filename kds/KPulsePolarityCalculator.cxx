//_____________________________________________
//
// KPulsePolarityCalculator.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@kit.edu> on 11/19/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#include "KPulsePolarityCalculator.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include <iostream>

KPulsePolarityCalculator::KPulsePolarityCalculator(void)
{

}

KPulsePolarityCalculator::~KPulsePolarityCalculator(void)
{
	
}

int KPulsePolarityCalculator::GetExpectedPolarity(KRawBoloPulseRecord *p, KRawBolometerRecord* bolo)
{
  
  
  //heat pulses are always expected to be negative-going???
  if (p->GetIsHeatPulse())
    return -1;
    
  if(bolo == 0)
    bolo = p->GetBolometerRecord();
    
  if(bolo){
    //first, figure out which type of detector you have
    TString detName = bolo->GetDetectorName();

    if(detName.BeginsWith("FID")){
      return KPulsePolarityCalculator::GetExpectedPolarityFromFID(bolo, p);
    }
    else if(detName.BeginsWith("ID")){
      return KPulsePolarityCalculator::GetExpectedPolarityFromID(bolo, p);
    } 
    else {
      std::cerr << "KPulsePolarityCalculator doesn't yet support this type of detector: " << detName.Data() << std::endl;
    }
  }
  // else {
  //     
  //   }
  
  //guess pulse polarity is opposite of the voltage... yes KRawBoloPulseRecord:: "Polarity" is a very poor name choice.
  //suggestion - change this to "Bias"
  if (p->GetPolarity()  > 0)
    return -1;
  else if (p->GetPolarity()  < 0)
    return 1;
  else
    return 0; //!!! should i spit out some error?
  
}

int KPulsePolarityCalculator::GetExpectedPolarityFromFID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p)
{
  //

  //not set up to do anything special
  return KPulsePolarityCalculator::GetPolarityStandardCalculator(bolo,p);
  
  //in the past, there were once 6 electrodes per FID detector, so we have to allow for the ABCDGH configuration
  //otherwise, the new standard FID configuration (for 800 g detectors) is ABCD
  
}

int KPulsePolarityCalculator::GetExpectedPolarityFromID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p)
{
  //

  //not set up to do anything special
  return KPulsePolarityCalculator::GetPolarityStandardCalculator(bolo,p);  
}

int KPulsePolarityCalculator::GetPolarityStandardCalculator(KRawBolometerRecord *bolo, KRawBoloPulseRecord *p)
{
  
  if(p->GetIsHeatPulse())
    return -1;
  
  double aveVolts = 0;
  double numChans = 0;
  for (int i = 0; i < bolo->GetNumPulseRecords(); i++){
    KRawBoloPulseRecord *pp = bolo->GetPulseRecord(i);
    if( !pp->GetIsHeatPulse()){
      aveVolts += pp->GetPolarity();
      numChans++;
    }
  }  
  if (numChans > 0)
    aveVolts = aveVolts/numChans;
    
  if (p->GetPolarity() - aveVolts > 0)
    return -1;
  else if (p->GetPolarity() - aveVolts < 0)
    return 1;
  else
    return 0; //!!! should i spit out some error?
    
}
