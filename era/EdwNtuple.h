/**
   @file    EdwNtuple.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class used to build simple Root output analysis ntuples
*/

#ifndef EDWNTUPLE_H_
#define EDWNTUPLE_H_

#include "EdwUtils.h"
#include "EdwEvent.h"
#include "EdwTemplate.h"
#include "FitPulse.h"
#include "EdwAnaDB.h"
#include "EdwRawDB.h"
#include "EdwNtpBuffer.h"

/**
   @enum    AmplList
   @brief   Flags to decide which kind of amplitude to compute.

*/
enum AmplList {
  AMPL_RAW = 0x101,
  AMPL_FIT,
  AMPL_FITFILT,
  AMPL_WIENER,
  AMPL_ALL
} ;

/**
   @enum    PulseShapeList
   @brief   Flags to decide which pulse shape parameters should be computed
*/
enum PulseShapeList {
  SHAPE_NO = 0x201,
  SHAPE_RISE,
  SHAPE_FALL,
  SHAPE_FWHM,
  SHAPE_ALL
} ;

#define DEFAULT_PRINTAMPLERR 1 /**< Flag to say if the sigma_a is recorded in the ntuples */
#define EVTBUFFERSIZE 2000 /**< Size of the event buffer when used. Must be large enough to be useful, not too large to have a reasonable memory occupation */
#define NOCENTER 10000 /**< Conventional number used as a flag for heat->ionization time synchronization */

/**
   @class   EdwNtuple
   @brief   Class used to build simple Root output analysis ntuples

   The class isn't complete yet - major changes still to come. The idea is to have a "standard" ntuple format which could be used later by simple macros, as well as inside the EdwRootAna program. Still have to decide whether this has to be a real TTree or a TNtuple.

*/
class EdwNtuple : public TObject {
 public:
  EdwNtuple() ;
  ~EdwNtuple() ;
  EdwNtuple(string aFileName, string aTitle="EdwRootNtp"); /**< Standard constructor */
  AmplList AmplFlag() const { return fAmplFlag; } /**< Flag for amplitude computation */
  PulseShapeList ShapeFlag() const { return fShapeFlag; } /**< Flag for pulse shape parameter computation */
  void SetAmplFlag(AmplList aFlag) { fAmplFlag = aFlag; } /**< Flag for amplitude computation */
  void SetShapeFlag(PulseShapeList aFlag) { fShapeFlag = aFlag; } /**< Flag for pulse shape parameter computation */
  void SetPrintAmplErr(Bool_t aPrint) { fPrintAmplErr = aPrint; } /**< Flag to record or not the sigma_ampl value on an event by event basis */
  UInt_t EvtBufferSize() const { return fEvtBufferSize; } /**< Size of the buffer used in the ntuple calculation loops */
  void SetEvtBufferSize(UInt_t aSize) { fEvtBufferSize=aSize; } /**< Size of the buffer used in the ntuple calculation loops */
  
  void PrepareNtp() ; 
  /**< Subroutine defining the various branches of the TTree from the involved channels */
  void FillEvtFromBuffer(UInt_t ievt, EdwNtpBuffer& aBuffer) ; 
  /**< Fill the TTree from a buffer which has been computed */

  PulseVariables ComputeSinglePulse(EdwPulse* aEdwPulse, EdwTemplate& aTmplt, EdwPreprocessData& aPreprocess, Short_t aSign=0, Int_t aPattern=0, Int_t aOffsetScanMin=NOCENTER, Int_t aOffsetScanMax=NOCENTER, const vector<Float_t> *aFixTimes = NULL);
  /**< Basic step : computation of amplitudes and pulse shape parameters associated to a single pulse. 
     The relative timing parameters of the pulse wrt another pulse may be fixed or constrained by making use of the aCenterScanBin and aFixTimes arguments. A PulseVariables structure is returned. */
  PulseVariables ComputeSinglePulse(FitPulse& lPulse, EdwTemplate& aTmplt, EdwPreprocessData& aPreprocess, Short_t aSign = 0, Int_t aCenterScanBin = NOCENTER, const vector<Float_t> *aFixTimes = NULL);
	/**<Same as ComputSinglePulse(EdwPulse* ), but you must pass in a FitPulse reference. The advantage of this is that you get to see the resulting processed waveform via the FitPulse object.*/
	
