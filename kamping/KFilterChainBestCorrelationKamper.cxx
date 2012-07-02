//
// KFilterChainBestCorrelationKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Definition of parameters for output KPulseAnalysisRecord
//  fAmp : amplitude estimate from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//  fPeakPosition: estimated pulse peak position from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//  fChiSq: ChiSq returned by ROOT fit routine from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//  
// Extra field definition for output KPulseAnalysisRecord:
// 0 : NDF of the fit -- fit with floating start time
// 1 : ROOT's TMinuit Fit return output - 0=good fit.

// 2 : RMS of the first 40% of the processed pulse
// 3 : RMS of preprocessed pulse
// 4 : RMS of processed pulse

// 5: order of lowpass filter -- butterworth
// 6: corner frequency of lowpass
// 7: order of highpass filter
// 8: corner frequency of highpass

// 9 : Baseline estimation value - amplitude estimate of the fit with a fixed start time set to fBaselinePosition.
// 10 : fBaselinePosition - the fixed position of the pulse peak time in the Baseline amplitude estimate fit..
// 11 : baseline fit result -- ROOT's TMinuit Fit return output for baseline fit

// 12 : Heat channel amplitude estimate from the fit with the peak time fixed to the estimated Ionization pulse time. Heat channel only. This will be empty for ionization channels.
// 13: Fixed pulse time -- the estimated Ionization pulse time used in the fit. This will be empty for ionization channels.
// 14 : fixed pulse time fit result -- ROOT's TMinuit Fit return output for Heat channel fit with the peak time fixed to the Ionization peak time. This will be empty for ionization channels.


#include "KFilterChainBestCorrelationKamper.h"

#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KIIRFilter.h"
#include "KIIRFirstOrder.h"
#include "KIIRSecondOrder.h"
#include "KIIRThirdOrder.h"
#include "KIIRFourthOrder.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KRawBolometerRecord.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
#include "TVirtualFitter.h"

#include <stdexcept>
#include <math.h>


using namespace std;

KFilterChainBestCorrelationKamper::KFilterChainBestCorrelationKamper(void)
{

  SetName("KFilterChainBestCorrelationKamper");
  

  //fHeatPulseStampWidths.reserve(6);
  
  fPreProcessor = 0;
  fPostProcessor = 0;
  fTemplate.resize(0);
  fPosRangeMin = 0;
  fPosRangeMax = 0;
	fBaselinePosition = 50;
}

KFilterChainBestCorrelationKamper::~KFilterChainBestCorrelationKamper(void)
{
  
}


