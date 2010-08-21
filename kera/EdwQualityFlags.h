/**
   @file    EdwQualityFlags.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class containing some quality criteria associated to a given channel for a given period. This object is stored within the EdwAnaDB database.
*/

#ifndef _EDWQUALITYFLAGS_H_
#define _EDWQUALITYFLAGS_H_

#include "EdwUtils.h"
#include "EdwEvent.h"
#include "FitPulse.h"

/**
   @class   EdwQualityFlags
   @brief   Class containing some quality criteria associated to a given channel for a given period. This object is stored within the EdwAnaDB database.

   Currently baselines are implemented, calculated from a gaussian fit of the amplitude distribution of noise events, with amplitudes obtained by standard time or frequency fits. Still some improvments to do... many improvments to do.. currently the class is not in use!

*/
class EdwQualityFlags : public TObject {
 public:
  EdwQualityFlags() ;
  ~EdwQualityFlags() ;
  EdwQualityFlags(time_t aStart, time_t aStop, string aChannel) ;
  /**< Constructor for a given period [aStart,aStop] and a given channel */
  time_t StartTime() const { return fStartTime; }
  /**< Beginning of the period of validy */
  time_t EndTime() const { return fEndTime; }
  /**< End of the period of validity */
  string Channel() const { return fChannel; }
  /**< Represented channel */
  Float_t GetMeanRawNoise() const { return fMeanRawNoise; }
  /**< Average of raw noises as computed from the stdev of pretrigger data (to improve..) */
  Float_t GetStdRawNoise() const { return fStdRawNoise; }
  /**< Stddev of raw noises from pretrigger data (to improve..) */
  Float_t GetBaselineTimeFit() const { return fBaselineTimeFit; }
  /**< Baseline noise obtained from time fits of noise events */
  Float_t GetBaselineFreqFit() const { return fBaselineFreqFit; }
  /**< Baseline noise obtained from frequency fits of noise events */

  void ComputeRawNoise(TChain* aChain) ;
  /**< Computes raw noises - to upgrade a lot */
  void ComputeBaselines(TChain* aChain, vector<Float_t> aTemplate, const FloatVect& aTemplateFFT, const FloatVect& aNoise, string aPlotHisto="", Bool_t aDoTimeOnly=0, Bool_t aDoFreqOnly=0);
  /**< Computes baseline noises from time and/or frequency fits of noise events. The fits are made with a fixed time for the template. */

 private:
  time_t fStartTime ;  /**< Beginning of the period of validy */
  time_t fEndTime ;  /**< End of the period of validity */
  string fChannel ;  /**< Represented channel */
  Float_t fMeanRawNoise ;  /**< Average of raw noises as computed from the stdev of pretrigger data (to improve..) */
  Float_t fStdRawNoise ;  /**< Stddev of raw noises from pretrigger data (to improve..) */
  Float_t fBaselineTimeFit;  /**< Baseline noise obtained from time fits of noise events */
  Float_t fBaselineFreqFit;  /**< Baseline noise obtained from frequency fits of noise events */

  ClassDef(EdwQualityFlags,1)

};

#endif
