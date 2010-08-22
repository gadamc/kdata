/**
   @file    OffsetFFT.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Interface class describing a collection of FFTs with time offsets
*/

#ifndef _OFFSETFFT_H_
#define _OFFSETFFT_H_

#include "EdwUtils.h"

/**
   @class   OffsetFFT
   @brief   Interface class describing a collection of FFTs with time offsets

   Useful for template, used also directly by fitpulse

*/

class OffsetFFT : public TObject {
 public:
  OffsetFFT();
  ~OffsetFFT();
  vector<Float_t> GetOffsetFFT(Int_t aOffset) const ; /**< Return one the FFTs saved in the object */
  vector<Float_t> GetNonIntegerOffsetFFT(Float_t aOffset) const ; /**< Compute and returns the FFT with a non-integer time-offset, by interpolation of the existing ffts */
  time_t TStartKernel() const { return fTStartKernel; } /**< Kernel validity period */
  time_t TEndKernel() const { return fTEndKernel; } /**< Kernel validity period */
  void SetTStartKernel(time_t aTime) { fTStartKernel = aTime; } /**< Kernel validity period */
  void SetTEndKernel(time_t aTime) { fTEndKernel = aTime; } /**< Kernel validity period */
  void ComputeKernels(const FloatVect& aNoise) ; /**< Compute kernels from the list of ffts and a given noise, for optimal filtering purpose */
  vector<Float_t> GetKernel(Int_t aOffset) const ; /**< Return a kernel for optimal filtering */
  Float_t GetDenom(Int_t aOffset) const ; /**< Kernel denominator */
  Int_t MinOffset() const { return vectmin(fBinsOffsetFFT); } /**< Time range of the ffts saved in the object */
  Int_t MaxOffset() const { return vectmax(fBinsOffsetFFT); } /**< Time range of the ffts saved in the object */

  // todo = "clear" functions
  Int_t PulseBin() const { return fPulseBin; } /**< Pulse bin */
  Float_t PulseAbsMax() const { return fPulseAbsMax; } /**< abs max */
  Short_t PulseSign() const { return fPulseSign; } /**< sign */

 protected:
  vector<Float_t> fExtrapolatedTrace; /**< here now ! */
  vector<Int_t> fBinsOffsetFFT; /**< List of bins for which the ffts are saved */
  /**< Values of time offsets (in bins) for which the FFTs are saved in fTracesOffsetFFT */
  vector< vector<Float_t> > fTracesOffsetFFT;
  /**< Matrix of FFTs with different time offsets */
  UInt_t fTraceSize; /**< trace size */
  Int_t fPulseBin; /**< bin */
  Float_t fPulseAbsMax; /**< abs max */
  Short_t fPulseSign; /**< sign */


  // The following parameters should not be saved in files
  // They are used are calculus intermediate for wiener filtering
  time_t fTStartKernel; /**< t */
  time_t fTEndKernel; /**< t */
  vector<Float_t> fDenoms; /**< kernel denominators */
  vector< vector<Float_t> > fKernels; /**< kernels */

  ClassDef(OffsetFFT,2)

};

#endif
