/**
 @file    FitPulse.cc
 @author  Eric Armengaud, armengau@in2p3.fr
 @brief   Implementation of the FitPulse class. See the .h file
 */

#include "FitPulse.h"
#include <climits>

ClassImp(FitPulse); /**< Root dictionnary */

FitPulse::FitPulse() : Pulse() {
  fBasicPreprocessed = 0;
}

FitPulse::~FitPulse() {
  this->ClearTrace();
}

FitPulse::FitPulse(const EdwPulse* aPulse, Bool_t aBasicPreprocess) : Pulse() {
  this->SetChannel(aPulse->Channel()) ;
  this->SetSampling_ns(aPulse->Sampling_ns()) ;
  this->SetTrace(aPulse->Trace()) ;
  fProcessedTrace = CopyVect(fTrace);
  
  if (this->IsHeat()) {
    fSign = HEAT_SIGN;
    fPatternLength = HEAT_PATTERN_LENGTH;
  } else {
    fSign = ION_SIGN;
    fPatternLength = ION_PATTERN_LENGTH;
  }
  fBaseStart = 0;
  fBaseStop = (Int_t)(PRETRIGGER_FRACTION*(Float_t)fProcessedTrace.size());
  fBasicPreprocessed = 0;
  fIsExpoBaseline = 0;
  if (aBasicPreprocess) this->BasicPreprocess();
}

void FitPulse::PlotTrace(string aPlotName) {
  fProcessedTrace.Plot(aPlotName,0,"Processed Trace");
}

void FitPulse::PlotPattern(string aPlotName) {
  plotvector(fPattern,aPlotName,0,"Pattern");
}

void FitPulse::PlotPeaks(string aPlotName) {
  TH1F* lHist = vect2hist(fProcessedTrace.Vect(),"Processed Trace");
  TCanvas* c = new TCanvas("c","canvas");
  lHist->SetStats(0);
  lHist->Draw(); c->Update();
  Double_t lYmin = c->GetUymin();
  Double_t lYmax = c->GetUymax();
  for (UInt_t i=0; i<fPeakBins.size(); i++) {
    TLine* lLine = new TLine(fPeakBins[i],lYmin,fPeakBins[i],lYmax);
    lLine->SetLineWidth(3); lLine->SetLineColor(2);
    lLine->Draw("SAME");
    // Warning: small memory leak here (but can't do another way)!!
  }
  c->SaveAs(aPlotName.c_str());
  delete c;
  delete lHist;
}

Float_t FitPulse::ComputeNoise() {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  UInt_t lNb = fBaseStop-fBaseStart+1;
  Float_t lNoise = 0;
  for (UInt_t i=0; i<lNb; i++) lNoise+=(pow(fProcessedTrace[i],2)); // NO MORE ROOT binning
  lNoise = TMath::Sqrt(lNoise/lNb); // THIS definition!
  
  return lNoise;
}

void FitPulse::RemoveSaturatedSpikes(Int_t maxlength) {
  // This step should take place before any other preprocessing (eg. pattern, baseline etc..)
  // Replace saturated points by a linear interpolation of their nearest neighboors.
  // Do this only if the saturation takes place on less than maxlength points.
  // This is a "spike removal" procedure, NOT a processing of saturation due to a real signal.
  
  // First get the list of points where saturation takes place
  vector<UInt_t> lSatPoints;
  for (UInt_t i=0; i<fTrace.size(); i++) {
    if (fTrace[i] >= SHRT_MAX || fTrace[i] <= SHRT_MIN) lSatPoints.push_back(i);
  }
  
  // Correction if needed:
  if (lSatPoints.size() != 0) {
    for (UInt_t i=0; i<lSatPoints.size(); i++) {
      Int_t i_pt=(Int_t)(lSatPoints[i]);
      // Find the nearest unsaturated neighboors
      Int_t i_inf=i_pt;
      while (fTrace[i_inf] >= SHRT_MAX || fTrace[i_inf] <= SHRT_MIN) {
        i_inf--;
        if (i_inf<0) break;
      }
      Int_t i_sup=i_pt;
      while (fTrace[i_sup] >= SHRT_MAX || fTrace[i_sup] <= SHRT_MIN) {
        i_sup++;
        if (i_sup >= (Int_t)fTrace.size()) break;
      }
      // Apply the linear interpolation if the "spike length" is not too long
      if ( (i_sup-i_inf-1) <= maxlength ) {
        if (i_inf < 0) {
          fProcessedTrace[i]=fProcessedTrace[i_sup];
        } else if (i_sup >= (Int_t)fTrace.size()) { 
          fProcessedTrace[i]=fProcessedTrace[i_inf];
        } else {
          fProcessedTrace[i]=fProcessedTrace[i_inf]+
          (i-i_inf)*(fProcessedTrace[i_sup]-fProcessedTrace[i_inf])/(Float_t)(i_sup-i_inf);
        }
      }
    }
  }
  
}

void FitPulse::RemoveBaseline() {
  fMeanBase = fProcessedTrace.Integral(fBaseStart,fBaseStop)/(Float_t)(fBaseStop-fBaseStart+1) ;
  for (UInt_t i=0; i<fProcessedTrace.size(); i++) // NO MORE ROOT binning convention 
    fProcessedTrace[i]-=fMeanBase;
}

void FitPulse::RemoveLinearBase() {
  // subtract y=ax+b
  UInt_t n = fProcessedTrace.size();
  Float_t b = fProcessedTrace[0];
  Float_t a = (fProcessedTrace[n-1]-fProcessedTrace[0])/(Float_t)(n-1);
  for (UInt_t k=0; k<n; k++) fProcessedTrace[k] -= (a*k+b);
  this->RemoveBaseline(); // necessary to remove the baseline once again after
}

