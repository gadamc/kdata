/**
   @file    EdwTemplate.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class describing the template associated to a given channel.
*/

#ifndef _EDWTEMPLATE_H_
#define _EDWTEMPLATE_H_

#include "EdwUtils.h"
#include "EdwEvent.h"
#include "FitPulse.h"
#include "OffsetFFT.h"

// From mkfilter: Butterworth 2nd order
// Heat > 5 Hz (assuming sampling at 1kHz)
// Ion [1-20] kHz (assuming sampling at 100 kHz)
#define DEFAUT_FILTER_HEAT_DIRECT_SIZE 3 /**< Nb of direct coefficients for heat filter */
#define DEFAUT_FILTER_HEAT_DIRECT {0.97803047886,-1.95606095772,0.97803047886} /**< Heat filter direct coefs */
#define DEFAUT_FILTER_HEAT_INVERSE_SIZE 2 /**< Nb of inverse coefs for heat filter */
#define DEFAUT_FILTER_HEAT_INVERSE {1.9555782403,-0.9565436765} /**< Heat filter inverse coefs */
#define DEFAUT_FILTER_ION_DIRECT_SIZE 5 /**< Nb of direct coefs for ionization filter */
#define DEFAUT_FILTER_ION_DIRECT {0.19296035298,0,-0.38592070596,0,0.19296035298} /**< Ionization filter direct coefs */
#define DEFAUT_FILTER_ION_INVERSE_SIZE 4 /**< Nb of inverse coefs for ionization filter */
#define DEFAUT_FILTER_ION_INVERSE {2.3350824021,-1.9509646898,0.8192636853,-0.2066719852} /**< Ionization filter inverse coefs */

// for consistency these should be the same as "FIT_TIMEMIN.." in fitpulse :
#define FFTMINOFFSET_HEAT -10 /**< Minimum of the range in offsets computed in the fft array for heat channels */ 
#define FFTMAXOFFSET_HEAT 10 /**< Maximum of the range in offsets computed in the fft array for heat channels */ 
#define FFTMINOFFSET_ION -2000 /**< Minimum of the range in offsets computed in the fft array for ionization channels */ 
//#define FFTMAXOFFSET_ION 1000 // Pas assez en synchro heat! --> modif to 2000
#define FFTMAXOFFSET_ION 2000 /**< Maximum of the range in offsets computed in the fft array for ionization channels */ 
#define FFTBINOFFSET 1 /**< Offset bin in the fft arrays */ 

/**
   @class   EdwTemplate
   @brief   Class describing the template associated to a given channel.

   This template is defined for a given channel and a given period. The class contains the pulse template as well as its Fourier representation. A template is currently built in 2 steps from a given list of events.... blabla..
   The template is not the raw trace of Short_t type, but the preprocessed trace, currently obtained after the standard basic preprocessing operation.

*/
class EdwTemplate : public OffsetFFT {
 public:
  EdwTemplate() ;
  ~EdwTemplate() ;
  EdwTemplate(time_t aStart, time_t aEnd, vector<string> aRuns, string aChannel);
  /**< Constructor of EdwTemplate, defined by a given period [aStart,aEnd] for a given channel, computed from a list of runs aRuns  */
  EdwTemplate(time_t aStart, time_t aEnd, vector<string> aRuns, string aChannel, string aASCIIDir, string aPlotDir="None");
  /**< Constructor from asii file */
  void Clear(); 
  /**< Clear the template */
  void DumpInfo() const ; /**< dump info but not all the trace points */
  void DumpTrace(string aFile) const ; /**< Dump trace points */
  Bool_t IsHeat(); 
  /**< True if channel is heat */
  string Channel() {return fChannel; } 
  /**< Template channel */
  void SetChannel(string aChannel) { fChannel = aChannel; } 
  /**< Set template channel */
  time_t StartValidity() { return fStartValidity;} 
  /**< Start of the period of validity */
  time_t EndValidity() { return fEndValidity;} 
  /**< End of the period of validity */
  void SetValidity(time_t aTStart, time_t aTEnd) ; 
  /**< Set the period of validity */
  void PlotTrace(string aPlotName);
  /**< Plot the trace of template */
  void PlotFilteredTrace(string aPlotName); /**< Plot the trace after standard filtering */
  void PlotExtraTrace(string aPlotName); /**< Plot the trace with its exponential extrapolation */

