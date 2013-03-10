//_____________________________________________
// KBBv2TimeDomainFitKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//
// performs a fit in the time domain to the function
//  fPulseFit = new TF1("fpositive", "[0]  + [1]/(1 + exp(-2*[2]*(x - [3])))", 0, 8192);
//  
//  by defaul the fit range is from 0 to 8192. but this can be changed by the user.
// 
//  all results regarding time are in units of bin number. 
//
//  The following map<string, KResult> is returned by MakeKamp
//      fResults["amp"] = KResult("amp", fPulseFit->GetParameter(1), "ADU");
//      fResults["peakPositon"] = KResult("peakPosition", fPulseFit->GetParameter(3), "bin");
//      fResults["chiSq"] = KResult("chiSq", fPulseFit->GetChisquare(), "");
//      fResults["baselineAmp"] = KResult("baseline", fPulseFit->GetParameter(0), "ADU");
//      fResults["sharpness"] = KResult("sharpness", fPulseFit->GetParameter(2), "1/bin");
//      fResults["fitStatus"] = KResult("fitstatus", fitStatus);  //fitstatus returned my ROOT Minuit routine
//      fResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetOffset(), "ADU");
//      fResults["slopeRemoved"] = KResult("slopeRemoved", mProc.GetSlope(), "ADU/bin");
//
//      fResults["pulseType"] = KResult("heat", 1);
//      fResults["pulseType"] = KResult("ionization", 1);

//  The following is JUST returned by MakeBaseKamp
//     fResults["pulseType"] = KResult("heat", 1);
//     fResults["pulseType"] = KResult("ionization", 1);
//


#include "KBBv2TimeDomainFitKamper.h"
#include "TF1.h"
#include "math.h"
#include "KLinearRemoval.h"
#include "KBaselineRemoval.h"
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

std::map<std::string, KResult>& KBBv2TimeDomainFitKamper::MakeKamp(KRawBoloPulseRecord * raw)
{


  if(raw->GetIsHeatPulse())
    fResults["pulseType"] = KResult("heat", 1);
  else
    fResults["pulseType"] = KResult("ionization", 1);

  if(raw->GetIsHeatPulse())
    return fResults;
    

  //rec->SetUnit(0);
  
  if(raw->GetPulseLength() == 0)
    return fResults;
  
  
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
      cerr << "KBBv2TimeDomainFitKamper. Pulse preprocessor failed" << endl; return fResults;
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
  
  fResults["amp"] = KResult("amp", fPulseFit->GetParameter(1), "ADU");
  //rec->SetAmp(fPulseFit->GetParameter(1));
  fResults["peakPositon"] = KResult("peakPosition", fPulseFit->GetParameter(3), "bin");
  //rec->SetPeakPosition(fPulseFit->GetParameter(3));
  //rec->SetSlopeRemoved(fLineRemove.GetSlope());
  fResults["chiSq"] = KResult("chiSq", fPulseFit->GetChisquare(), "");
  //rec->SetChiSq(fPulseFit->GetChisquare());
  
  fResults["baselineAmp"] = KResult("baseline", fPulseFit->GetParameter(0), "ADU");
  fResults["sharpness"] = KResult("sharpness", fPulseFit->GetParameter(2), "1/bin");
  fResults["fitStatus"] = KResult("fitstatus", fitStatus);

  //rec->SetExtra(fPulseFit->GetParameter(0), 0);//the baseline value... essentially.
  //rec->SetExtra(fPulseFit->GetParameter(2), 1);//the parameter that defines the sharpness of the step
  //rec->SetExtra(fitStatus, 2);//status returned by the ROOT fitter

  for(unsigned int i = 0; i < fPulsePreProcessor->GetNumProcessors(); i++){

    try{
      KLinearRemoval& mProc = dynamic_cast<KLinearRemoval &>( *fPulsePreProcessor->GetProcessor(i) );
      fResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetOffset(), "ADU");
      fResults["slopeRemoved"] = KResult("slopeRemoved", mProc.GetSlope(), "ADU/bin");
    }
    catch(std::bad_cast){} //just do nothing.
    
    try{
      KBaselineRemoval& mProc = dynamic_cast<KBaselineRemoval &>( *fPulsePreProcessor->GetProcessor(i) );
      fResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetBaselineOffset(), "ADU");
    }
    catch(std::bad_cast){} //do nothing.
  }
  
  return fResults;
}

std::map<std::string, KResult>& KBBv2TimeDomainFitKamper::MakeBaseKamp(KRawBoloPulseRecord * raw)
{

  if(raw->GetIsHeatPulse())
    fResults["pulseType"] = KResult("heat", 1);
  else
    fResults["pulseType"] = KResult("ionization", 1);

  return fResults;
}
