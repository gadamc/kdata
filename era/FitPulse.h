/**
   @file    FitPulse.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class for numerical operations on pulses
*/

#ifndef _FITPULSE_H_
#define _FITPULSE_H_

#include "EdwUtils.h"
#include "Pulse.h"
#include "EdwPulse.h"
#include "OffsetFFT.h"

#include "TVirtualFFT.h"

#define PRETRIGGER_SECURITY 0.9 /**< Used in computation of the point where baseline stops */
#define PRETRIGGER_FRACTION 0.4 /**< standard pretrigger region */
#define HEAT_PATTERN_LENGTH 0 /**< Heat pattern (none) */
#define ION_PATTERN_LENGTH 200 /**< Nb of points of the pattern on ionization traces */
#define HEAT_SIGN -1 /**< Sign of heat pulses */
#define ION_SIGN 1 /**< Default value of the sign of ionization pulses (must be changed as a function of voltage sign)*/
#define FIT_AMPL_RANGE 2 /**< Amplitude range (wrt template amplitude) allowed in direct pulse fit */
#define EXPOBASE_AMPLMIN -20 /**< Tuned parameter for extrapolation of the trace */
#define EXPOBASE_TAUMIN 0.002 /**< Tuned parameter for extrapolation of the trace */
#define EXPOBASE_TAUMAX 0.04 /**< Tuned parameter for extrapolation of the trace */

#define FP_XWEAK -1 /**< Extra-weak : for very noisy periods */
#define FP_WEAK 0 /**< "garantit" des vrais pulses */
#define FP_STRICT 1 /**< "garantit" des traces sans pulses */
#define FP_SENS_ION_XWEAK 9 /**< Peak finder parameter */
#define FP_ORDER_ION_XWEAK 2 /**< Peak finder parameter */
#define FP_SENS_ION_WEAK 7 /**< Peak finder parameter */
#define FP_ORDER_ION_WEAK 2 /**< Peak finder parameter */
#define FP_SENS_ION_STRICT 5 /**< Peak finder parameter */
#define FP_ORDER_ION_STRICT 3 /**< Peak finder parameter */
#define FP_SENS_HEAT_XWEAK 5.5 /**< Peak finder parameter */
#define FP_ORDER_HEAT_XWEAK 3 /**< Peak finder parameter */
#define FP_SENS_HEAT_WEAK 4.5 /**< Peak finder parameter */
#define FP_ORDER_HEAT_WEAK 3 /**< Peak finder parameter */
#define FP_SENS_HEAT_STRICT 3.5 /**< Peak finder parameter */
#define FP_ORDER_HEAT_STRICT 6 /**< Peak finder parameter */
#define FP_LENGTH 20 /**< Peak finder parameter */

// consistency should be same as in EdwTemplate.. (for wiener fitting) :
#define FIT_TIMEMIN_HEAT -5 /**< Default time window for the heat pulse fit */
#define FIT_TIMEMAX_HEAT 3 /**< Default time window for the heat pulse fit */
#define FIT_TIMEMIN_ION -500 /**< Default time window for the ionization pulse fit */
#define FIT_TIMEMAX_ION 200 /**< Default time window for the ionization pulse fit */
#define FIT_TIMESTEP_HEAT 1 /**< Standard time bin for the heat pulse fit */
#define SUBBIN_TIMESTEP_HEAT 0.2 /**< Subbin accuracy for heat pulse fit (with no help from ionization) */
#define FIT_TIMESTEP_ION 5 /**< Standard time bin for the first ionization pulse fit scan. celui la peut-etre a mieux tuner... */
#define SCAN_FACTOR 4 /**< Parameter to tune the second ionization pulse fit scan */
#define SCANRANGE_IONBIN_FROM_HEAT 200 /**< on fait (centre)+-(range) */
#define SCANRANGE_IONBIN_FROM_ION 2 /**< on fait (centre)+-(range) */

#define PULSESHAPE_SMOOTHING 2 /**< Smoothing parameter for the computation of pulse shape variables */

#define PLATEAU_WINDOW_HEAT 50 /**< default plateau for heat window function */
#define RISE_WINDOW_ION 20 /**< default 'smoothing length' for ion window function */