  void BuildBasicTemplate(TChain* aChain, Float_t aAmpl, Int_t aBin, Float_t aDeltaBin);
  /**< First step in template building: select one single event with one detected pulse, a pulse amplitude near aAmpl and a bin location near aBin (with typical accuracy aDeltaBin). The trace of this single event is the template */

  void BuildFitTemplate(TChain* aEvents, Float_t aAmpl, Int_t aBin, Float_t aDeltaBin, Float_t aChi2PerDofCut=0, string aPlotPulses="") ;
  /**< Second step in template building: a first estimate of template is used to select events from the chain with the following criteria: single pulse, amplitude, pulse location, chi2. The template is then the average of the selected event traces */

  void BuildTemplateFromASCIITrace(string aFile) ; 
  /**< Instead of building the template from a chain of events, just use an ascii file containing a list of points representing the template. */

  void ComputeFFT() ; 
  /**< Computes the FFT of the template trace */
  void ExtrapolateTrace(Float_t aExpoFitFraction=0.75, Bool_t aFitBaseline=1);
  /**< Build an extrapolated trace from the basis trace, with double size, and exponential extrapolation of the pulse, starting from aExpoFitFraction of the basic trace. A non-zero post-pulse baseline may be fitted (case of an overshoot) */
  void BuildOffsetFFTs(Int_t aMinOffset, Int_t aMaxOffset, Int_t aBin=1) ; 
  /**< Build an array of FFTs of the trace, translated by offsets ranging from MinOffset to MaxOffset */
  void BuildOffsetFFTs() ;
  /**< Build an array of FFTs of the trace, translated by offsets ranging from MinOffset to MaxOffset */

  void SetFilterParams(); /**< The filters will be set to their default values */
  void SetFilterParams(vector<Float_t> aDirect, vector<Float_t> aInverse); /**< Use custom values for filter */
  vector<Float_t> GetDirectFilter() const { return fDirectFilterCoefs; } /**< Direct filter coefs */
  vector<Float_t> GetInverseFilter() const { return fInverseFilterCoefs; } /**< Inverse filter coefs */
  vector<Float_t> FilteredTrace() const { return fFilteredTrace; } /**< Filtered trace from the 'basic' filter */
  FloatVect TraceFFT() const ; /**< FFT of the trace */
  vector<Float_t> VectTraceFFT() const { return fTraceFFT; } /**< FFT of the trace */

  vector<Float_t> Trace() const { return fTrace; } /**< Trace */
  vector<Float_t> ExtrapolTrace() const { return fExtrapolatedTrace; } /**< Trace with exponential extrapolation at large times */
  vector<string> BuildRuns() const { return fBuildRuns; } /**< List of the runs that were used to build the template */
  Float_t ExpoFitFraction() const { return fExpoFitFraction; } /**< Parameter for exponential extrapolation of the trace */
  vector<Int_t> EventsUsed() const { return fEventsUsed; } /**< List of samba event numbers that were used to build the template */
  vector<string> RunsOfEventsUsed() const { return fRunsOfEventsUsed; } /**< See this->EventUsed() */
  
  void ComputePulseBin() ; /**< Bin of the pulse in the template. Defined as the maximum of abs value */

 private:
  vector<Float_t> fDirectFilterCoefs; /**< Direct filter coefs */
  vector<Float_t> fInverseFilterCoefs; /**< Inverse filter coefs */
  vector<Float_t> fFilteredTrace; /**< Filtered trace from the 'basic' filter */
  vector<string> fBuildRuns;   /**< Runs used to build the template */
  vector<Float_t> fTrace; /**< Template trace*/
  //  vector<Float_t> fExtrapolatedTrace;  --> now in OffsetFFT!!!
  //  Longer trace, with exponential extrapolation, for time translation in Fourier fit
  Float_t fExpoFitFraction ;   /**< Used in extrapolated trace computation */
  string fChannel; /**< Template channel */
  time_t fStartValidity; /**< Period of validity */
  time_t fEndValidity; /**< Period of validity */
  vector<Int_t> fEventsUsed; 
  /**< Events effectively used to construct the template: list of their Samba event numbers */
  vector<string> fRunsOfEventsUsed;
  /**< Events effectively used to construct the template: list of their Samba run names */
  vector<Float_t> fTraceFFT ;
  /**< FFT of fTrace, in the format used by Root (N/2 first points = real part; N/2 last point = imaginary part) */

  ClassDef(EdwTemplate,2)

};

#endif
