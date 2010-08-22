
/**
   @file    EdwNtpBuffer.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwNtpBuffer class. See the .h file
*/

#include "EdwNtpBuffer.h"

//ClassImp(EdwNtpBuffer); /**< Root dictionnary */

//EdwNtpBuffer::EdwNtpBuffer() : TObject() {
EdwNtpBuffer::EdwNtpBuffer() {
  WienerSyncAmpl = new Matrix<Float_t> [NBMAX_AMPL];
  WienerSyncChi2 = new Matrix<Float_t> [NBMAX_AMPL];
  for (UInt_t i=0; i<NBMAX_AMPL; i++) { // a tout hasard..
    (WienerSyncAmpl[i]).clear();
    (WienerSyncChi2[i]).clear();
  }
  this->Clear();
}

EdwNtpBuffer::~EdwNtpBuffer() {
  this->Clear();
  delete [] WienerSyncAmpl; // par precaution. peut-etre cause de crash (?)
  delete [] WienerSyncChi2;
}

//EdwNtpBuffer::EdwNtpBuffer(UInt_t aBufSize, UInt_t aNbChannels) : TObject() {
EdwNtpBuffer::EdwNtpBuffer(UInt_t aBufSize, UInt_t aNbChannels) {
  BufferSize = aBufSize; 
  NbChannels = aNbChannels;
  EvtNum.resize(BufferSize);
  Run.resize(BufferSize); 
  DateSec.resize(BufferSize);
  DateMuSec.resize(BufferSize);
  TriggerBit1.resize(BufferSize);
  TriggerBit2.resize(BufferSize);
  RawBaseline.resize(BufferSize,NbChannels);
  RawNoise.resize(BufferSize,NbChannels);
  NbPeaks.resize(BufferSize,NbChannels);
  RawAmpl.resize(BufferSize,NbChannels);
  RawBin.resize(BufferSize,NbChannels);
  FitAmpl.resize(BufferSize,NbChannels);
  FitZeroAmpl.resize(BufferSize,NbChannels);
  FitBin.resize(BufferSize,NbChannels); 
  FitChi2.resize(BufferSize,NbChannels);
  FitAmplErr.resize(BufferSize,NbChannels);
  FitFilterAmpl.resize(BufferSize,NbChannels);
  FitFilterZeroAmpl.resize(BufferSize,NbChannels);
  FitFilterBin.resize(BufferSize,NbChannels); 
  FitFilterChi2.resize(BufferSize,NbChannels);
  FitFilterAmplErr.resize(BufferSize,NbChannels);
  WienerAmpl.resize(BufferSize,NbChannels);
  WienerZeroAmpl.resize(BufferSize,NbChannels);
  WienerBin.resize(BufferSize,NbChannels); 
  WienerChi2.resize(BufferSize,NbChannels);
  WienerAmplErr.resize(BufferSize,NbChannels);
  RiseTime.resize(BufferSize,NbChannels);
  FallTime.resize(BufferSize,NbChannels);
  FWHM.resize(BufferSize,NbChannels);
  WienerSyncAmpl = new Matrix<Float_t> [NBMAX_AMPL];
  WienerSyncChi2 = new Matrix<Float_t> [NBMAX_AMPL];
  for (UInt_t i=0; i<NBMAX_AMPL; i++) {
    (WienerSyncAmpl[i]).resize(BufferSize,NbChannels);
    (WienerSyncChi2[i]).resize(BufferSize,NbChannels);
  }
}

void EdwNtpBuffer::Clear() {
  BufferSize = 0; NbChannels = 0;
  EvtNum.clear(); Run.clear(); DateSec.clear();
  DateMuSec.clear(); TriggerBit1.clear(); TriggerBit2.clear();
  RawBaseline.clear(); RawNoise.clear(); NbPeaks.clear();
  RawAmpl.clear(); RawBin.clear();
  FitAmpl.clear(); FitZeroAmpl.clear(); FitBin.clear(); 
  FitChi2.clear(); FitAmplErr.clear();
  FitFilterAmpl.clear(); FitFilterZeroAmpl.clear(); FitFilterBin.clear(); 
  FitFilterChi2.clear(); FitFilterAmplErr.clear();
  WienerAmpl.clear(); WienerZeroAmpl.clear(); WienerBin.clear(); 
  WienerChi2.clear(); WienerAmplErr.clear();
  RiseTime.clear(); FallTime.clear(); FWHM.clear();
  for (Int_t i=0; i<NBMAX_AMPL; i++) {
    (WienerSyncAmpl[i]).clear();
    (WienerSyncChi2[i]).clear();
  }
}