Bool_t FitPulse::RemoveExpoBaseline(Bool_t aRemove, Float_t aAmplMin) {
  
  if (this->IsHeat()) {
    vector<Float_t> lCutTrace;
    for (Int_t i=fBaseStart; i<=fBaseStop; i++) lCutTrace.push_back(fProcessedTrace[i]); // NO MORE Root histo convention
    TH1F* lCutHist = vect2hist(lCutTrace);
    // Fit a fraction of the trace
    TF1* lFunc = new TF1("myfit","[0]*exp(-[1]*x)+[2]",0,fProcessedTrace.size());
    lFunc->SetParameter(2,lCutTrace[lCutTrace.size()-1]); // crucial initialization
    lFunc->SetParameter(0,lCutTrace[0]-lFunc->GetParameter(2));
    lFunc->SetParameter(1,1/lCutTrace.size());
    lFunc->SetParLimits(2,-(Float_t)SHRT_MAX,(Float_t)SHRT_MAX);
    lFunc->SetParLimits(0,-(Float_t)SHRT_MAX,0);
    lFunc->SetParLimits(1,EXPOBASE_TAUMIN,EXPOBASE_TAUMAX); // decreasing pulse (HEAT only!!!), sharp tau cut to catch real evts
    lCutHist->Fit("myfit","QWN");
    if (lFunc->GetParameter(0) > aAmplMin || lFunc->GetParameter(1) > EXPOBASE_TAUMAX*0.95 || lFunc->GetParameter(1) < EXPOBASE_TAUMIN*1.05) {
      fIsExpoBaseline = 0;
      if (aRemove) this->RemoveBaseline() ;
    } else {
      fIsExpoBaseline = 1;
      if (aRemove) {
        for (UInt_t i=0; i<fProcessedTrace.size(); i++) // NO MORE ROOT binning convention 
          fProcessedTrace[i]-=(lFunc->Eval(i-1)); // GROSSE QUESTIUON: i ou i-1?????????
      }
      
    }
    delete lFunc;
    delete lCutHist;
  }
  
  return fIsExpoBaseline;
}

void FitPulse::RemovePattern(Int_t aNbPatternPts) {
  // Allows to remove a noise with perfectly known period and which remains with a constant
  // phase all over the event (better than single filtering)
  // Especially useful to correct for the heat modulation noise in the ionisation channels
  
  if (aNbPatternPts != 0) {
    // Construct the pattern
    fPattern.clear();
    fPattern.resize(aNbPatternPts,0);
    Int_t lNbLoops = (Int_t)TMath::Floor((fBaseStop-fBaseStart+1)/(Float_t)aNbPatternPts) ;
    for (Int_t i=0;i<lNbLoops;i++){
      Int_t lIter = aNbPatternPts*i;
      for (Int_t j=0;j<aNbPatternPts;j++) fPattern[j] += (fProcessedTrace[j+lIter]) ;
      // NO MORE factor (+1) = ROOT binning convention
    }
    for (Int_t j=0;j<aNbPatternPts;j++) fPattern[j]/=lNbLoops;
    
    // Substract the pattern
    Int_t lShift, j;
    for (UInt_t i=0; i<fProcessedTrace.size(); i++) {
      lShift = (Int_t)TMath::Floor((i-fBaseStart)/(Float_t)aNbPatternPts); 
      // Need a good drawing!
      // lShift = index in the trace of the nearest beginning of the pattern from point i.
      j = i-(fBaseStart+lShift*aNbPatternPts);
      fProcessedTrace[i] -= fPattern[j]; // NO MORE Root binning
    }
  }
}

void FitPulse::SetBaseFromPeakBin(Int_t aBin) {
  if (aBin != 0) {
    fBaseStart = 0;
    if (this->IsHeat()) fBaseStop = (Int_t)(PRETRIGGER_SECURITY*(aBin + FIT_TIMEMIN_HEAT)) ; // sign
    else fBaseStop = (Int_t)(PRETRIGGER_SECURITY*(aBin + FIT_TIMEMIN_ION)) ;
  }
}

void FitPulse::ApplyWindow(Int_t aWidth) {
  if (aWidth == 0) {
    if (this->IsHeat()) aWidth = PLATEAU_WINDOW_HEAT;
    else aWidth = (Int_t)fProcessedTrace.size() - 2*RISE_WINDOW_ION;
  }
  FloatVect lWindowFunction = CopyVect(WindowFunction(fProcessedTrace.size(),aWidth));
  fProcessedTrace.Multiply(lWindowFunction);
}

void FitPulse::SetSign(Short_t aSign) {
  if (aSign != -1 && aSign != 1 && aSign != 0) cerr << "FitPulse::SetSign : wrong sign parameter" << endl;
  fSign = aSign;
}

void FitPulse::BasicPreprocess(Bool_t aCheckPeak) {
  this->RemoveSaturatedSpikes();
  this->RemoveBaseline();
  this->RemovePattern(fPatternLength);
  
  if (fSign == -1) {
    fSimpleAmpl = this->GetMin();
    fSimpleAmplBin = (Float_t)fProcessedTrace.MinBin();
  } else if (fSign == 1) {
    fSimpleAmpl = this->GetMax(); // here maybe need to iterate if fPulseBin < fBaseStop...
    fSimpleAmplBin = (Float_t)fProcessedTrace.MaxBin();
  } else {
    Float_t lMin = this->GetMin();
    fSimpleAmpl = this->GetMax();
    if (-lMin >= fSimpleAmpl) {
      fSimpleAmpl = lMin;
      fSimpleAmplBin = (Float_t)fProcessedTrace.MinBin();
    } else fSimpleAmplBin = (Float_t)fProcessedTrace.MaxBin();
  }
  fBasicPreprocessed = 1;
  
  // if a pulse takes place in the beginning of the trace it might bias the pattern
  if (aCheckPeak) {
    this->FindPeaks(FP_WEAK); // discutable encore..
    if (fPeakBins.size()) {
      Int_t lMinPos = vectmin(fPeakBins);
      if (lMinPos <= fBaseStop && lMinPos > fPatternLength) { // An early pulse has biased the pattern!!
                                                              // (note that if the pulse is really really very early I can't do anything...)
        fBaseStop = lMinPos - 1 ;
        if (fBaseStop <= fBaseStart+9) fBaseStop = fBaseStart+10;
        this->BasicPreprocess(0); // no peak check this time!!! (no infinite loop)
      }
    }
  }
  
}

