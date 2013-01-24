/*
 *  KMuonVetoSystemRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KMUONVETOSYSTEMRECORD_H__
#define __KMUONVETOSYSTEMRECORD_H__

#include "TBits.h"
#include "Rtypes.h"

//const Int_t kSizeOfChannelArrays = 97;

class KMuonVetoSystemRecord  { 

public:
  //Constructors
  KMuonVetoSystemRecord(void);
  KMuonVetoSystemRecord(const KMuonVetoSystemRecord &aRec);
  KMuonVetoSystemRecord& operator=(const KMuonVetoSystemRecord &aRec);
  virtual ~KMuonVetoSystemRecord(void);
  virtual void Clear(Option_t *opt = "C");
  Bool_t IsSame(const KMuonVetoSystemRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KMuonVetoSystemRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KMuonVetoSystemRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);
  
  Bool_t IsTimeReconstructed(void) const;
  Bool_t IsGoodMuonVetoData(void) const;

  //getters
  Bool_t IsSystemOn(void) const {return fSystemOn;}
  Int_t GetMultiADC(void) const {return fMultiADC;} 
  Double_t GetDistanceEst(void) const {return fDistanceEst;}  
  Double_t GetDistanceNemo(void) const {return fDistanceNemo;}  
  UInt_t GetEventNumber(void) const {return fEventNumber;}  
  Int_t GetFileNumber(void) const {return fFileNumber;}
  Int_t GetMultiTDC(void) const {return fMultiTDC;} 
  Int_t GetPcTimeSec(void) const {return fPcTimeSec;} 
  Int_t GetPcTimeMuSec(void) const {return fPcTimeMuSec;} 
  Bool_t TestEventQualityBit(Int_t i) const;
  Int_t GetRunNumber(void) const {return fRunNumber;}
  Int_t GetCommonStopTime(void) const {return fCommonStopTime;}
  
  Bool_t IsReliableStamp(void) const {return TestEventQualityBit(0);}
  Bool_t IsLEDFired(void) const {return TestEventQualityBit(10);}
  
  Double_t GetRunStartTime(void) const {return fRunStartTime;}
  Double_t GetRunEndTime(void) const {return fRunEndTime;}
  Double_t GetFileStartTime(void) const {return fFileStartTime;}
  Double_t GetFileEndTime(void) const {return fFileEndTime;}

  //setters
  void SetSystemOn(Bool_t a) {fSystemOn = a;}
  void SetMultiADC(Int_t a) {fMultiADC = a;}
  void SetDistanceEst(Double_t a) {fDistanceEst = a;}
  void SetDistanceNemo(Double_t a) {fDistanceNemo = a;}
  void SetEventNumber(UInt_t a) {fEventNumber = a;}
  void SetFileNumber(Int_t a) {fFileNumber = a;}
  void SetMultiTDC(Int_t a) {fMultiTDC = a;}
  void SetPcTimeSec(Int_t a) {fPcTimeSec = a;}
  void SetPcTimeMuSec(Int_t a) {fPcTimeMuSec = a;}
  void SetEventQualityBit(Int_t i, Bool_t val = true) {fEventQuality.SetBitNumber(i,val);}
  void SetEventQualityBits(TBits *aBits);
  void SetRunNumber(Int_t aNum) {fRunNumber = aNum;}
  void SetCommonStopTime(Int_t aNum) {fCommonStopTime = aNum;}
  
  void SetRunStartTime(Double_t aNum) {fRunStartTime = aNum;}
  void SetRunEndTime(Double_t aNum) {fRunEndTime = aNum;}
  void SetFileStartTime(Double_t aNum) {fFileStartTime = aNum;}
  void SetFileEndTime(Double_t aNum) {fFileEndTime = aNum;}
  
private:
  Int_t fRunNumber; //run number of the Muon Veto system
  Int_t fMultiADC;  //the number of PMTs in the MuVeto system with an ADC value  
  Double_t fDistanceEst; //laser distance from the East wall to the MuVeto System
  Double_t fDistanceNemo; //laser distance from the Nemo wall to the MuVeto System
  UInt_t fEventNumber;  //the event number returned by IRQ. Note, this can be reset in the middle of a run and there are a number of jumps to very high values
  Int_t fFileNumber;  //there are up to 99 files per Run in the MuVeto System
  Int_t fMultiTDC; //the number of PMTs with a TDC value 
  Int_t fPcTimeSec; //the time in seconds (presumably since Jan 1, 1970) returned by the DAQ PC
  Int_t fPcTimeMuSec; //the microseconds elaspsed within the PC time
  Int_t fCommonStopTime; //the TDC value of the common stop signal for Muon Veto Events. 
  
  Double_t fRunStartTime; //start time, in unix time, for the beginning of the current system run
  Double_t fRunEndTime; //end time, in unix time, for the beginning of the current system run
  Double_t fFileStartTime; //start time, in unix time, for the beginning of the current "file"
  Double_t fFileEndTime; //end time, in unix time, for the beginning of the current "file"

  
  TBits fEventQuality;  //|| //the Event Quality bits. But don't split them! Otherwise, they won't be accessible from TTree::Draw

  static const Int_t kReconstructedTimeRunBoundary; //make it static so that it is not written to the tree
  
  Bool_t fSystemOn;
  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KMuonVetoSystemRecord &aRec);

  ClassDef(KMuonVetoSystemRecord,5);
};


#endif // __KMUONVETOSYSTEMRECORD_H__
