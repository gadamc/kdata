//
// KFilterChainBestCorrelationKamper.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Definition of map returned by MakeKamp  (myResults is of type map<string, KResult>)
//
// myResults["amp"]
//        amplitude estimate from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//
// myResults["peakPosition"] 
//        estimated pulse peak position from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//
// myResults["chiSq"]
//        ChiSq returned by ROOT fit routine from time-domain pulse fit with floating peak position (+- 5 bins about point of best correlation)
//
// myResults["ndfFloat"] 
//        NDF of the fit -- fit with floating start time
//
// myResults["fitResult"] 
//        ROOT's TMinuit Fit return output - 0=good fit.
//
// myResults["baselineRmsPostProc"] 
//        RMS of the first 40% of the post-processed pulse
//
// myResults["rmsPreProc"] 
//        RMS of entire preprocessed pulse
//
// myResults["rmsPostProc"]
//        RMS of entire post-processed pulse
//  
// myResults["ampFixHeatPosition"]
//        amplitude estimate of the fit with a fixed start time set to fBaselinePosition.
//
// myResults["fixHeatPosition"]
//        fBaselinePosition - the fixed position of the pulse peak time in the above amplitude estimate fit..
//
// myResults["fitResults_fixHeatPosition"]
//        fit result -- ROOT's TMinuit Fit return output for baseline fit
//
// myResults["ampFixedPositionFromIon"] 
//        Heat channel amplitude estimate from the fit with the peak time fixed to the estimated Ionization pulse time. 
//        Heat channel only. This element of the map will not exist for ionization channels
//
// myResults["fixedPositionFromIon"] 
//        Fixed pulse time -- the estimated Ionization pulse time used in the fit. 
//        This element of the map will not exist for ionization channels
//
// myResults["fitResults_fixedPositionFromIon"]
//        fixed pulse time fit result -- ROOT's TMinuit Fit return output for Heat channel fit with the peak time fixed to the Ionization peak time. 
//
// myResults["baselineRemoved"]
//        the value removed from baseline removal object found in the PreProcessor object (if the preProcessor is a chain)
// myResults["slopeRemoved"] 
//        the linear slope removed from the baseline removal object. If a KBaselineRemoval object was used
//        then this element will not exist -- only if a KLinearRemoval object was found in the PreProcessor




#include "KFilterChainBestCorrelationKamper.h"

#include "KBaselineRemoval.h"
#include "KLinearRemoval.h"
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