void FitPulse::ComputeTraceFFT(Bool_t aWindowFlag) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  if (aWindowFlag) {
    UInt_t n=fProcessedTrace.size();
    Int_t lWidth = PLATEAU_WINDOW_HEAT;
    if (!this->IsHeat()) lWidth = (Int_t)n - 2*RISE_WINDOW_ION;
    vector<Float_t> lWindow=WindowFunction(n,lWidth);
    vector<Float_t> lWTrace(n);
    for (UInt_t k=0; k<n; k++) lWTrace[k]=lWindow[k]*fProcessedTrace[k];
    fProcessedTraceFFT = EdwRealFFT(lWTrace);
  } else fProcessedTraceFFT = EdwRealFFT(fProcessedTrace);
}

Float_t FitPulse::GetSimpleAmpl() {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  return fSimpleAmpl;
}

void FitPulse::TranslateTrace(Int_t aShiftBins) {
  // pulse(t) --> pulse(t-t0)
  // That is, if shift < 0 the pulse is advanced; if shift > 0 the pulse is delayed
  FloatVect lTmpTrace(fProcessedTrace.Vect());
  //  TH1F* lTmpTrace = new TH1F(*fProcessedTrace);
  Int_t n = lTmpTrace.size();
  if (aShiftBins < 0) { // NO MORE Root binning convention
    for (Int_t i=0; i<n+aShiftBins; i++)
      fProcessedTrace[i] = lTmpTrace[i-aShiftBins];
    for (Int_t i=n+aShiftBins; i<n; i++)
      fProcessedTrace[i] = fProcessedTrace[n+aShiftBins-1];
  } else {
    for (Int_t i=0; i<aShiftBins; i++)
      fProcessedTrace[i] = lTmpTrace[0];
    for (Int_t i=aShiftBins; i<n; i++)
      fProcessedTrace[i] = lTmpTrace[i-aShiftBins];
  }
  //  delete lTmpTrace;
}

void FitPulse::FindPeaks(Int_t aCriterium, Bool_t AnySign, Int_t aLength) {
  if (aCriterium == FP_WEAK && this->IsHeat())
    this->FindPeaks(FP_SENS_HEAT_WEAK,aLength,FP_ORDER_HEAT_WEAK,AnySign);
  else if (aCriterium == FP_STRICT && this->IsHeat())
    this->FindPeaks(FP_SENS_HEAT_STRICT,aLength,FP_ORDER_HEAT_STRICT,AnySign);
  else if (aCriterium == FP_WEAK && (!this->IsHeat()))
    this->FindPeaks(FP_SENS_ION_WEAK,aLength,FP_ORDER_ION_WEAK,AnySign);
  else if (aCriterium == FP_STRICT && (!this->IsHeat()))
    this->FindPeaks(FP_SENS_ION_STRICT,aLength,FP_ORDER_ION_STRICT,AnySign);
  else if (aCriterium == FP_XWEAK && this->IsHeat())
    this->FindPeaks(FP_SENS_HEAT_XWEAK,aLength,FP_ORDER_HEAT_XWEAK,AnySign);
  else if (aCriterium == FP_XWEAK && (!this->IsHeat()))
    this->FindPeaks(FP_SENS_ION_XWEAK,aLength,FP_ORDER_ION_XWEAK,AnySign);
  else cerr << "FindPeaks : incorrect criterium." << endl; 
}

void FitPulse::FindPeaks(Float_t aSensitivity, Int_t aLength, Int_t aOrder, Bool_t AnySign) {
  fPeakBins.clear();
  if (aSensitivity <= 0 || aLength <= 0 || aOrder <= 0) 
    cerr << "FindPeaks: negative input psrameter!" << endl;
  if (!fBasicPreprocessed) this->BasicPreprocess();
  
  vector<Float_t> lDerivTrace;
  for (UInt_t i=aOrder; i<fProcessedTrace.size(); i++) 
    lDerivTrace.push_back(fProcessedTrace[i]-fProcessedTrace[i-aOrder]); // NO MORE Root binning convention
  Int_t lNbPts=lDerivTrace.size();
  Double_t lThreshold = aSensitivity*vectrms(lDerivTrace);
  for (Int_t i=1; i<lNbPts; i++) {
    if (( (AnySign || fSign >= 0) && lDerivTrace[i] >= lThreshold) ||
        ( (AnySign || fSign <= 0) &&  lDerivTrace[i] <= -lThreshold)) {
      Int_t j_prev = 0 ;
      if ( fPeakBins.size()!= 0  ) j_prev=fPeakBins[fPeakBins.size()-1];
      if (j_prev == 0 || i > j_prev+aLength) fPeakBins.push_back(i);
    }
  }
  
}

Bool_t FitPulse::DetectSinglePulse() {
  Bool_t SinglePeak = 0;
  this->FindPeaks(FP_WEAK);
  if (fPeakBins.size() == 1) {
    this->FindPeaks(FP_STRICT);
    if (fPeakBins.size() == 1) SinglePeak = 1;
  }
  return SinglePeak;
}

