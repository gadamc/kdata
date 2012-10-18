/*
*  KBoloPulseRecord.h
*  KDataStructure
*
*  Created by Adam Cox on 3/25/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/
#ifndef __KBOLOPULSERECORD_H__
#define __KBOLOPULSERECORD_H__

#include "TObject.h"
#include "TRef.h"

#include <string>
using namespace std;

class KBoloPulseRecord : public TObject { 

public:
  //Constructors
  KBoloPulseRecord(void);
  KBoloPulseRecord(const KBoloPulseRecord &aRec);
  KBoloPulseRecord& operator=(const KBoloPulseRecord &aRec);
  virtual ~KBoloPulseRecord(void);
  virtual void Clear(Option_t *anOpt = "C");
  Bool_t IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KBoloPulseRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KBoloPulseRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);

  virtual const char* GetChannelName(void) const {return fChannelName.c_str();  }
  virtual Double32_t GetPositiveTriggerAmp(void) const { return fPositiveTriggerAmp;}
  virtual Double32_t GetNegativeTriggerAmp(void) const { return fNegativeTriggerAmp;}
  virtual const char* GetState(void) const { return fState.c_str();}
  virtual Double32_t GetPolarity(void) const { return fPolarity;}
  virtual Double32_t GetGain(void) const { return fGain;}
  
  virtual Double32_t GetBoloBoxVersion(void) const {return fBoloBoxVersion;}
  virtual Double32_t GetConvergencePeriod(void) const {return fConvergencePeriod;}
  virtual const char* GetRelay1Status(void) const {return fRelay1Status.c_str();}
  virtual const char* GetRelay2Status(void) const {return fRelay2Status.c_str();}
  virtual Double32_t GetFetDac(void) const {return fFetDac;}
  virtual Double32_t GetBoloGain(void) const {return fBoloGain;}

  virtual Double32_t GetPulseTimeWidth(void) const { return fPulseTimeWidth;}
  virtual UInt_t GetPretriggerSize(void) const { return fPretriggerSize;}
  virtual UInt_t GetFilterSize(void) const { return fFilterSize;}
  virtual UInt_t GetPulseLength(void) const { return fPulseLength;}
  virtual Double32_t GetHeatPulseStampWidth(void) const { return fHeatPulseStampWidth;}
  virtual Double32_t GetSampleFrequency(void) const {return 1./(GetPulseTimeWidth()*1.e-9);}
  virtual Double32_t GetNyquistFrequency(void) const { return GetSampleFrequency()/2.;}
  virtual Int_t GetCryoPosition(void) const { return fCryoPosition;}
  virtual const char* GetPolarFet(void) const { return fPolarFet.c_str();}
  virtual Double32_t GetCorrPied(void) const { return fCorrPied;}
  virtual Double32_t GetCompModul(void) const { return fCompModul;}
  virtual Double32_t GetCompModulation(void) const { return GetCompModul();}
  virtual Double32_t GetCorrTrngl(void) const { return fCorrTrngl;}
  virtual Double32_t GetAmplModul(void) const { return fAmplModul;}
  virtual Double32_t GetAmplModulation(void) const { return GetAmplModul();}
  virtual Bool_t GetIsHeatPulse(void) const { return fIsHeatPulse;}

  virtual void SetChannelName(const char* name) {fChannelName = name;  } 
  virtual void SetPositiveTriggerAmp(Double32_t aVal) {fPositiveTriggerAmp = aVal;}
  virtual void SetNegativeTriggerAmp(Double32_t aVal) {fNegativeTriggerAmp = aVal;}
  virtual void SetState(const char* aVal) {fState = aVal;}
  virtual void SetPolarity(Double32_t aVal) {fPolarity = aVal;}
  virtual void SetGain(Double32_t aVal) {fGain = aVal;}

  virtual void SetBoloBoxVersion(Double32_t aVal) {fBoloBoxVersion = aVal;}
  virtual void SetConvergencePeriod(Double32_t aVal) {fConvergencePeriod = aVal;}
  virtual void SetRelay1Status(const char* aVal) {fRelay1Status = aVal;}
  virtual void SetRelay2Status(const char* aVal) {fRelay2Status = aVal;}
  virtual void SetFetDac(Double32_t aVal) {fFetDac = aVal;}
  virtual void SetBoloGain(Double32_t aVal) {fBoloGain = aVal;}
  
  virtual void SetPulseTimeWidth(Double32_t aVal) {fPulseTimeWidth = aVal;  }
  virtual void SetPretriggerSize(UInt_t aVal) {fPretriggerSize = aVal;}
  virtual void SetFilterSize(UInt_t aVal) {fFilterSize = aVal;}
  virtual void SetPulseLength(UInt_t aVal) {fPulseLength = aVal;}
  virtual void SetHeatPulseStampWidth(Double32_t aVal) {fHeatPulseStampWidth = aVal;}
  virtual void SetCryoPosition(Int_t aVal) {fCryoPosition = aVal;}
  virtual void SetPolarFet(const char* aVal) {fPolarFet = aVal;}
  virtual void SetCorrPied(Double32_t aVal) {fCorrPied = aVal;}
  virtual void SetCompModul(Double32_t aVal) {fCompModul = aVal;}
  virtual void SetCompModulation(Double32_t aVal) {SetCompModul(aVal);}
  virtual void SetCorrTrngl(Double32_t aVal) {fCorrTrngl = aVal;}
  virtual void SetAmplModul(Double32_t aVal) {fAmplModul = aVal;}
  virtual void SetAmplModulation(Double32_t aVal) {SetAmplModul(aVal);}
  virtual void SetIsHeatPulse(Bool_t aVal) {fIsHeatPulse = aVal;}

protected:
  TRef fBolometerRecord;  //the value of this is the pointer to KBolometerRecord object that this pulse belongs to.

private:
  
  string fChannelName;  //the name of the channel. for example: "chaleur ID4"
  Double32_t fPositiveTriggerAmp;
  Double32_t fNegativeTriggerAmp;
  string fState;
  Double32_t fPolarity;
  Double32_t fGain;

  //bolo box information
  Double32_t fBoloBoxVersion;
  Double32_t fConvergencePeriod;
  string fRelay1Status;
  string fRelay2Status;
  Double32_t fFetDac;
  Double32_t fBoloGain;

  Double32_t fPulseTimeWidth; //holds the number of ns for each point.  = 1 / f. Default is 10.080 micro sec
  UInt_t fPretriggerSize;
  UInt_t fFilterSize;
  UInt_t fPulseLength;
  Double32_t fHeatPulseStampWidth;  //heat pulse only - size of the heat sample size for this bolometer in stamps (same as D2 in Samba) 
  Int_t fCryoPosition;
  string fPolarFet;
  
  //these are only valid for chaleur signals, but to keep the structure
  //to have just one type of pulse record (could change to have an ionization and heat pulse)
  //these value are included in all pulse records.... why aren't these in the Samba record?
  Double32_t fCorrPied;  //heat modulation paramter
  Double32_t fCompModul; //heat modulation compensation(?)
  Double32_t fCorrTrngl;  //heat modultion parameter
  Double32_t fAmplModul;  //amplitude of heat modulation
  Bool_t fIsHeatPulse;

  void InitializeMembers(void);
  void CopyLocalMembers(const KBoloPulseRecord &aRec);

  ClassDef(KBoloPulseRecord,9);
};


#endif // __KBOLOPULSERECORD_H__

