//_____________________________________________
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
  fFitFunc =0;
  fFitBase = 0;
  fFitHist =0;
}

KFilterChainBestCorrelationKamper::~KFilterChainBestCorrelationKamper(void)
{
  if(fFitFunc) delete fFitFunc;
  if(fFitHist) delete fFitHist;
  if(fFitBase) delete fFitBase;
}


std::map<std::string, KResult> KFilterChainBestCorrelationKamper::MakeKamp(KRawBoloPulseRecord * pRec, double fixPeakPosition)
{
  
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


  return MakeKamp( pRec->GetTrace(), fixPeakPosition, pRec->GetChannelName());

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
  if ((x[0]-par[1])<1) 
    return 0.0;
  
  if ((x[0]-par[1])>(fTemplate.size()-2))
    return fTemplate[fTemplate.size()-2]+(x[0]-par[1]-(fTemplate.size()-2))*(fTemplate[fTemplate.size()-1]-fTemplate[fTemplate.size()-2]);
  
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
    double y0 = fTemplate[(int)x0];
    double x1 = (double) x_m;
    double y1 = fTemplate[(int)x1];
    double x2 = (double) x_m+1;
    double y2 = fTemplate[(int)x2];
    
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