FloatVect FitPulse::SmoothedTrace(UInt_t aSmoothing) const {
  FloatVect lOutput = fProcessedTrace;
  UInt_t N = lOutput.size();
  for (UInt_t bin=0; bin<N; bin++) {
    lOutput[bin]=0;
    if (bin >= aSmoothing && bin+aSmoothing < N) { // (car on va tomber hors trace)
      for (Int_t k=(Int_t)bin-(Int_t)aSmoothing; k<=(Int_t)bin+(Int_t)aSmoothing; k++) lOutput[bin]+=fProcessedTrace[k];
      lOutput[bin] /= ((Float_t)(1+2*aSmoothing));
    }
  }
  return lOutput;
}

vector<Float_t> FitPulse::SmoothedAmpl(const FloatVect& aSmoothTrace) const {
  Float_t ampl, amplbin;
  if (fSign == -1) {
    ampl = aSmoothTrace.Min();
    amplbin = (Float_t)aSmoothTrace.MinBin();
  } else if (fSign == 1) {
    ampl = aSmoothTrace.Max(); // here maybe need to iterate if fPulseBin < fBaseStop...
    amplbin = (Float_t)aSmoothTrace.MaxBin();
  } else {
    Float_t lMin = aSmoothTrace.Min();
    ampl = aSmoothTrace.Max();
    if (-lMin >= ampl) {
      ampl = lMin;
      amplbin = (Float_t)aSmoothTrace.MinBin();
    } else amplbin = (Float_t)aSmoothTrace.MaxBin();
  }
  vector<Float_t> out(2,0); out[0]=ampl; out[1]=amplbin;
  return out;
}

Float_t FitPulse::GetRiseTime(const Float_t aFracLow, const Float_t aFracHigh, UInt_t aSmooth, string aPlot) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  FloatVect lSmoothTrace = this->SmoothedTrace(aSmooth);
  vector<Float_t> lSmoothAmpl = this->SmoothedAmpl(lSmoothTrace);
  
  Float_t lTStart = 1;
  //  Float_t lTEnd = fSimpleAmplBin;
  Float_t lTEnd = lSmoothAmpl[1];
  if (lTEnd == 1) return 0;
  //  Float_t lAmplStart = aFracLow*fSimpleAmpl;
  //  Float_t lAmplEnd = aFracHigh*fSimpleAmpl;
  Float_t lAmplStart = aFracLow*lSmoothAmpl[0];
  Float_t lAmplEnd = aFracHigh*lSmoothAmpl[0];
  
  // Iteration to find lTStart and lTEnd
  Int_t i = (Int_t)lTEnd;
  //  Float_t lPtInf = fProcessedTrace[i];
  Float_t lPtInf = lSmoothTrace[i];
  Float_t lPtSup;
  do {
    i-=1;
    lPtSup = lPtInf;
    //    lPtInf = fProcessedTrace[i];
    lPtInf = lSmoothTrace[i];
    if (fSign < 0) { // if fsign=0, by convention will look rising pulse...
      if (lPtSup <= lAmplEnd && lPtInf >= lAmplEnd)
        lTEnd = i+(lAmplEnd-lPtInf)/(lPtSup-lPtInf);
      if (lPtSup <= lAmplStart && lPtInf >= lAmplStart)
        lTStart = i+(lAmplStart-lPtInf)/(lPtSup-lPtInf);
      
    } else {
      if (lPtSup >= lAmplEnd && lPtInf <= lAmplEnd)
        lTEnd = i+(lAmplEnd-lPtInf)/(lPtSup-lPtInf);
      if (lPtSup >= lAmplStart && lPtInf <= lAmplStart)
        lTStart = i+(lAmplStart-lPtInf)/(lPtSup-lPtInf);
    }
  } while (lTStart == 1 && i!=0);
  
  if (aPlot != "") {
    // fit pulse and estimated rising from
    TH1F* lTrace = vect2hist(lSmoothTrace.Vect(),"Trace");
    TCanvas* c = new TCanvas("c","canvas");
    lTrace->SetStats(0);
    lTrace->Draw();
    TLine* lLine = new TLine(lTStart,lAmplStart,lTEnd,lAmplEnd);
    lLine->SetLineWidth(3);
    lLine->Draw("SAME");
    c->SaveAs(aPlot.c_str());
    delete c;
    delete lLine;
    delete lTrace;
  }
  
  return lTEnd-lTStart;
}

Float_t FitPulse::GetFallTime(const Float_t aFracLow, const Float_t aFracHigh, UInt_t aSmooth, string aPlot) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  FloatVect lSmoothTrace = this->SmoothedTrace(aSmooth);
  vector<Float_t> lSmoothAmpl = this->SmoothedAmpl(lSmoothTrace);
  
  Float_t lTStart = lSmoothAmpl[1];
  Float_t lLast = fProcessedTrace.size()-1; // NO MORE N (root binning convention)
  Float_t lTEnd = lLast;
  if (lTStart == lLast) return 0;
  Float_t lAmplStart = aFracHigh*lSmoothAmpl[0];
  Float_t lAmplEnd = aFracLow*lSmoothAmpl[0];
  
  // Iteration to find lTStart and lTEnd
  Int_t i = (Int_t)lTStart;
  //  Float_t lPtInf = fProcessedTrace[i];
  Float_t lPtInf = lSmoothTrace[i];
  Float_t lPtSup;
  do {
    lPtSup = lPtInf;
    //    lPtSup = fProcessedTrace[i+1];
    lPtInf = lSmoothTrace[i+1];
    if (fSign < 0) {
      if (lPtSup >= lAmplEnd && lPtInf <= lAmplEnd)
        lTEnd = i+(lAmplEnd-lPtInf)/(lPtSup-lPtInf);
      if (lPtSup >= lAmplStart && lPtInf <= lAmplStart)
        lTStart = i+(lAmplStart-lPtInf)/(lPtSup-lPtInf);
      
    } else {
      if (lPtSup <= lAmplEnd && lPtInf >= lAmplEnd)
        lTEnd = i+(lAmplEnd-lPtInf)/(lPtSup-lPtInf);
      if (lPtSup <= lAmplStart && lPtInf >= lAmplStart)
        lTStart = i+(lAmplStart-lPtInf)/(lPtSup-lPtInf);
    }
    i+=1;
  } while (lTEnd == lLast && i!= lLast);
  
  if (aPlot != "") {
    // fit pulse and estimated rising from
    TH1F* lTrace = vect2hist(lSmoothTrace.Vect(),"Trace");
    TCanvas* c = new TCanvas("c","canvas");
    lTrace->SetStats(0);
    lTrace->Draw();
    TLine* lLine = new TLine(lTStart,lAmplStart,lTEnd,lAmplEnd);
    lLine->SetLineWidth(3);
    lLine->Draw("SAME");
    c->SaveAs(aPlot.c_str());
    delete c;
    delete lLine; delete lTrace;
  }
  
  return lTEnd-lTStart;
}

