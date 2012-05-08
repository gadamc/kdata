//
// KBBv2TimeDomainFitKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KBBv2TimeDomainFitKamper.h"
#include "TF1.h"
#include "math.h"
#include <iostream>

using namespace std;

KBBv2TimeDomainFitKamper::KBBv2TimeDomainFitKamper(void)
{
  SetName("KBBv2TimeDomainFitKamper");
  fPulseFit = new TF1("fpositive", "[0]  + [1]/(1 + exp(-2*[2]*(x - [3])))", 0, 8192);
  fData.Set(8192); //preset the length
}

KBBv2TimeDomainFitKamper::~KBBv2TimeDomainFitKamper(void)
{
  //destroy stuff
  if(fPulseFit) delete fPulseFit;
}

Bool_t KBBv2TimeDomainFitKamper::MakeKamp(KRawBoloPulseRecord * raw, KPulseAnalysisRecord *rec)
{
  if(raw->GetIsHeatPulse())
    return false;
    
  rec->SetIsBaseline(false); 
  rec->SetUnit(0);
  
  if(raw->GetPulseLength() == 0)
    return false;
  
  
  if(fPolCalc.GetExpectedPolarity(raw) < 0){
    fPulseFit->SetParameter(1, -1);
    fPulseFit->SetParLimits(1,-pow(2,15), 0);
  }
  else {
    fPulseFit->SetParameter(1, 1);
    fPulseFit->SetParLimits(1, 0, pow(2,15));
  }
  //reset the parameters
  fPulseFit->SetParameter(0, 0);
  fPulseFit->SetParameter(2, 0.149);
  //fPulseFit->FixParameter(2, 0.149);
  //fPulseFit->SetParLimits(2, 0, 0.3);
  fPulseFit->SetParameter(3, 4100);
  fPulseFit->SetParLimits(3, 0, 8192);
  
  if(raw->GetPulseLength() != (unsigned int)fData.GetN())
    fData.Set(raw->GetPulseLength());  //check for change in size of data, preset size of TGraph as necessary
  
  if(fPulsePreProcessor){
    
    fPulsePreProcessor->SetInputPulse( (vector<short> &)raw->GetTrace());
    if(!fPulsePreProcessor->RunProcess()){
      cerr << "KBBv2TimeDomainFitKamper. Pulse preprocessor failed" << endl; return false;
    }
    
    for(unsigned int i = 0; i < fPulsePreProcessor->GetOutputPulseSize(); i++)
      fData.SetPoint(i, i, fPulsePreProcessor->GetOutputPulse()[i]);
      
  }
  else {
    //vector<short> thePulse = raw->GetTrace();
    for(unsigned int i = 0; i < raw->GetTrace().size(); i++)
      fData.SetPoint(i, i, raw->GetTrace()[i]);
  }
    
  Int_t fitStatus = fData.Fit(fPulseFit, "QRW");
  
  rec->SetAmp(fPulseFit->GetParameter(1));
  rec->SetPeakPosition(fPulseFit->GetParameter(3));
  //rec->SetSlopeRemoved(fLineRemove.GetSlope());
  rec->SetChiSq(fPulseFit->GetChisquare());
  
  rec->SetExtra(fPulseFit->GetParameter(0), 0);//the baseline value... essentially.
  rec->SetExtra(fPulseFit->GetParameter(2), 1);//the parameter that defines the sharpness of the step
  rec->SetExtra(fitStatus, 2);//status returned by the ROOT fitter
  
  return true;
}

Bool_t KBBv2TimeDomainFitKamper::MakeBaseKamp(KRawBoloPulseRecord * raw, KPulseAnalysisRecord * /*rec*/)
{
  if(raw->GetIsHeatPulse())
    return false;
    
  // rec->SetIsBaseline(true); 
  //   rec->SetUnit(0);
  //   
  //   if(raw->GetPulseLength() == 0)
  //     return false;
  //   
  //   if(fPulsePreProcessor){
  //     
  //     fPulsePreProcessor->SetInputPulse( (vector<short> &)raw->GetTrace());
  //     if(!fPulsePreProcessor->RunProcess()){
  //       cerr << "KBBv2TimeDomainFitKamper. Pulse preprocessor failed" << endl; return false;
  //     }
  //     
  //     for(int i = 0; i < fPulsePreProcessor->GetOutputPulseSize(); i++)
  //       fData.SetPoint(i, i, fPulsePreProcessor->GetOutputPulse()[i]);
  //       
  //   }
  //   else {
  //     //vector<short> thePulse = raw->GetTrace();
  //     for(unsigned int i = 0; i < raw->GetTrace().size(); i++)
  //       fData.SetPoint(i, i, raw->GetTrace()[i]);
  //   }
  //   
  //   //okay - how should this be done? 
  //   double mean = 0;
  //   double rms = 0;
  //   int points = 0;
  //   for(int i = 0; i < fData.GetN()*0.20; i++){
  //     mean += fData.GetY(i);
  //     rms += fData.GetY(i)*fData.GetY(i);
  //     points++;
  //   }
  //   if(points) mean = mean/(double)points;
  //   if(points) rms = sqrt(rms/(double)points);
  //     
  //   rec->SetAmp(mean);
  //   rec->SetPeakPosition(0);
  //   //+rec->SetSlopeRemoved(fLineRemove.GetSlope());
  //   rec->SetExtra(rms, 0);
  
  return true;
}
