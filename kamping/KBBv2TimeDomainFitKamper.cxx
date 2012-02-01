//
// KBBv2TimeDomainFitKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

//

#include "KBBv2TimeDomainFitKamper.h"
#include "TF1.h"
#include "math.h"
#include <iostream>

using namespace std;

KBBv2TimeDomainFitKamper::KBBv2TimeDomainFitKamper(void):
fIrrFour( -3.58973389,  4.85127588, -2.92405266, 0.66301048, 3.12389769e-05, 
1.24955908e-04, 1.87433862e-04, 1.24955908e-04, 3.12389769e-05)
{
  SetName("KBBv2TimeDomainFitKamper");
  fLineRemove.SetBaselineStop(0.20);
  fPositiveFit = new TF1("fpositive", "[0]  + [1]/(1 + exp(-2*[2]*(x - [3])))", 0, 8192);
  fNegativeFit = new TF1("fnegative", "[0]  - [1]/(1 + exp(-2*[2]*(x - [3])))", 0, 8192);
  fData.Set(8192);
}

KBBv2TimeDomainFitKamper::~KBBv2TimeDomainFitKamper(void)
{

}

Bool_t KBBv2TimeDomainFitKamper::MakeKamp(KRawBoloPulseRecord * raw, KPulseAnalysisRecord *rec)
{
  if(raw->GetIsHeatPulse())
    return false;
    
  rec->SetIsBaseline(false); 
  rec->SetUnit(0);
  
  if(raw->GetPulseLength() == 0)
    return false;
  
  TF1 *f1 = 0;
  if(fPolCalc.GetExpectedPolarity(raw) < 0)
    f1 = fNegativeFit;
  else f1 = fPositiveFit;
  
  //reset the parameters
  f1->SetParameter(0, 0);
  f1->SetParameter(1, 1);
  f1->SetParameter(2, 0.149);
  //f1->FixParameter(2, 0.149);
  //f1->SetParLimits(2, 0, 0.3);
  f1->SetParameter(3, 4100);
  f1->SetParLimits(3, 0, 8192);
  
  if(raw->GetPulseLength() != (unsigned int)fData.GetN())
    fData.Set(raw->GetPulseLength());
  
  fLineRemove.SetInputPulse( (vector<short> &)raw->GetTrace());
  if(!fLineRemove.RunProcess()){
    cerr << "KLinearRemoval failed" << endl; return false;
  }
  
  fIrrFour.SetInputPulse(fLineRemove.GetOutputPulse(), fLineRemove.GetOutputPulseSize());
  if(!fIrrFour.RunProcess()){
    cerr << "KIIRFourthOrder failed" << endl; return false;
  }
  
  for(int i = 0; i < fIrrFour.GetOutputPulseSize(); i++)
    fData.SetPoint(i, i, fIrrFour.GetOutputPulse()[i]);
    
  Int_t fitStatus = fData.Fit(f1, "QRW");
  
  rec->SetAmp(f1->GetParameter(1));
  rec->SetPeakPosition(f1->GetParameter(3));
  rec->SetSlopeRemoved(fLineRemove.GetSlope());
  rec->SetChiSq(f1->GetChisquare());
  
  rec->SetExtra(f1->GetParameter(0), 0);//the baseline value... essentially.
  rec->SetExtra(f1->GetParameter(2), 1);//the parameter that defines the sharpness of the step
  rec->SetExtra(fitStatus, 2);//status returned by the ROOT fitter
  
  return true;
}

Bool_t KBBv2TimeDomainFitKamper::MakeBaseKamp(KRawBoloPulseRecord * raw, KPulseAnalysisRecord *rec)
{
  if(raw->GetIsHeatPulse())
    return false;
    
  rec->SetIsBaseline(true); 
  rec->SetUnit(0);
  
  if(raw->GetPulseLength() == 0)
    return false;
  
  fLineRemove.SetInputPulse( (vector<short> &)raw->GetTrace());
  if(!fLineRemove.RunProcess()){
    cerr << "KLinearRemoval failed" << endl; return false;
  }
  
  fIrrFour.SetInputPulse(fLineRemove.GetOutputPulse(), fLineRemove.GetOutputPulseSize());
  if(!fIrrFour.RunProcess()){
    cerr << "KIIRFourthOrder failed" << endl; return false;
  }
  
  double mean = 0;
  double rms = 0;
  int points = 0;
  for(int i = 0; i < fIrrFour.GetOutputPulseSize()*0.20; i++){
    mean += fIrrFour.GetOutputPulse()[i];
    rms += fIrrFour.GetOutputPulse()[i]*fIrrFour.GetOutputPulse()[i];
    points++;
  }
  if(points) mean = mean/(double)points;
  if(points) rms = sqrt(rms/(double)points);
    
  rec->SetAmp(mean);
  rec->SetPeakPosition(0);
  rec->SetSlopeRemoved(fLineRemove.GetSlope());
  rec->SetExtra(rms, 0);

  return true;
}