Float_t FitPulse::GetFWHM(const Float_t aFracWidth, UInt_t aSmooth, string aPlot) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  FloatVect lSmoothTrace = this->SmoothedTrace(aSmooth);
  vector<Float_t> lSmoothAmpl = this->SmoothedAmpl(lSmoothTrace);
  Int_t N = lSmoothTrace.size();
  
  Float_t t1 = 0;
  Float_t t2 = (Float_t)N-1;
  Float_t lAmplCut = aFracWidth*lSmoothAmpl[0];
  
  // Iteration to find t1
  Int_t i = (Int_t)lSmoothAmpl[1];
  Float_t lPtInf = lSmoothTrace[i];
  Float_t lPtSup;
  do { 
    i-=1;
    lPtSup = lPtInf;
    //    lPtInf = fProcessedTrace[i];
    lPtInf = lSmoothTrace[i];
    if (fSign < 0) { // if fsign=0, by convention will look rising pulse...
      if (lPtSup <= lAmplCut && lPtInf >= lAmplCut)
        t1 = i+(lAmplCut-lPtInf)/(lPtSup-lPtInf);
    } else {
      if (lPtSup >= lAmplCut && lPtInf <= lAmplCut)
        t1 = i+(lAmplCut-lPtInf)/(lPtSup-lPtInf);
    }
  } while (t1 == 0 && i!=0);
  
  // Iteration to find t2
  i = (Int_t)lSmoothAmpl[1];
  lPtInf = lSmoothTrace[i];
  do {
    lPtSup = lPtInf;
    lPtInf = lSmoothTrace[i+1];
    if (fSign < 0) {
      if (lPtSup <= lAmplCut && lPtInf >= lAmplCut)
        t2 = i+(lAmplCut-lPtInf)/(lPtSup-lPtInf);
    } else {
      if (lPtSup >= lAmplCut && lPtInf <= lAmplCut)
        t2 = i+(lAmplCut-lPtInf)/(lPtSup-lPtInf);
    }
    i+=1;
  } while ((Int_t)t2 == N-1 && i!= N-1);
  
  if (aPlot != "") {
    // fit pulse and estimated rising from
    TH1F* lTrace = vect2hist(lSmoothTrace.Vect(),"Trace");
    TCanvas* c = new TCanvas("c","canvas");
    lTrace->SetStats(0);
    lTrace->Draw();
    TLine* lLine = new TLine(t1,lAmplCut,t2,lAmplCut);
    lLine->SetLineWidth(3);
    lLine->Draw("SAME");
    c->SaveAs(aPlot.c_str());
    delete c;
    delete lLine;
    delete lTrace;
  }
  
  return t2-t1;
}

void FitPulse::FilterTrace(vector<Float_t> aDirect, vector<Float_t> aInverse) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  vector<Float_t> lIn = fProcessedTrace.Vect();
  vector<Float_t> lOut = EdwFilter(lIn,aDirect,aInverse);
  fProcessedTrace.Set(lOut);
}


/* ------------------------------------------
 ---- FitPulse : pulse fitting routines ------
 ------------------------------------------- */


