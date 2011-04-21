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
#include "KClonesArray.h"

//#include <time>

using namespace std;

//class TObject;
//class KEventFactory;


class KHLAEvent;
class KRawBoloPulseRecord;
class KRawSambaRecord;
class KSambaRecord;
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
  static const char* GetClassName() {return "KRawEvent";}
  
	virtual KEvent& operator=(const KEvent &anEvent);
	virtual KRawEvent& operator=(const KRawEvent &anEvent);
	
  Int_t AddSubRecords(const KRawEvent &anEvent);
  Bool_t AddMuonModuleSubRecord(const KRawMuonModuleRecord &inMuonModule);
  Bool_t AddBoloSubRecord(const KRawBolometerRecord &inBolo);
  
  //Getters and Setters to member variables
	virtual Int_t GetNumSambas(void) const {return (fSamba) ? fSamba->GetEntriesFast() : 0;}
	Int_t GetNumBolos(void) const {return (fBolo) ? fBolo->GetEntriesFast() : 0;;}
	Int_t GetNumBoloPulses(void) const {return (fBoloPulse) ? fBoloPulse->GetEntriesFast() : 0;} 
	Int_t GetNumMuonModules(void) const {return (fMuonModule) ? fMuonModule->GetEntriesFast() : 0;} 
  
  KRawMuonVetoSysRecord* GetMuonVetoSystemRecord(void) {return &fMuonSystem;}

  virtual KSambaRecord* GetSamba(Int_t i) const;
	KRawBolometerRecord* GetBolo(Int_t i) const;
	KRawBoloPulseRecord* GetBoloPulse(Int_t i) const;
	KRawMuonModuleRecord* GetMuonModule(Int_t i) const;
  
  const KClonesArray* GetSambaRecords(void) const {return static_cast<KClonesArray *>(fSamba);}
	const KClonesArray* GetBoloRecords(void) const {return static_cast<KClonesArray *>(fBolo);}
	const KClonesArray* GetBoloPulseRecords(void) const {return static_cast<KClonesArray *>(fBoloPulse);}
	const KClonesArray* GetMuonModuleRecords(void) const {return static_cast<KClonesArray *>(fMuonModule);}
  
  KRawSambaRecord* AddSamba();
	KRawBolometerRecord* AddBolo();
	KRawBoloPulseRecord* AddBoloPulse();
	KRawMuonModuleRecord* AddMuonModule();
  
  
private: 
	
  KRawMuonVetoSysRecord fMuonSystem; //Muon Veto System Record

  //the '//->' comment tells ROOT that this pointer will never be NULL
  
  TClonesArray *fSamba; //->  an array of samba records
  TClonesArray *fBolo; //-> an array of bolometer records
  TClonesArray *fBoloPulse; //-> an array of pulse record
  TClonesArray *fMuonModule; //-> an array of muon module records
  
  void CreateArrays(void);
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawEvent &anEvent);
  void CopyClonesArrays(const KRawEvent &anEvent);
  
  template<class T> T* AddSubRecord(TClonesArray *mArray);
  void DeleteArray(Option_t *anOpt, TClonesArray *mArray);
  void ClearArray(Option_t *anOpt, TClonesArray *mArray);
  
  UInt_t GetLargestUniqueIDNumber(void);

	ClassDef(KRawEvent ,1);
};



#endif // __KRAWEVENT_H__