	void SingleChannelEventLoop(UInt_t jchannel, TChain* aChain, EdwNtpBuffer& aBuffer, EdwAnaDB* aDB, EdwRawDB* aRawDB, EdwEvent* aEvt, EdwPulse* aPulse, UInt_t aOffset=0, Int_t aSynchroFlag=0);
  /**< Fundamental loop over the events for a single channel. 
     The aBuffer object is filled. The possible synchronization of the channel wrt other channels is taken into account with the aSynchroFlag argument: 0 = no sync; 1 = heat->ion (constrain the time search window); 2 = ion->heat (fix the time completely). */

  void FillDataFromChain(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB = NULL, ULong_t iStart=0, ULong_t iStop=0, EdwRawDB* aRawDB = NULL) ;
  /**< Performs complete processings,  */
  // Todo: aussi le istart/istop pour cette fct.
  void FillDataByChannelOrder(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB = NULL, ULong_t iStart=0, ULong_t iStop=0, EdwRawDB* aRawDB = NULL) ;
  /**< Same as FillDataFromChain but the loop is on channels and not events, which is more optimal for CPU in wiener. */
  //Go from n_evt=istart to istop-1. (eg. 0-->9999 then 10000->19999...) 
 
  void FillDataWithChannelSynchro(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB = NULL, ULong_t iStart=0, ULong_t iStop=0, EdwRawDB* aRawDB = NULL) ;
  /**< Driver to compute the ntuple using channel time synchronization */
  void PlotSelectEvents(TChain* aChain, vector<string> aChannels, string aEvtListFile, string aPlotDir, EdwAnaDB* aDB = NULL, EdwRawDB* aRawDB = NULL) ;
  /**< Plot a list of events */
  void SetHeatIonSynchro(string aFile) ; /**< This file contains information on time synchronization of different channels */
  vector<Float_t> GetSyncParams(string aHeatChannel) const ; /**< Synchro params between channels */
  vector<string> GetSyncIonChannels(string aHeatChannel) const; /**< Synchro params between channels */
  string GetSyncHeatChannel(string aIonChannel) const; /**< Synchro params between channels */

	void SetChannels(vector<string> aChanList){fChannels = aChanList;}
	
 private:
  UInt_t fEvtBufferSize; /**< Size of the intermediate buffer */
  string fFileName ; /**< Name of the root file */
  TFile* fFile; /**< root file */
  AmplList fAmplFlag; /**< Flag to determine which amplitudes (raw, wiener,..) must be computed */
  PulseShapeList fShapeFlag; /**< Flag to determine which pulse shape parameters must be computed */
  Bool_t fPrintAmplErr; /**< Flag to record the error on amplitude determination, sigma_a. 1 by default */
  TTree* fTree ; /**< Tree to be filled */
  vector<string> fChannels; /**< Channels to be processed */
  vector<Float_t> fRawBaseline; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fRawNoise; /**< Values to be computed and assigned to ntuple variables */
  vector<Int_t> fNbPeaks; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fRawAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fRawBin; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitZeroAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitBin; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitChi2; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitAmplErr; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitFilterAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitFilterZeroAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitFilterBin; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitFilterChi2; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFitFilterAmplErr; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fWienerAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fWienerZeroAmpl; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fWienerBin; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fWienerChi2; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fWienerAmplErr; /**< Values to be computed and assigned to ntuple variables */
  FloatVect *fWienerSyncAmpl; /**< Values to be computed and assigned to ntuple variables */
  FloatVect *fWienerSyncChi2; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fRiseTime; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFallTime; /**< Values to be computed and assigned to ntuple variables */
  vector<Float_t> fFWHM; /**< Values to be computed and assigned to ntuple variables */
  ULong64_t fDateSec; /**< Values to be computed and assigned to ntuple variables */
  Int_t fDateMuSec; /**< Values to be computed and assigned to ntuple variables */
  UInt_t fEvtNum; /**< Values to be computed and assigned to ntuple variables */
  char fRun[9]; /**< Values to be computed and assigned to ntuple variables */
  Int_t fTriggerBit1; /**< Values to be computed and assigned to ntuple variables */
  Int_t fTriggerBit2; /**< Values to be computed and assigned to ntuple variables */
  string fHeatIonSynchroFile; /**< Values to be computed and assigned to ntuple variables */

  ClassDef(EdwNtuple,1)
};

#endif