vector<Float_t> FitPulse::GetFitAmpl(vector<Float_t> aTemplate, Bool_t aFitTime, Bool_t aInterpolTime, Float_t aTimeOffsetMin, Float_t aTimeOffsetMax,string aPlotFit) {
  if (!fBasicPreprocessed) this->BasicPreprocess();
  UInt_t lSize = aTemplate.size();
  // Ici c'est stupide on recode une fct de l'objet template car on peut pas l'utiliser facilement dans cette classe.
  Int_t lTmpltBin = 0; Float_t lTmpltAbsMax =0 ; Short_t lTmpltSign = 0;
  for (UInt_t i=0; i<lSize; i++) {
    if (TMath::Abs(aTemplate[i]) >= lTmpltAbsMax) {
      lTmpltBin = i; 
      lTmpltAbsMax = TMath::Abs(aTemplate[i]);
      if (aTemplate[i] >= 0) lTmpltSign = 1;
      else lTmpltSign = -1;
    }
  }
  
  PulseFitFunction* lFitFunc = new PulseFitFunction(aTemplate,aFitTime,aInterpolTime);
  Int_t NbParams = 1;
  if (aFitTime) NbParams = 2;
  TF1* Function = new TF1("function",lFitFunc,0,lSize-1,NbParams,"PulseFitFunction");
  // for fSign = 0 we search a priori a positive amplitude.. But the allowed amplitude range will go to negative values.
  Function->SetParameter(0,this->GetSimpleAmpl()/(lTmpltSign*lTmpltAbsMax));
  Float_t lAmplRange = TMath::Abs(Function->GetParameter(0)*FIT_AMPL_RANGE); // rajout de abs.
  Function->SetParLimits(0,-lAmplRange,lAmplRange);
  if (aFitTime) {
    if ( aTimeOffsetMin == 0 ) {
      if (this->IsHeat()) aTimeOffsetMin = FIT_TIMEMIN_HEAT;
      else aTimeOffsetMin = FIT_TIMEMIN_ION;
    }
    if ( aTimeOffsetMax == 0 ) {
      if (this->IsHeat()) aTimeOffsetMax = FIT_TIMEMAX_HEAT;
      else aTimeOffsetMax = FIT_TIMEMAX_ION;
    }
    Function->SetParameter(1,this->GetSimpleAmplBin()-lTmpltBin);
    if (Function->GetParameter(1) <= aTimeOffsetMin) 
      Function->SetParameter(1,aTimeOffsetMin+1);
    if (Function->GetParameter(1) >= aTimeOffsetMax) 
      Function->SetParameter(1,aTimeOffsetMax-1);
    Function->SetParLimits(1,aTimeOffsetMin,aTimeOffsetMax);
  }
  
  // THIS IS *THE* CPU_CONSUMER HERE - TO OPTIMIZE AS MUCH AS POSSIBLE
  TH1F* lTrace = vect2hist(fProcessedTrace.Vect(),"Processed trace");
  lTrace->Fit("function","QWN","",1,lSize-2); // (no drawing)
                                              //  lTrace->Fit("function","QWNM","",1,lSize-2); // (no drawing)
  vector<Float_t> params(4,0);
  params[0]=Function->GetParameter(0);
  if (aFitTime) params[1]=Function->GetParameter(1)+lTmpltBin; // not template-dependant!
  params[2]=Function->GetChisquare()/Function->GetNumberFitPoints();
  params[3]=Function->GetParError(0);
  
  if (aPlotFit != "") {
    TCanvas* c = new TCanvas("c","Template Time Fit");
    lTrace->Draw();
    Function->Draw("same");
    c->SaveAs(aPlotFit.c_str());
    delete c;
  }
  
  delete lFitFunc;
  delete Function;
  delete lTrace;
  return params;
}

PulseFitFunction::PulseFitFunction(vector<Float_t> aTemplate, Bool_t aFitTime, Bool_t aInterpolTime) {
  fTemplate = aTemplate ;
  fTemplateSize = (Int_t)fTemplate.size();
  fFitTime = aFitTime ;
  fInterpolTime = aInterpolTime ;
}

Float_t PulseFitFunction::operator() (Double_t* x, Double_t* p) {
  // This routine MUST be absolutely CPU-optimized!!!
  Float_t pt = x[0];
  if (fFitTime) pt -= p[1]; // fit p0*template(x-p1) in this case
  Int_t i0 = (Int_t)TMath::Floor(pt) ;
  Int_t j0 = i0+1;
  if ( i0 < 0 || j0 >= fTemplateSize ) {
    TF1::RejectPoint();
    return 0;
  }
  Float_t template_interpol = fTemplate[i0];
  // Switching the following line off reduces the cpu by 1/3
  // (but disables a sub-bin timing accuracy)
  if (fInterpolTime) template_interpol += (fTemplate[j0]-template_interpol)*(pt-i0);
  return p[0]*template_interpol;
}

/* ***************************
 optimal filtering routines
 *************************** */

vector<Float_t> FitPulse::GetWienerAmpl(const FloatVect& aTemplateFFT, const FloatVect& aNoise, Bool_t aDoFFT, Bool_t aChi2Switch, string aPlotFilter, string aPlotFit) {
  // Optimal filtering in Fourier space
  // aTemplateFFT = fft of the template in "semi-complex" form
  // aNoise = power spectrum of the noise
  
  vector<Float_t> lOutput(4,0);
  if (aNoise.size() == 0) {
    cerr << "GetWienerAmpl: noise not defined" << endl;
    return lOutput;
  }
  
  FloatVect lTemplate = aTemplateFFT; // new TH1F(*aTemplateFFT);
  
  // Time translation in Fourier space for the template has been removed here (done in getnonintegerfft...)
  if (aDoFFT) {
    if (!fBasicPreprocessed) this->BasicPreprocess();
    this->ComputeTraceFFT();
  }
  
  bool zeronoise=0;
  if (aNoise.Integral() == 0) zeronoise=1;
  //  if (zeronoise) cout <<"Zeronoise!" <<endl;
  
  FloatVect lDenum = lTemplate;
  lDenum.Multiply(lTemplate);
  if (!zeronoise) lDenum.Divide(aNoise);
  FloatVect lNumer = lTemplate;
  lNumer.Multiply(fProcessedTraceFFT);
  if (!zeronoise) lNumer.Divide(aNoise);
  
  Float_t dede = lDenum.Integral();
  if (dede == 0) dede=1;
  Float_t DenumInt = 1/dede;
  Float_t lAmpl = DenumInt*lNumer.Integral();
  lOutput[0]=lAmpl;
  lOutput[3]=sqrt(DenumInt);
  if (aChi2Switch) {
    if (fProcessedTraceFFT.size() == 0) this->ComputeTraceFFT();
    FloatVect lChi2Histo(lTemplate);
    lChi2Histo.Scale(lAmpl);
    lChi2Histo.Add(fProcessedTraceFFT,-1);
    lChi2Histo.Multiply(lChi2Histo);
    if (!zeronoise) lChi2Histo.Divide(aNoise);
    lOutput[2]=lChi2Histo.Integral()/fProcessedTraceFFT.size();
  }
  
  if (aPlotFilter != "" ) {
    FloatVect lFilter = lTemplate;
    if (!zeronoise) lFilter.Divide(aNoise);
    lFilter.Scale(DenumInt);
    lFilter.Multiply(lFilter);
    // Compute the gain
    vector<Float_t> lGain;
    Int_t N = lFilter.size();
    lGain.push_back(lFilter[0]);
    for (Int_t i=1; i<N/2; i++)
      lGain.push_back(lFilter[i]+lFilter[N-i]); // NO MORE Root binning convention + BUT STILL fft semi-hermitian convention
    lGain.push_back(lFilter[N/2]);
    for (UInt_t i=0; i<lGain.size(); i++) lGain[i]=sqrt(lGain[i]);
    plotvector(lGain,aPlotFilter,1); // log scale
  }
  
  if (aPlotFit != "") {
    TH1F* lTrace = vect2hist(fProcessedTrace.Vect());
    TCanvas* c = new TCanvas("c","Wiener Fit");
    lTrace->Draw();
    PulseFitFunction* lFitFunc = new PulseFitFunction(EdwInvFFT(lTemplate.Vect()),1,1);
    TF1* Function = new TF1("function",lFitFunc,0,aTemplateFFT.size()-1,2,"PulseFitFunction");
    Function->SetParameter(0,lAmpl);
    Function->SetParameter(1,0);
    Function->Draw("same");
    c->SaveAs(aPlotFit.c_str());
    delete c;
    delete lTrace;
  }
  
  return lOutput;
}

