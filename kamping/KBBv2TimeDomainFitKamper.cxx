//
// KBBv2TimeDomainFitKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//
//   The following map<string, KResult> is returned by MakeKamp
// myResults["amp"] = KResult("amp", fPulseFit->GetParameter(1), "ADU");
// myResults["peakPositon"] = KResult("peakPosition", fPulseFit->GetParameter(3), "bin");
// myResults["chiSq"] = KResult("chiSq", fPulseFit->GetChisquare(), "");
// myResults["baseline"] = KResult("baseline", fPulseFit->GetParameter(0), "ADU");
// myResults["sharpness"] = KResult("sharpness", fPulseFit->GetParameter(2), "1/ADU");
// myResults["fitStatus"] = KResult("fitstatus", fitStatus);
//
//    The following is returned by MakeBaseKamp
// if(raw->GetIsHeatPulse())
//     myResults["pulseType"] = KResult("heat", 0);
// else
//     myResults["pulseType"] = KResult("ionization", 0);

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

std::map<std::string, KResult> KBBv2TimeDomainFitKamper::MakeKamp(KRawBoloPulseRecord * raw)
{
  if(raw->GetIsHeatPulse())
    return false;
    
  map<string, KResult> myResults;

  //rec->SetUnit(0);
  
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
  
  myResults["amp"] = KResult("amp", fPulseFit->GetParameter(1), "ADU");
  //rec->SetAmp(fPulseFit->GetParameter(1));
  myResults["peakPositon"] = KResult("peakPosition", fPulseFit->GetParameter(3), "bin");
  //rec->SetPeakPosition(fPulseFit->GetParameter(3));
  //rec->SetSlopeRemoved(fLineRemove.GetSlope());
  myResults["chiSq"] = KResult("chiSq", fPulseFit->GetChisquare(), "");
  //rec->SetChiSq(fPulseFit->GetChisquare());
  
  myResults["baseline"] = KResult("baseline", fPulseFit->GetParameter(0), "ADU");
  myResults["sharpness"] = KResult("sharpness", fPulseFit->GetParameter(2), "1/ADU");
  myResults["fitStatus"] = KResult("fitstatus", fitStatus);

  //rec->SetExtra(fPulseFit->GetParameter(0), 0);//the baseline value... essentially.
  //rec->SetExtra(fPulseFit->GetParameter(2), 1);//the parameter that defines the sharpness of the step
  //rec->SetExtra(fitStatus, 2);//status returned by the ROOT fitter
  
  return myResults;
}

std::map<std::string, myResults> KBBv2TimeDomainFitKamper::MakeBaseKamp(KRawBoloPulseRecord * raw, KPulseAnalysisRecord * /*rec*/)
{

  map<string, double> myResults;
  if(raw->GetIsHeatPulse())
    myResults["pulseType"] = KResult("heat", 0);
  else
    myResults["pulseType"] = KResult("ionization", 0);

  return myResults;
}
