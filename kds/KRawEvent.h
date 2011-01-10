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
#include "KRawMuonVetoSysRecord.h"
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
class KRawSambaRecord;
class KRawBolometerRecord;
class KRawMuonModuleRecord;

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
  virtual void Clear(Option_t *opt = "C");
  void ClearArrays(Option_t *opt = "C");
  Bool_t IsSame(const KRawEvent &anEvent, Bool_t bPrint = false) const;
  Bool_t operator==(const KRawEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const KRawEvent &anEvent) const { return !(*this==anEvent); }
	virtual void Compact(void);

	KRawEvent& operator=(const KEvent &anEvent);
	KRawEvent& operator=(const KHLAEvent &anEvent);
	KRawEvent& operator=(const KRawEvent &anEvent);
	
  Int_t AddSubRecords(const KRawEvent &anEvent);

  //Getters and Setters to member variables
	Int_t GetNumSambas(void) const {return fNumSamba;}
	Int_t GetNumBolos(void) const {return fNumBolo;}
	Int_t GetNumBoloPulses(void) const {return fNumBoloPulse;} 
	Int_t GetNumMuonModules(void) const {return fNumMuonModule;} 
  
  KRawMuonVetoSysRecord* GetMuonVetoSystemRecord(void) {return &fMuonSystem;}

  KRawSambaRecord* GetSamba(Int_t i) const;
	KRawBolometerRecord* GetBolo(Int_t i) const;
	KRawBoloPulseRecord* GetBoloPulse(Int_t i) const;
	KRawMuonModuleRecord* GetMuonModule(Int_t i) const;
  
  TClonesArray* GetSambaRecords(void) const {return fSamba;}
	TClonesArray* GetBoloRecords(void) const {return fBolo;}
	TClonesArray* GetBoloPulseRecords(void) const {return fBoloPulse;}
	TClonesArray* GetMuonModuleRecords(void) const {return fMuonModule;}
  
  KRawSambaRecord* AddSamba();
	KRawBolometerRecord* AddBolo();
	KRawBoloPulseRecord* AddBoloPulse();
	KRawMuonModuleRecord* AddMuonModule();
  
  
  /* to be moved into arrays...
   
   
  void SetRunName(const char* name){ fRunName = name;  }
  void SetSambaEventNumber(UInt_t num) { fSambaEventNumber = num;  }
  
  //Raw Header Information
  //need to make these private and write getters/setters.
  //and make sure they don't overlap with the data stored 
  //in KEvent, which is propagated to the HLA level.

  string fRunName;
  UInt_t fSambaEventNumber;
  UInt_t fSambaMajorVersion;
  UInt_t fSambaMinorVersion;
  UInt_t fSambaReleaseNumber;
  time_t fDateSec;
  Int_t fDateMuSec;
  Double32_t fSambaDelay;
  UInt_t fTriggerBit1;
  UInt_t fTriggerBit2;
  
  */
  
private: 
	
  KRawMuonVetoSysRecord fMuonSystem; //Muon Veto System Record
  
  Int_t fNumSamba;
  Int_t fNumBolo;
  Int_t fNumBoloPulse;
  Int_t fNumMuonModule;

  TClonesArray *fSamba; //->
  TClonesArray *fBolo; //->
  TClonesArray *fBoloPulse; //->
  TClonesArray *fMuonModule; //->
  
  void CreateArrays(void);
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawEvent &anEvent);
  void CopyClonesArrays(const KRawEvent &anEvent);
  
  template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
  void DeleteArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
  void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
  
  UInt_t GetLargestUniqueIDNumber(void);

	ClassDef(KRawEvent ,1);
};



#endif // __KRAWEVENT_H__