std::map<std::string, KResult> KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * pRec, double fixPeakPosition)
{
  //cout << "MakeKamp start"<<endl;
  // rec->SetName(GetName());
  // rec->SetUnit(0);

  map<string, KResult> myResults;
  if(pRec->GetPulseLength() == 0 || pRec->GetTrace().size() == 0){

    myResults["peakPosition"] = KResult("peakPosition", -1, "bin");
    //rec->SetPeakPosition(-1);
    myResults["amp"] = KResult("amp", -99999, "ADU");
    //rec->SetAmp(-99999);
    myResults["ndfFloat"] = KResult("ndfFloat", -99999);
    //rec->SetExtra(-99999,0);
    return myResults;
  }

  
  if(fTemplate.size() == 0)
    {cerr << "KFilterChainBestCorrelationKamper: fTemplate isn't set!" << endl; return myResults;}

  fPeakPos = -1.0;
  
  if(!fPreProcessor  || !fPostProcessor) {cerr << "KFilterChainBestCorrelationKamper: pre or post processor pointer not set." << endl; return myResults;} 

  fPreProcessor->SetInputPulse((std::vector<short> &)pRec->GetTrace());
	if(!fPreProcessor->RunProcess())
		{cerr << "KFilterChainBestCorrelationKamper: fPreProcessor failed" << endl; return myResults;}
	
	fPostProcessor->SetInputPulse(fPreProcessor);

  if(!fPostProcessor->RunProcess())
    {cerr << "KFilterChainBestCorrelationKamper: fPostProcessor failed" << endl; return myResults;}

  fCorrelation.SetInputPulse(fPostProcessor);

	fCorrelation.SetResponse(fTemplate);
	if( !fCorrelation.RunProcess() ){
		cerr << "KFilterChainBestCorrelationKamper: fCorrelation failed" <<endl; return myResults;
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
	//rec->SetExtra(Baseline_RMS,2);
  myResults["baselineRmsPostProc"] = KResult("baselineRmsPostProc", Baseline_RMS, "ADU");
	//--------------------
	
	//RMS of the preprocessed pulse
	if(fPreProcessor){
		double Preprocessed_RMS = 0.0;
		sample = 0;
		for(;sample < fPreProcessor->GetOutputPulseSize();sample++)
			Preprocessed_RMS += fPreProcessor->GetOutputPulse()[sample]*fPreProcessor->GetOutputPulse()[sample];
		Preprocessed_RMS /= (double) sample;
		Preprocessed_RMS = sqrt(Preprocessed_RMS);
		//rec->SetExtra(Preprocessed_RMS,3);
    myResults["rmsPreProc"] = KResult("rmsPreProc", Preprocessed_RMS, "ADU");
	}
	//-----------------------
	
	//RMS of the processed pulse
	double Processed_RMS = 0.0;
	sample = 0;
	for(;sample < fPostProcessor->GetOutputPulseSize();sample++)
		Processed_RMS += fPostProcessor->GetOutputPulse()[sample]*fPostProcessor->GetOutputPulse()[sample];
	Processed_RMS /= (double) sample;
	Processed_RMS = sqrt(Processed_RMS);
	//rec->SetExtra(Processed_RMS,4);
  myResults["rmsPostProc"] = KResult("rmsPostProc", Processed_RMS, "ADU");
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

  myResults["amp"] = KResult("amp", fitfunc->GetParameter(0), "ADU");
  myResults["peakPosition"] = KResult("peakPosition", (double)fitfunc->GetParameter(1) + fPulseStartTimeInTemplate, "bin");
  myResults["chiSq"] = KResult("chiSq", fitfunc->GetChisquare());
  myResults["ndfFloat"] = KResult("ndfFloat", fitfunc->GetNDF());
  myResults["fitResult"] = KResult("fitResult", fitres);

	// rec->SetAmp(fitfunc->GetParameter(0));
	// rec->SetPeakPosition((double)fitfunc->GetParameter(1)+fPulseStartTimeInTemplate);
	// rec->SetChiSq(fitfunc->GetChisquare());
	// rec->SetExtra(fitfunc->GetNDF(),0);
	// rec->SetExtra(fitres,1);
	//if(fitres != 0)
		//cout << "fit is not valid "<<fitres << endl;
	
	// Baseline value estimation
	fitfunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fBaselinePosition)/fMaxAbsValueInTemplate,fBaselinePosition);
	fitfunc->FixParameter(1,fBaselinePosition);
	fitres = hist->Fit("fitfunc","RQNS");

  myResults["ampFixHeatPosition"] = KResult("ampFixHeat", fitfunc->GetParameter(0), "ADU");
  myResults["fixHeatPosition"] = KResult("fixHeatPosition", fBaselinePosition, "bin");
  myResults["fitResults_fixHeatPosition"] = KResult("fitResults_fixHeatPosition", fitres);
	// rec->SetExtra(fitfunc->GetParameter(0),9);
 //  rec->SetExtra(fBaselinePosition,10);
 //  rec->SetExtra(fitres,11);
	//-----------------
	
	// Heat pulse time fixed by ionisation
	if(fixPeakPosition > 0){

		fitfunc->SetParameters(*(fPostProcessor->GetOutputPulse()+(int)fixPeakPosition)/fMaxAbsValueInTemplate,fixPeakPosition);
		fitfunc->FixParameter(1,fixPeakPosition);
		fitres = hist->Fit("fitfunc","RQNS");
    myResults["ampFixedPositionFromIon"] = KResult("ampFixedPositionFromIon", fitfunc->GetParameter(0), "ADU");
    myResults["fixedPositionFromIon"] = KResult("fixedPositionFromIon", fixPeakPosition, "bin");
    myResults["fitResults_fixedPositionFromIon"] = KResult("fitResults_fixedPositionFromIon", fitres);

		//rec->SetExtra(fitfunc->GetParameter(0),12);
		//rec->SetExtra(fixPeakPosition,13);
    //rec->SetExtra(fitres,14);
	}
  else myResults["fitResults_fixedPositionFromIon"] =  KResult("fitResults_fixedPositionFromIon", -1);
    //rec->SetExtra(-1,14);

  for(unsigned int i = 0; i < fPreProcessor->GetNumProcessors(); i++){
    
    try{
      KLinearRemoval& mProc = dynamic_cast<KLinearRemoval &>( *fPreProcessor->GetProcessor(i) );
      myResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetOffset(), "ADU");
      myResults["slopeRemoved"] = KResult("slopeRemoved", mProc.GetSlope(), "ADU/bin");
      //rec->SetBaselineRemoved(mProc.GetOffset());
      //rec->SetSlopeRemoved(mProc.GetSlope());
    }
    catch(std::bad_cast){} //just do nothing.
    
    try{
      KBaselineRemoval& mProc = dynamic_cast<KBaselineRemoval &>( *fPreProcessor->GetProcessor(i) );
      myResults["baselineRemoved"] = KResult("baselineRemoved", mProc.GetBaselineOffset(), "ADU");
      //rec->SetBaselineRemoved(mProc.GetBaselineOffset());
    }
    catch(std::bad_cast){} //do nothing.

  }

  // //calculate the pulse rise time and pulse width.
  // double baselineValue = 0;
  // std::vector<Short_t>& rawPulse = pRec->GetTrace();

  // if(!mProc){
  //   //we didn't find a baseline removal type of processor above, so we have to calculate 
  //   //the baseline amplitude by hand. 
  //   double stop = 0.4*rawPulse.size();
  //   unsigned int start = 0;
  //   unsigned int i = start;
  //   for( ; i < stop; i++){
  //     baselineValue += rawPulse[i];
  //   }
  // baselineValue = baselineValue/double(i-start);
  // }

  //now calculate the area under the pulse template, given 


  
	delete fitfunc;
	delete hist;
	
	return myResults;

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
  //the template should be of positive polarity....
  //
  //

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
