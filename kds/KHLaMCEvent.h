/*
 *  KHLaMCEvent.h
 *  KDataStructure
 *
 *  Created by Alexander WUnderle on 6/1/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLAMCEVENT_H__
#define __KHLAMCEVENT_H__

#include "KEvent.h"

//class TObject;
//class KEventFactory;
class TClonesArray; 
class KMCBolometerRecord;

class KHLaMCEvent : public KEvent {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	KHLaMCEvent(void);
	KHLaMCEvent(const KHLaMCEvent &anEvent);
	virtual ~KHLaMCEvent(void);
	KHLaMCEvent& operator=(const KHLaMCEvent &anEvent);
	KHLaMCEvent& operator=(const KEvent &anEvent);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KHLaMCEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLaMCEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const KHLaMCEvent &anEvent) const { return !(*this==anEvent); }
  static const char* GetClassName() {return "KHLaMCEvent";}

	Int_t GetNumBolos(void) const {return fNumBolo;}

	void SetNumBolo(Int_t aNum){fNumBolo = aNum;}
	
  //methods required by the KEvent abstract class.

	KBolometerRecord* GetBolo(Int_t i) const;
	TClonesArray* GetBoloSubRecords(void) const {return fBolo;}
  KMCBolometerRecord* AddBolo();
	
  
  //methods required by the KEvent abstract class.
  //make them private so that the user can't access them. 
	virtual KSambaRecord* GetSamba(Int_t ) const {KSambaRecord *s = 0; return s;}
  virtual KBoloPulseRecord* GetBoloPulse(Int_t ) const {KBoloPulseRecord *s = 0; return s;}
  virtual KMuonModuleRecord* GetMuonModule(Int_t ) const {KMuonModuleRecord *s = 0; return s;}
  
  virtual Int_t GetNumSambas(void) const {return 0;}
  virtual Int_t GetNumBoloPulses(void) const {return 0;}
  virtual Int_t GetNumMuonModules(void) const {return 0;}
  
  virtual Int_t AddSubRecords(const KEvent& ){ return 0;  };
  virtual Bool_t AddMuonModuleSubRecord(const KMuonModuleRecord& ){ return 0;  };
  virtual Bool_t AddBoloSubRecord(const KBolometerRecord& ){ return 0;  };
  
  virtual KMuonVetoSystemRecord* GetMuonVetoSystemRecord(void)  {KMuonVetoSystemRecord *s = 0; return s;}
  
  
private: 
	Int_t fNumBolo;  //the number of KMCBolometerRecord in this event. 
	
	TClonesArray *fBolo; //->  pointer to an array of KMCBolometerRecord found in this event.
	
	template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
	void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
	void CreateArrays(void);
	void InitializeMembers(void);
	void CopyClonesArrays(const KHLaMCEvent &anEvent);
	void CopyLocalMembers(const KHLaMCEvent &anEvent);

  
  
	ClassDef(KHLaMCEvent ,1);
};



#endif // __KHLAMCEVENT_H__