/**
   @class   FitPulse
   @brief   Class for operations on pulses

   A FitPulse, inherited from the Pulse object, contains both a raw, unprocessed trace (Short_t) and a processed trace (TH1F) on which several functions may be applied: depatterning, peak finding, maximum or integral amplitude estimation, template fitting in time or Fourier space.

*/
class FitPulse : public Pulse {
public:
  FitPulse() ;
  FitPulse(const EdwPulse*, Bool_t aBasicPreprocess=0) ;
  /**< FitPulse constructor from an EdwPulse: the processed trace is just copied from the raw trace contained in the EdwPulse. If aBasicPreprocess is true, the standard preprocessing is applied to the processed trace */
  ~FitPulse() ;
  FloatVect ProcessedTrace() const { return fProcessedTrace; }
  /**< The processed trace */
  FloatVect ProcessedTraceFFT() const { return fProcessedTraceFFT; }
  /**< FFT of the processed trace (ROOT FFT format) */
  Float_t MeanBase() const { return fMeanBase; } /**< Baseline level */
  vector<Float_t> Pattern() const { return fPattern ; }
  /**< Returns the pattern which is computed within the RemovePattern() function */
  vector<Int_t> GetPeakBins() const { return fPeakBins; }
  /**< Returns the positions of peaks found in the trace with the FindPeaks() function */
  Float_t GetSimpleAmpl() ;
  /**< Simple amplitude estimator: maximum or minimum of the standard preprocessed trace */
  Bool_t BasicPreprocessed() { return fBasicPreprocessed; }
  /**< True if the trace was already preprocessed with the standard preprocessing algorithm */
  Float_t GetMax() const { return fProcessedTrace.Max(); }
  /**< Maximum of the trace */
  Float_t GetMin() const { return fProcessedTrace.Min(); }
  /**< Min of the trace */
  Float_t GetSimpleAmplBin() const { return fSimpleAmplBin; } /**< Bin of the pulse (using raw amplitude estimator) */
  Float_t GetIntegral(Int_t xmin,Int_t xmax) const { return fProcessedTrace.Integral(xmin,xmax); }
  /**< Integral of the trace in a given range */
  void SetSign(Short_t aSign) ; /**< Define the sign of the pulse */
  Short_t Sign() const { return fSign; } /**< Sign of the pulse. Known a priori, for example from the channel voltage */
  void SetPatternLength(UShort_t aLength) { fPatternLength = aLength; } /**< Pattern length */
  UShort_t PatternLength() const { return fPatternLength; } /**< Pattern length */
  Bool_t IsExpoBaseline() const { return fIsExpoBaseline; } /**< Returns 1 if the baseline is assumed to have an exponential shape. */
  void SetBaseStart(Int_t aStart) { fBaseStart = aStart; } /**< Pretrigger start */
  void SetBaseStop(Int_t aStop) { fBaseStop = aStop; } /**< Pretrigger stop */
  void SetBaseFromPeakBin(Int_t aBin) ; /**< Defines the pretrigger region from the bin of the pulse, given a priori as a parameter */

  void PlotTrace(string aPlotName); /**< Plot trace */
  void PlotPattern(string aPlotName); /**< Plot pattern */
  void PlotPeaks(string aPlotName) ; 
  /**< Plot trace and add red lines to show the peaks that have been detected by the simple FindPeaks() algorithm */

  Float_t ComputeNoise() ; /**< Basic noise computation from pretrigger data */
  void RemoveSaturatedSpikes(Int_t maxlength=3) ;
  void RemoveBaseline() ; /**< Subtract the baseline from pretrigger data */
  void RemoveLinearBase() ; /**< Substract linear function in order to have the same values in the first and last bin, to avoid spurious FFT effects */
  Bool_t RemoveExpoBaseline(Bool_t aRemove=1, Float_t aAmplMin=EXPOBASE_AMPLMIN) ;
  /**< fit expo in baseline, and if fit is != 0 above aSigmaCut, remove it */
  void RemovePattern(Int_t aNbPatternPts) ; /**< Pattern removal algorithm */
  void ApplyWindow(Int_t aWidth = 0) ; /**< Trace windowing for side effects in fft: modified Hanning window */
  void FindPeaks(Int_t aCriterium, Bool_t AnySign=0, Int_t aLength=FP_LENGTH) ;
  /**< Basic peak finder algorithm */
  void FindPeaks(Float_t aSensitivity, Int_t aLength, Int_t aOrder, Bool_t AnySign=0) ;
  /**< Basic peak finder algorithm */
  Bool_t DetectSinglePulse(); /**< Returns 1 if the basic peak finder detects a single pulse */
  void TranslateTrace(Int_t aShiftBins); /**< Translate the trace; uses NO extrapolation */
  void BasicPreprocess(Bool_t aCheckPeak=0); /**< Basic processing operations (no pulse fitting) */
  void ComputeTraceFFT(Bool_t aWindowFlag=1) ; /**< FFT of the trace */
  void FilterTrace(vector<Float_t> aDirect, vector<Float_t> aInverse) ; /**< Filtering of the trace in real space */

