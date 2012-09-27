/**
   @file    Pulse.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Basic class (but non-virtual) describing a pulse
*/

#ifndef _PULSE_H_
#define _PULSE_H_

#include "EdwUtils.h"

/**
   @class   Pulse
   @brief   Basic class (but non-virtual) describing a pulse

   The pulse information is stored as short data here.

*/
class Pulse : public TObject {
 public:
  Pulse() ;
  ~Pulse() ;
  string Channel() const { return fChannel; } /**< Channel */
  vector<Short_t> Trace() const { return fTrace; } /**< Raw trace (short type) */
  Int_t TraceSize() const { return fTrace.size(); } /**< Trace size */
  Short_t Trace(Short_t pt) const { return fTrace.at((Int_t)pt); } /**< Raw trace */
  ULong_t Sampling_ns() const { return fSampling_ns; } /**< Sampling frequency */
  Bool_t IsHeat() const { return fIsHeat; } /**< Returns 1 if the channels is "chaleur..." */
  Bool_t IsSaturated() ; /**< Returns 1 if the trace (of short type) reaches saturation */

  void SetChannel(string ch) ; /**< Channel */
  void SetSampling_ns(ULong_t aSampling) { fSampling_ns = aSampling; } /**< Sampling frequency */
  void SetTrace(const vector<Short_t> & aTrace) { fTrace = aTrace; } /**< Trace */
  void SetTrace(UShort_t n, const Short_t* aData) ; /**< Trace */
  void ClearTrace() { fTrace.clear(); } /**< Clear trace */
  void PlotRawTrace(string aPlotName) ; /**< Plot trace (root) */
  
 protected:
  string fChannel; /**< Channel */
  Bool_t fIsHeat ; /**< heat channel or not */
  ULong_t fSampling_ns ; /**< Sampling frequency */
  vector<Short_t> fTrace; /**< Raw trace */
  // TODO: avoir une variable isheat (evite compare channel...)

  ClassDef(Pulse,1)
};

#endif
