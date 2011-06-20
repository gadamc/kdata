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

  void SetPositiveTriggerAmp(Double32_t aVal) {fPositiveTriggerAmp = aVal;}
  void SetNegativeTriggerAmp(Double32_t aVal) {fNegativeTriggerAmp = aVal;}
  void SetState(const char* aVal) {fState = aVal;}
  void SetPolarity(Double32_t aVal) {fPolarity = aVal;}
  void SetGain(Double32_t aVal) {fGain = aVal;}

	
private:
		//private methods
  
  Double32_t fPositiveTriggerAmp;
  Double32_t fNegativeTriggerAmp;
  string fState;
  Double32_t fPolarity;
  Double32_t fGain;

  
	void InitializeMembers(void);
	void CopyLocalMembers(const KBoloPulseRecord &aRec);
	
  ClassDef(KBoloPulseRecord,5);
};


#endif // __KBOLOPULSERECORD_H__