vector<Float_t> FitPulse::WienerFast(const FloatVect& aKernel, const Float_t aDenom, Bool_t aDoFFT) {
  if (aDoFFT) {
    if (!fBasicPreprocessed) this->BasicPreprocess();
    this->ComputeTraceFFT();
  }
  // Added condition...:
  if (fProcessedTraceFFT.size() == 0) this->ComputeTraceFFT();
  FloatVect lConvol = fProcessedTraceFFT;
  lConvol.Multiply(aKernel);
  
  vector<Float_t> lOutput(4,0);
  lOutput[0]=lConvol.Integral();
  lOutput[3]=sqrt(aDenom);
  return lOutput;
}

vector<Float_t> FitPulse::GetFitWiener(const OffsetFFT* aTemplateFFTs, const FloatVect& aNoise, Bool_t aFast, Bool_t aSubBin, Float_t aOffsetMin, Float_t aOffsetMax, Float_t aOffsetStep, string aPlotFit, Bool_t aNoFixSign) {
  
  vector<Float_t> params(4,0);
  // Check Noise spectrum
  if (aNoise.size() == 0) {
    cerr << "GetFitWiener: empty input noise spectrum" << endl;
    return params;
  }
  
  if ( aOffsetMin == 0 ) {
    if (this->IsHeat()) aOffsetMin = FIT_TIMEMIN_HEAT;
    else aOffsetMin = FIT_TIMEMIN_ION;
  }
  if ( aOffsetMax == 0 ) {
    if (this->IsHeat()) aOffsetMax = FIT_TIMEMAX_HEAT;
    else aOffsetMax = FIT_TIMEMAX_ION;
  }
  if (aOffsetStep == 0) {
    if (this->IsHeat()) aOffsetStep = FIT_TIMESTEP_HEAT;
    else aOffsetStep = FIT_TIMESTEP_ION;
  }
  
  // First: [re]compute the time offset of the pulse for the "zero-offset" pulse
  // This is used then in the output for the definition of pulse time offset:
  // pulse offset = template offset + relative pulse offset wrt template 
  Int_t lTmpltBin = aTemplateFFTs->PulseBin();
  Short_t lTmpltSign = aTemplateFFTs->PulseSign() ;
  if (fSign == 0 || lTmpltSign == 0) {
    cerr << "Warning in GetWienerFit: fSign = "<<fSign<<" - lTmpltSign = "<<lTmpltSign<<" for channel "<<fChannel<<". Exiting."<<endl;
    exit(-1);
  }
  
  // Additionnal constraint on the scan range : don't go beyond the position of the pulse!
  if (aOffsetMin <= -lTmpltBin) aOffsetMin = -lTmpltBin+1;
  Int_t lTmpltReverseBin = (Int_t)fProcessedTrace.size() - lTmpltBin;
  if (!this->IsHeat()) lTmpltReverseBin -= RISE_WINDOW_ION;
  if (aOffsetMax >= lTmpltReverseBin) aOffsetMax = lTmpltReverseBin-1;
  
  // Case when heat time is completely beyond the ion trace.. un peu de la bidouille
  //  if (!this->IsHeat() && aOffsetMin+SCANRANGE_IONBIN_FROM_HEAT > aOffsetMax) 
  //    aOffsetMin = aOffsetMax - SCANRANGE_IONBIN_FROM_HEAT;
  
  // Case when the search window will create some problems
  // Update Apr 2010 : plus souple... pour pouvoir scanner plus large.
  //  if (aOffsetMin < (Float_t)aTemplateFFTs->MinOffset() || aOffsetMax > (Float_t)aTemplateFFTs->MaxOffset())
  //    return params;
  Float_t lMin = (Float_t)aTemplateFFTs->MinOffset();
  Float_t lMax = (Float_t)aTemplateFFTs->MaxOffset();
  if (aOffsetMin < lMin) aOffsetMin = lMin;
  if (aOffsetMax < lMin) aOffsetMax = lMin;
  if (aOffsetMin > lMax) aOffsetMin = lMax;
  if (aOffsetMax > lMax) aOffsetMax = lMax;
  
  //  if (aOffsetMin < (Float_t)aTemplateFFTs->MinOffset()) aOffsetMin = (Float_t)aTemplateFFTs->MinOffset();
  //  if (aOffsetMax > (Float_t)aTemplateFFTs->MaxOffset()) aOffsetMax = (Float_t)aTemplateFFTs->MaxOffset();
  
  // First loop to localize the maximum. For example 5 bins in ioinzation pulses is ok.
  Short_t lSignChange = fSign*lTmpltSign;
  //  Float_t lAmpl = -100000*(Float_t)lSignChange; // +100000 if tmplt sign != expected pulse sign
  Float_t lAmpl = 0; // nvelle convention...
  Float_t lOffset = aOffsetMin; Float_t lSigmaA=0;
  Bool_t lDoFFT = 1;
  if (aNoFixSign) lSignChange=0;
  vector<Float_t> RoughLoop = this->WienerLoop(aTemplateFFTs,aNoise,aOffsetMin,aOffsetMax,aOffsetStep,lAmpl,aFast,lDoFFT,lSignChange);
  lAmpl=RoughLoop[0]; lOffset=RoughLoop[1]; lSigmaA=RoughLoop[3];
  
  // Second loop to get a 1-bin accuracy
  if (aOffsetStep != 1) {
    Float_t ScanFactor = SCAN_FACTOR;
    Int_t lMin = (Int_t)(lOffset-ScanFactor*aOffsetStep);
    Int_t lMax = (Int_t)(lOffset+ScanFactor*aOffsetStep);
    if (lMin < aOffsetMin) lMin = (Int_t)aOffsetMin ; // blague!
    if (lMax > aOffsetMax) lMax = (Int_t)aOffsetMax ;
    vector<Float_t> FineLoop = this->WienerLoop(aTemplateFFTs,aNoise,lMin,lMax,1,lAmpl,aFast,0,lSignChange);
    lAmpl=FineLoop[0]; lOffset=FineLoop[1]; lSigmaA=FineLoop[3];
  }
  
  // Subbin case
  if (aSubBin && this->IsHeat()) {
    Float_t ScanFactor = SCAN_FACTOR;
    Float_t lMin = lOffset-ScanFactor*aOffsetStep;
    Float_t lMax = lOffset+ScanFactor*aOffsetStep;
    if (lMin < aOffsetMin) lMin = aOffsetMin ; // blague!
    if (lMax > aOffsetMax) lMax = aOffsetMax ;
    vector<Float_t> SubBinLoop = this->WienerLoop(aTemplateFFTs,aNoise,lMin,lMax,SUBBIN_TIMESTEP_HEAT,lAmpl,aFast,0,lSignChange);
    lAmpl=SubBinLoop[0]; lOffset=SubBinLoop[1]; lSigmaA=SubBinLoop[3];
  }
  
  // Compute chi2
  // un check...
  if (fProcessedTraceFFT.size() == 0) this->ComputeTraceFFT();
  vector<Float_t> lTmpltFFT;
  if (aSubBin && this->IsHeat())
    lTmpltFFT = aTemplateFFTs->GetNonIntegerOffsetFFT(lOffset);
  else lTmpltFFT = aTemplateFFTs->GetOffsetFFT((Int_t)lOffset);
  FloatVect lChi2Histo(lTmpltFFT);
  lChi2Histo.Scale(lAmpl);
  lChi2Histo.Add(fProcessedTraceFFT,-1);
  lChi2Histo.Multiply(lChi2Histo);
  lChi2Histo.Divide(aNoise);
  Float_t lChi2 = lChi2Histo.Integral();
  
  // Plot fit
  if (aPlotFit != "") GetWienerAmpl(lTmpltFFT,aNoise,0,0,"",aPlotFit);
  
  params[0]=lAmpl; params[1]=lOffset+lTmpltBin;
  params[2]=lChi2/fProcessedTraceFFT.size(); params[3]=lSigmaA;
  return params;
}