  vector<Float_t> GetFitAmpl(vector<Float_t> aTemplate, Bool_t aFitTime=1, Bool_t aInterpolTime=1, Float_t aTimeOffsetMin=0, Float_t aTimeOffsetMax=0, string aPlotFit="") ; 
  /**< Fit in direct space with a template. One can fit only the amplitude A, or also the time offset T0 of the impulsion maximum wrt the beginning of the trace. A sub-bin accuracy in the T0 fit is achieved if the flag aInterpolTime is set. The output is (A[,T0],Chi2) */

  vector<Float_t> GetWienerAmpl(const FloatVect& aTemplateFFT, const FloatVect& aNoise, Bool_t aDoFFT = 0, Bool_t aChi2Switch=0, string aPlotFilter="", string aPlotFit="") ;
  /**< Returns Wiener amplitude from linear fit, using the optimal filter algorithm. If aDoFFT is not set, it will be assumed that fProcessedTraceFFT was already computed. Output: (A, sigma_A) */
  vector<Float_t> WienerFast(const FloatVect& aKernel, const Float_t aDenom, Bool_t aDoFFT=0);
  /**< Subroutine for Wiener filtering: compute an aplitude using a kernel that was already fully calculated */
  vector<Float_t> GetFitWiener(const OffsetFFT* aTemplateFFTs, const FloatVect& aNoise, Bool_t aFast=1, Bool_t aSubBin=1, Float_t aOffsetMin=0, Float_t aOffsetMax=0, Float_t aOffsetStep=0, string aPlotFit="", Bool_t aNoFixSign=0) ;
  /**< Wiener fitting driver, with varying time offset. Returns (A,T0,Chi2,NbFitPts,sigma_A) */
  vector<Float_t> WienerLoop(const OffsetFFT* aTemplateFFTs, const FloatVect& aNoise, Float_t aMin, Float_t aMax, Float_t aStep, Float_t aAmpl, Bool_t aFast, Bool_t aDoFFT, Short_t aSignChange);
  /**< Subroutine for Wiener filtering: loop over an array of time offsets, and returns the best-fitting time offset */

  FloatVect SmoothedTrace(UInt_t aSmoothing) const;
  /**< Smoothed trace (top-hat window) */
  vector<Float_t> SmoothedAmpl(const FloatVect& aSmoothTrace) const ;
  /**< Smoothed trace */
  Float_t GetRiseTime(const Float_t aFracLow=0.2, const Float_t aFracHigh=0.9, UInt_t aSmooth=PULSESHAPE_SMOOTHING, string aPlot="");
  /**< Compute a pulse shape parameter using the smoothed trace: rise time. */
  Float_t GetFallTime(const Float_t aFracLow=0.3, const Float_t aFracHigh=0.9, UInt_t aSmooth=PULSESHAPE_SMOOTHING, string aPlot="");
  /**< Compute a pulse shape parameter using the smoothed trace: fall time */
  Float_t GetFWHM(const Float_t aFracWidth=0.5, UInt_t aSmooth=PULSESHAPE_SMOOTHING, string aPlot="");
  /**< Compute a pulse shape parameter using the smoothed trace: pulse width */

private:
  FloatVect fProcessedTrace; /**< Processed trace (initialized to the raw trace, then may undergo baseline substraction, depatterning, filtering.. */
  FloatVect fProcessedTraceFFT; /**< FFT of the processed trace */
  Int_t fBaseStart; /**< Pretrigger */
  Int_t fBaseStop; /**< Pretrigger */
  Float_t fMeanBase; /**< Average value of the baseline from the pretrigger region */
  vector<Float_t> fPattern; /**< Pattern */
  Float_t fSimpleAmplBin; /**< Raw amplitude */
  Float_t fSimpleAmpl; /**< Raw amplitude */
  Bool_t fBasicPreprocessed; /**< Set to 1 if baseline substraction, depatterning.. were already done for that pulse */
  vector<Int_t> fPeakBins; /**< Position of peaks detected by the raw peakfinder algorithm */
  Short_t fSign; /**< Pulse sign (defined a priori) */
  UShort_t fPatternLength; /**< Pattern */
  Bool_t fIsExpoBaseline; /**< expo baselien */

  ClassDef(FitPulse,1)
};

/**
   @class   PulseFitFunction
   @brief   Function used in Minuit fit

*/
class PulseFitFunction {
 public:
  PulseFitFunction() {}
  ~PulseFitFunction(){}
  PulseFitFunction(vector<Float_t>, Bool_t, Bool_t) ; /**< Standard constructor */
  Float_t operator() (Double_t* x, Double_t* p) ; /**< Used by Minuit */

 private:
  Int_t fTemplateSize; /**< Template */
  vector<Float_t> fTemplate; /**< Template */
  Bool_t fFitTime; /**< If set to 1, time is also a free parameter of the fit */
  Bool_t fInterpolTime; /**< If set to 1, time offset may take a non-integer value, the template is then interpolated */

};

#endif
