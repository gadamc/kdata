/*
 *  KRawEvent.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/24/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWEVENT_H__
#define __KRAWEVENT_H__

#include "KEvent.h"
#include <string>
#include "TClonesArray.h"

//#include <time>

using namespace std;

//class TObject;
//class KEventFactory;

const Char_t kRawEventName[] = "Raw";
const Char_t kRawClassName[] = "KRawEvent";

class KHLAEvent;
class KRawBoloPulseRecord;

class KRawEvent : public KEvent {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	KRawEvent(void);
	KRawEvent(const KRawEvent &anEvent);
	virtual ~KRawEvent(void);
	KRawEvent& operator=(const KEvent &anEvent);
	KRawEvent& operator=(const KHLAEvent &anEvent);
	KRawEvent& operator=(const KRawEvent &anEvent);
	virtual void Clear(Option_t *opt = "C");
  void ClearArrays(Option_t *opt = "C");
	Bool_t IsSame(const KRawEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const KRawEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const KRawEvent &anEvent) const { return !(*this==anEvent); }
  Int_t AddSubRecords(const KRawEvent &anEvent);

	virtual void Compact(void);
  
  KRawBoloPulseRecord* GetBoloPulse(Int_t i) const;
  KRawBoloPulseRecord* AddBoloPulse(void);

  
  string fRunName;
  Int_t fSambaEventNumber;
  Int_t fDateSec;
  Int_t fDateMuSec;
  Double32_t fSambaDelay;
  UInt_t fTriggerBit1;
  UInt_t fTriggerBit2;
  
  Int_t fNumBoloPulse;
  TClonesArray* fBoloPulse; //->
  TClonesArray* GetBoloPulseRecords(void) const {return fBoloPulse;}
  
private: 
	
  
  
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawEvent &anEvent);
  void CopyClonesArrays(const KRawEvent &anEvent);
  void CreateArrays(void);

  template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
  void DeleteArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
  void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
  
	ClassDef(KRawEvent ,1);
};



#endif // __KRAWEVENT_H__
