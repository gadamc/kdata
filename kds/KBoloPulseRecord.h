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

#include "KSubRecord.h"
#include <string>
using namespace std;

class KBoloPulseRecord : public KSubRecord { 

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

  Double32_t GetPositiveTriggerAmp(void) const { return fPositiveTriggerAmp;}
  Double32_t GetNegativeTriggerAmp(void) const { return fNegativeTriggerAmp;}
  const char* GetState(void) const { return fState.c_str();}
  Double32_t GetPolarity(void) const { return fPolarity;}
  Double32_t GetGain(void) const { return fGain;}
  
  Double32_t GetBoloBoxVersion(void) const {return fBoloBoxVersion;}
  Int_t GetConvergencePeriod(void) const {return fConvergencePeriod;}
  const char* GetRelay1Status(void) const {return fRelay1Status.c_str();}
  const char* GetRelay2Status(void) const {return fRelay2Status.c_str();}
  Int_t GetFetDac(void) const {return fFetDac;}

  void SetPositiveTriggerAmp(Double32_t aVal) {fPositiveTriggerAmp = aVal;}
  void SetNegativeTriggerAmp(Double32_t aVal) {fNegativeTriggerAmp = aVal;}
  void SetState(const char* aVal) {fState = aVal;}
  void SetPolarity(Double32_t aVal) {fPolarity = aVal;}
  void SetGain(Double32_t aVal) {fGain = aVal;}

  void SetBoloBoxVersion(Double32_t aVal) {fBoloBoxVersion = aVal;}
  void SetConvergencePeriod(Int_t aVal) {fConvergencePeriod = aVal;}
  void SetRelay1Status(const char* aVal) {fRelay1Status = aVal;}
  void SetRelay2Status(const char* aVal) {fRelay2Status = aVal;}
  void SetFetDac(Int_t aVal) {fFetDac = aVal;}

private:

  Double32_t fPositiveTriggerAmp;
  Double32_t fNegativeTriggerAmp;
  string fState;
  Double32_t fPolarity;
  Double32_t fGain;

  //bolo box information
  Double32_t fBoloBoxVersion;
  Int_t fConvergencePeriod;
  string fRelay1Status;
  string fRelay2Status;
  Int_t fFetDac;

  void InitializeMembers(void);
  void CopyLocalMembers(const KBoloPulseRecord &aRec);

  ClassDef(KBoloPulseRecord,6);
};


#endif // __KBOLOPULSERECORD_H__

