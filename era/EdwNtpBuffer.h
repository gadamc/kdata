
/**
   @file    EdwNtpBuffer.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class used by ntuples
*/

#ifndef _EDWNTPBUFFER_H_
#define _EDWNTPBUFFER_H_

#include "EdwUtils.h"

#define NBMAX_AMPL 6 /**< Size of the arrays WienerSyncAmpl/Chi2 */

/**
   @struct   PulseVariables
   @brief    Structure containing all variables associated to a single pulse of an event, which are associated to the ntuple.

   This variable is computed by the routine ComputeSinglePulse and then is used to fill the current EdwNtpBuffer object.

*/
struct PulseVariables {
  Float_t RawBaseline; /**< Pulse variable */
  Float_t RawNoise; /**< Pulse variable */
  Int_t NbPeaks; /**< Pulse variable */
  Float_t RawAmpl; /**< Pulse variable */
  Float_t RawBin; /**< Pulse variable */
  Float_t FitAmpl; /**< Pulse variable */
  Float_t FitZeroAmpl; /**< Pulse variable */
  Float_t FitBin; /**< Pulse variable */
  Float_t FitChi2; /**< Pulse variable */
  Float_t FitAmplErr; /**< Pulse variable */
  Float_t FitFilterAmpl; /**< Pulse variable */
  Float_t FitFilterZeroAmpl; /**< Pulse variable */
  Float_t FitFilterBin; /**< Pulse variable */
  Float_t FitFilterChi2; /**< Pulse variable */
  Float_t FitFilterAmplErr; /**< Pulse variable */
  Float_t WienerAmpl; /**< Pulse variable */
  Float_t WienerZeroAmpl; /**< Pulse variable */
  Float_t WienerBin; /**< Pulse variable */
  Float_t WienerChi2; /**< Pulse variable */
  Float_t WienerAmplErr; /**< Pulse variable */
  Float_t WienerSyncAmpl[NBMAX_AMPL]; /**< Pulse variable */
  Float_t WienerSyncChi2[NBMAX_AMPL]; /**< Pulse variable */
  Float_t RiseTime; /**< Pulse variable */
  Float_t FallTime; /**< Pulse variable */
  Float_t FWHM; /**< Pulse variable */
};

// Pour faire plus simple on a decide de ne plus faire heriter de root cette classe..
//class EdwNtpBuffer : public TObject {

/**
   @class   EdwNtpBuffer
   @brief   Contains an array of all ntuple variables

   This is used by the ntuple main loops when using separate buffers which are needed when the loop is made first over channels and not over events.

*/
class EdwNtpBuffer {
 public:
  EdwNtpBuffer();
  ~EdwNtpBuffer();
  EdwNtpBuffer(UInt_t aBufSize, UInt_t aNbChannels); 
  /**< Initialize the buffer with a size adapted to a given depth and a given number of channels */
  void Clear(); /**< Clear completely the buffer */
  void ClearPulseVar(UInt_t ievt, UInt_t jchannel); 
  /**< Reset to 0 the pulse variables associated to a given event and channel in the buffer */
  void FillFromPulseVar(PulseVariables aData, UInt_t ievt, UInt_t jchannel);
  /**< Fill the pulse variables associated to a given event and channel in the buffer */
  void UpdateFromPulseVar(PulseVariables aData, UInt_t ievt, UInt_t jchannel);
  /**< Fill only the "sync" pulse variables */

  UInt_t BufferSize; /**< Buffer depth (number of events) */
  UInt_t NbChannels; /**< Nb of channels in the buffer */
  vector<UInt_t> EvtNum; /**< List of Samba event numbers */
  vector<string> Run; 
  /**< List of samba run names. on reste en string, on passe en char[9] plus tard */
  vector<ULong64_t> DateSec; /**< List of event times */
  vector<Int_t> DateMuSec; /**< List of event times (microseconds) */
  vector<Int_t> TriggerBit1; /**< List of bit triggers */
  vector<Int_t> TriggerBit2; /**< List of bit triggers */
  Matrix<Float_t> RawBaseline; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> RawNoise; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Int_t> NbPeaks; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> RawAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> RawBin; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitZeroAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitBin; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitChi2; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitAmplErr; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitFilterAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitFilterZeroAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitFilterBin; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitFilterChi2; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FitFilterAmplErr; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> WienerAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> WienerZeroAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> WienerBin; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> WienerChi2; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> WienerAmplErr; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> *WienerSyncAmpl; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> *WienerSyncChi2; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> RiseTime; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FallTime; /**< List of pulse variables (matrix format): var[ievt][jchannel] */
  Matrix<Float_t> FWHM; /**< List of pulse variables (matrix format): var[ievt][jchannel] */

  //  ClassDef(EdwNtpBuffer,1)
};

#endif