Bool_t KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec, double fixPeakPosition)
{
  //cout << "MakeKamp start"<<endl;
  rec->SetIsBaseline(false); 
  rec->SetName(GetName());
  rec->SetUnit(0);
  if(pRec->GetPulseLength() == 0){
    //cerr << "KFilterChainBestCorrelationKamper: ::MakeKamp. Pulse Length is zero." << endl;
    rec->SetPeakPosition(-1);
    rec->SetAmp(-99999);
    rec->SetExtra(-99999,0);
    return false;
}

  
  if(pRec->GetTrace().size() == 0){
    rec->SetPeakPosition(-1);
    rec->SetAmp(-99999);
    rec->SetExtra(-99999,0);
    return false;
  }
  
  
  if(fTemplate.size() == 0)
    {cerr << "KFilterChainBestCorrelationKamper: fTemplate isn't set!" << endl; return false;}

  fPeakPos = -1.0;
  
  if(!fPreProcessor  || !fPostProcessor) {cerr << "KFilterChainBestCorrelationKamper: pre or post processor pointer not set." << endl; return false;} 

  fPreProcessor->SetInputPulse((std::vector<short> &)pRec->GetTrace());
	if(!fPreProcessor->RunProcess())
		{cerr << "KFilterChainBestCorrelationKamper: fPreProcessor failed" << endl; return false;}
		
  fCorrelation.SetInputPulse(fPreProcessor);

	fPostProcessor->SetInputPulse(fPreProcessor);

  if(!fPostProcessor->RunProcess())
    {cerr << "KFilterChainBestCorrelationKamper: fPostProcessor failed" << endl; return false;}

 
	fCorrelation.SetResponse(fTemplate);
	if( !fCorrelation.RunProcess() ){
		cerr << "KFilterChainBestCorrelationKamper: fCorrelation failed" <<endl; return false;
	}
	
  //cerr << "Range:"<<fPosRangeMin<<","<<fPosRangeMax<<endl;
	fPeakPos = (double) GetPositionOfMaxAbsValue(fCorrelation.GetOutputPulse(),fCorrelation.GetOutputPulseSize(),fPosRangeMin,fPosRangeMax)+fPulseStartTimeInTemplate;
         

	fAmpEstPos = fPeakPos+(fAmpEstimatorTimeInTemplate-fPulseStartTimeInTemplate);

	//Baseline RMS of the first 40% of the processed pulse
	double Baseline_RMS = 0.0;
	unsigned int sample = 0;
	for(;sample < 0.4*fPostProcessor->GetOutputPulseSize();sample++)
		Baseline_RMS += fPostProcessor->GetOutputPulse()[sample]*fPostProcessor->GetOutputPulse()[sample];
	Baseline_RMS /= (double) sample;
	Baseline_RMS = sqrt(Baseline_RMS);
	rec->SetExtra(Baseline_RMS,2);
	//--------------------
	
	//RMS of the preprocessed pulse
	if(fPreProcessor){
		double Preprocessed_RMS = 0.0;
		sample = 0;
		for(;sample < fPreProcessor->GetOutputPulseSize();sample++)
			Preprocessed_RMS += fPreProcessor->GetOutputPulse()[sample]*fPreProcessor->GetOutputPulse()[sample];
		Preprocessed_RMS /= (double) sample;
		Preprocessed_RMS = sqrt(Preprocessed_RMS);
		rec->SetExtra(Preprocessed_RMS,3);
	}
	//-----------------------
	
	//RMS of the processed pulse
	double Processed_RMS = 0.0;
	sample = 0;
	for(;sample < fPostProcessor->GetOutputPulseSize();sample++)
		Processed_RMS += fPostProcessor->GetOutputPulse()[sample]*fPostProcessor->GetOutputPulse()[sample];
	Processed_RMS /= (double) sample;
	Processed_RMS = sqrt(Processed_RMS);
	rec->SetExtra(Processed_RMS,4);
	//-----------------------------
	

	//Fitting
	//TVirtualFitter::SetDefaultFitter("Fumili");
	// Amplitude estimation
	TF1 *fitfunc = new TF1("fitfunc",this, &KFilterChainBestCorrelationKamper::TemplateFitFunction,(double) (fPeakPos - fPulseStartTimeInTemplate - 5),(double) (fPeakPos - fPulseStartTimeInTemplate + fTemplate.size() - 5),2,"KFilterChainBestCorrelationKamper","TemplateFitFunction");
	fitfunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fAmpEstPos)/fMaxAbsValueInTemplate,fPeakPos - fPulseStartTimeInTemplate);
	fitfunc->SetParLimits(1,fPeakPos - fPulseStartTimeInTemplate - 5,fPeakPos - fPulseStartTimeInTemplate + 5);
	
	//fitfunc->FixParameter(1,PeakPos - fPulseStartTimeInTemplate);
	//fitfunc->SetParLimits(0,-1000,0);
	TH1D *hist = new TH1D("hist",pRec->GetChannelName(),fPostProcessor->GetOutputPulseSize(),0,fPostProcessor->GetOutputPulseSize()-1);
	for(unsigned int i = 0; i < fPostProcessor->GetOutputPulseSize();i++){
		hist->SetBinContent(i+1,fPostProcessor->GetOutputPulse()[i]);
		hist->SetBinError(i+1,Baseline_RMS);
	}
	Int_t fitres = hist->Fit("fitfunc","RQNS");

	rec->SetAmp(fitfunc->GetParameter(0));
	rec->SetPeakPosition((double)fitfunc->GetParameter(1)+fPulseStartTimeInTemplate);
	rec->SetChiSq(fitfunc->GetChisquare());
	rec->SetExtra(fitfunc->GetNDF(),0);
	rec->SetExtra(fitres,1);
	//if(fitres != 0)
		//cout << "fit is not valid "<<fitres << endl;
	
	// Baseline value estimation
	fitfunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fBaselinePosition)/fMaxAbsValueInTemplate,fBaselinePosition);
	fitfunc->FixParameter(1,fBaselinePosition);
	fitres = hist->Fit("fitfunc","RQNS");

	rec->SetExtra(fitfunc->GetParameter(0),9);
  rec->SetExtra(fBaselinePosition,10);
  rec->SetExtra(fitres,11);
	//-----------------
	
	// Heat pulse time fixed by ionisation
	if(fixPeakPosition > 0){

		fitfunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fixPeakPosition)/fMaxAbsValueInTemplate,fixPeakPosition);
		fitfunc->FixParameter(1,fixPeakPosition);
		fitres = hist->Fit("fitfunc","RQNS");
		rec->SetExtra(fitfunc->GetParameter(0),12);
		rec->SetExtra(fixPeakPosition,13);
    rec->SetExtra(fitres,14);
	}
  else rec->SetExtra(-1,14);

	delete fitfunc;
	delete hist;
	
	return true;

}



