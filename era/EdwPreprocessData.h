/**
   @file    EdwPreprocessData.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class encapsulating the preprocessing data which is needed as a first analysis step. The EdwPreprocessData objects are stored within the EdwAnaDB database.
*/

#ifndef _EDWPREPROCESSDATA_H_
#define _EDWPREPROCESSDATA_H_

// class to be inserted in edwanadb
// data from pulse preprocessing in periods
// (typically 15 min - 1h), such as noises, spectra...

#include "EdwUtils.h"
#include "FitPulse.h"
#include "EdwEvent.h"

/**
   @class   EdwPreprocessData
   @brief   Class encapsulating the preprocessing data which is needed as a first analysis step. The EdwPreprocessData objects are stored within the EdwAnaDB database.

   Currently the most important data stored in this object is the noise spectrum, used for optimal filtering.

*/
class EdwPreprocessData : public TObject {
 public:
  EdwPreprocessData() ;
  ~EdwPreprocessData() ;
  EdwPreprocessData(time_t, time_t, string) ; /**< Standard constructor: period of validity and channel */

  virtual void Clear(); /**< clear data*/
  void DumpInfo() const ; /**< Print basic information on the data to stdout */
  time_t StartTime() const { return fStartTime; } /**< Period of validity of these data */
  time_t EndTime() const { return fEndTime; } /**< Period of validity of these data */
  void SetStartTime(time_t aTime) { fStartTime = aTime; } /**< Period of validity of these data */
  void SetEndTime(time_t aTime) { fEndTime = aTime; } /**< Period of validity of these data */
  Bool_t IsTimeOk(time_t aTime) ; /**< Returns 1 if aTime is in the period of validity of the data */

  string Channel() const { return fChannel; } /**< Channel concerned by the data */
  void SetChannel(string aChannel) { fChannel = aChannel; } /**< Channel */
  FloatVect NoiseSpectrum() const { return CopyVect(fNoiseSpectrum); } 
  /**< Noise spectrum storage. Warning: This is the value of sigma2 - not sigma */
  void SetNoiseSpectrum(vector<Float_t> aNoise) { fNoiseSpectrum = aNoise; }
  /**< Noise spectrum storage. Warning: This is the value of sigma2 - not sigma */
  //  void SetFrequencyWeights(vector<Float_t> aWeights) { fFrequencyWeights = aWeights; }
  //  FloatVect FrequencyWeights() const { return CopyVect(fFrequencyWeights); }
  Float_t CorrelSlope() const { return fCorrelSlope; } /**< Not used yet.. */
  Float_t CorrelFactor() const { return fCorrelFactor; } /**< Not used yet.. */
  void SetCorrelSlope(Float_t aSlope) { fCorrelSlope = aSlope; } /**< Not used yet.. */
  void SetCorrelFactor(Float_t aFactor) { fCorrelFactor = aFactor; } /**< Not used yet.. */
  void SetCorrelChannels(vector<string> aChannels) { fCorrelChannels=aChannels; } /**< Not used yet.. */
  vector<string> CorrelChannels() const { return fCorrelChannels; } /**< Not used yet.. */
  void PlotSpectrum(string aPlotName, Bool_t aNoLog=0) ; /**< Plot the power spectrum. To improve */

  void SetPulseBin(Int_t aBin) { fPulseBin = aBin; } /**< Must be used to estimate the power spectrum from raw traces */
  void ComputeNoiseSpectrum(TChain* aChain); /**< Routine to compute the power spectrum from a chain of events */
  /*  void BasicChainPreprocess(TChain* aChain) ;
      void ComputeBasicCorrelation(TChain* aChain) ;*/
  
 private:
  time_t fStartTime ; /**< Period of validity */
  time_t fEndTime ; /**< Period of validity */
  string fChannel ; /**< Channel */
  vector<Int_t> fNbPtsPatterns ; /**< Not used yet.. */
  vector<Float_t> fNoiseSpectrum ; /**< Noise spectrum */
  //  vector<Float_t> fFrequencyWeights; // relative to a template since it is A^2(f)/N(f)
  Int_t fNbTracesForNoiseSpectrum ; /**< Number of raw traces that were used to estimate the noise spectrum */
  Float_t fCorrelSlope ; /**< Not used yet.. */
  Float_t fCorrelFactor ; /**< Not used yet.. */
  vector<string> fCorrelChannels; /**< Not used yet.. */
  Int_t fPulseBin; /**< Must be used to estimate the power spectrum from raw traces */

  ClassDef(EdwPreprocessData,2)
    
};

#endif