vector<Float_t> FitPulse::WienerLoop(const OffsetFFT* aTemplateFFTs, const FloatVect& aNoise, Float_t aMin, Float_t aMax, Float_t aStep, Float_t aAmpl, Bool_t aFast, Bool_t aDoFFT, Short_t aSignChange) {
  
  
  // nvlle convention: aAmpl=0 signifie on va prendre la 1ere ampl de toutes facons.
  vector<Float_t> output(4,0);
  Float_t lAmpl = aAmpl;
  Bool_t lDoFFT = aDoFFT;
  Float_t lOffset, lSigma;
  
  for (Float_t theOffset=aMin; theOffset<=aMax; theOffset+=aStep) {
    vector<Float_t> params;
    if (aStep >= 0.99) { // No subbin accuracy
      if (!aFast) {
        FloatVect lFFT(aTemplateFFTs->GetOffsetFFT((Int_t)theOffset));
        if (lFFT.size() == 0) return output;
        params = this->GetWienerAmpl(lFFT,aNoise,lDoFFT);
      } else {
        FloatVect lKernel(aTemplateFFTs->GetKernel((Int_t)theOffset));
        if (lKernel.size() == 0) return output;
        params = this->WienerFast(lKernel,aTemplateFFTs->GetDenom((Int_t)theOffset),lDoFFT);
      }
    } else { // Subbin accuracy routine
      FloatVect lFFT(aTemplateFFTs->GetNonIntegerOffsetFFT(theOffset));
      params = this->GetWienerAmpl(lFFT,aNoise,lDoFFT);
    }
    Float_t theAmpl = params[0];
    lDoFFT = 0;
    if ( ((aSignChange == 1) && theAmpl >= lAmpl) || (aSignChange == -1 && theAmpl <= lAmpl) 
        || lAmpl == 0 || (aSignChange == 0 && ( TMath::Abs(theAmpl) >= TMath::Abs(lAmpl))) ) {
      lAmpl = theAmpl;
      lOffset = theOffset;
      lSigma = params[3];
    }
  }
  
  output[0]=lAmpl; output[1]=lOffset; output[3]=lSigma;
  return output;
}