unsigned int KFilterChainBestCorrelationKamper::GetPositionOfMaxAbsValue(double* input, unsigned int size,unsigned int from, unsigned int to)
{
  unsigned int pos = 0;
  double max = 0.0;
  unsigned int i = (from < size)? from : 0;
  unsigned int stop = ((to > 0)&&(to < size)) ? to : size;
  for(; i < stop; i++)
    if( fabs(*(input+i)) > max ){
      pos = i;
      max = fabs(*(input+i));
    }
  return pos;
}

void KFilterChainBestCorrelationKamper::SetTemplate(std::vector<double>& templ, double AmpEstimatorTimeInTemplate, double PulseStartTimeInTemplate, double maxTemplateValue)
{

  fTemplate = templ;
  fAmpEstimatorTimeInTemplate = AmpEstimatorTimeInTemplate;
  fPulseStartTimeInTemplate = PulseStartTimeInTemplate;
  //fMaxAbsValueInTemplate = fabs(templ[GetPositionOfMaxAbsValue(&templ[0],templ.size())]);
  fMaxAbsValueInTemplate = maxTemplateValue;  
  
}
    


Double_t KFilterChainBestCorrelationKamper::TemplateFitFunction(Double_t *x, Double_t *par)
{
  if (((x[0]-par[1])<1) || ((x[0]-par[1])>(fTemplate.size()-2)))
    return 0.0;
  // Sterling formula (does not work for some reason)
  /* int x0 = (int) floor(x[0] - par[1] + 0.5);
  return par[0]*(fTemplate[x0] + (fTemplate[x0+1]+fTemplate[x0-1])*(x[0]-x0)/2.0+(fTemplate[x0+1]-2*fTemplate[x0]+fTemplate[x0-1])*(x[0]-x0)*(x[0]-x0)/4.0); */
  
  
  Double_t xx = x[0] - par[1];
  bool quadr_interp = false;
  unsigned int x_l = (unsigned int) floor(xx);
  unsigned int x_m = 0;
  if( (fabs(fTemplate[x_l])<fabs(fTemplate[x_l+1]))&&(fabs(fTemplate[x_l+1])>fabs(fTemplate[x_l+2])) ){
    quadr_interp = true;
    x_m = x_l+1;
  }
  if( (fabs(fTemplate[x_l])>fabs(fTemplate[x_l-1]))&&(fabs(fTemplate[x_l])>fabs(fTemplate[x_l+1])) ){
    quadr_interp = true;
    x_m = x_l;
  }
  if(quadr_interp){
    // quadratic interpolation around template maximum
    double x0 = (double) x_m-1;
    double y0 = fTemplate[x0];
    double x1 = (double) x_m;
    double y1 = fTemplate[x1];
    double x2 = (double) x_m+1;
    double y2 = fTemplate[x2];
    
    double a = ((y2-y1)-((y0-y1)*(x1-x2))/(x1-x0))/(x2*x2 - x1*x1 - ((x0*x0-x1*x1)*(x1-x2)/(x2-x1)));
    double b = (y0-y1)/(2*a*(x1-x0))-(x0*x0-x1*x1)/(2*(x1-x0));
    double c = y0 - a*x0*x0 + 2*a*b*x0 - a*b*b;
    return (Double_t) par[0]*(a*(xx-b)*(xx-b)+c);
  }
  else{
    // linear interpolation
    unsigned int fl_x = (unsigned int) floor(xx);
    if (fl_x - xx == 0.0)
      return (Double_t) par[0]*fTemplate[(unsigned int) xx];
    return (Double_t) par[0]*(fTemplate[fl_x]*(1-fabs(fl_x - xx)) + fTemplate[fl_x+1]*(1 - fabs(fl_x + 1 - xx)));
  }
}