void EdwNtpBuffer::ClearPulseVar(UInt_t ievt, UInt_t jchannel) {
  if (ievt >= BufferSize || jchannel >= NbChannels) {
    cerr <<"NtpBuffer::ClearPulseVar error. Exiting." <<endl; exit(-1);
  }

  RawBaseline[ievt][jchannel]=0;
  RawNoise[ievt][jchannel]=0;
  NbPeaks[ievt][jchannel]=0;
  RawAmpl[ievt][jchannel]=0;
  RawBin[ievt][jchannel]=0;
  FitAmpl[ievt][jchannel]=0;
  FitZeroAmpl[ievt][jchannel]=0;
  FitBin[ievt][jchannel]=0; 
  FitChi2[ievt][jchannel]=0;
  FitAmplErr[ievt][jchannel]=0;
  FitFilterAmpl[ievt][jchannel]=0;
  FitFilterZeroAmpl[ievt][jchannel]=0;
  FitFilterBin[ievt][jchannel]=0; 
  FitFilterChi2[ievt][jchannel]=0;
  FitFilterAmplErr[ievt][jchannel]=0;
  WienerAmpl[ievt][jchannel]=0;
  WienerZeroAmpl[ievt][jchannel]=0;
  WienerBin[ievt][jchannel]=0; 
  WienerChi2[ievt][jchannel]=0;
  WienerAmplErr[ievt][jchannel]=0;
  RiseTime[ievt][jchannel]=0;
  FallTime[ievt][jchannel]=0;
  FWHM[ievt][jchannel]=0;
  for (Int_t i=0; i<NBMAX_AMPL; i++) {
    (WienerSyncAmpl[i])[ievt][jchannel]=0;
    (WienerSyncChi2[i])[ievt][jchannel]=0;
  }
}

void EdwNtpBuffer::FillFromPulseVar(PulseVariables aData, UInt_t ievt, UInt_t jchannel) {
  if (ievt >= BufferSize || jchannel >= NbChannels) {
    cerr <<"NtpBuffer::FillFromPulseVar error. Exiting." <<endl; exit(-1);
  }

  RawBaseline[ievt][jchannel]=aData.RawBaseline;
  RawNoise[ievt][jchannel]=aData.RawNoise;
  NbPeaks[ievt][jchannel]=aData.NbPeaks;
  RawAmpl[ievt][jchannel]=aData.RawAmpl;
  RawBin[ievt][jchannel]=aData.RawBin;
  FitAmpl[ievt][jchannel]=aData.FitAmpl;
  FitZeroAmpl[ievt][jchannel]=aData.FitZeroAmpl;
  FitBin[ievt][jchannel]=aData.FitBin; 
  FitChi2[ievt][jchannel]=aData.FitChi2;
  FitAmplErr[ievt][jchannel]=aData.FitAmplErr;
  FitFilterAmpl[ievt][jchannel]=aData.FitFilterAmpl;
  FitFilterZeroAmpl[ievt][jchannel]=aData.FitFilterZeroAmpl;
  FitFilterBin[ievt][jchannel]=aData.FitFilterBin; 
  FitFilterChi2[ievt][jchannel]=aData.FitFilterChi2;
  FitFilterAmplErr[ievt][jchannel]=aData.FitFilterAmplErr;
  WienerAmpl[ievt][jchannel]=aData.WienerAmpl;
  WienerZeroAmpl[ievt][jchannel]=aData.WienerZeroAmpl;
  WienerBin[ievt][jchannel]=aData.WienerBin; 
  WienerChi2[ievt][jchannel]=aData.WienerChi2;
  WienerAmplErr[ievt][jchannel]=aData.WienerAmplErr;
  RiseTime[ievt][jchannel]=aData.RiseTime;
  FallTime[ievt][jchannel]=aData.FallTime;
  FWHM[ievt][jchannel]=aData.FWHM;
  for (Int_t i=0; i<NBMAX_AMPL; i++) {
    (WienerSyncAmpl[i])[ievt][jchannel]=(aData.WienerSyncAmpl)[i];
    (WienerSyncChi2[i])[ievt][jchannel]=(aData.WienerSyncChi2)[i];
  }
}
