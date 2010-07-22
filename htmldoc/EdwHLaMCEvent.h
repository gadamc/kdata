/*
 *  EdwHLaMCEvent.h
 *  EdwDataStructure
 *
 *  Created by Alexander WUnderle on 6/1/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLAMCEVENT_H__
#define __EDWHLAMCEVENT_H__

#include "EdwEventBase.h"

//class TObject;
//class EdwEventFactory;
class TClonesArray; 
class EdwMCSingleBoloSubRecord;

const Char_t kHLaMCEventName[] = "HLaMC";
const Char_t kHLaMCClassName[] = "EdwHLaMCEvent";


class EdwHLaMCEvent : public EdwEventBase {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	EdwHLaMCEvent(void);
	EdwHLaMCEvent(const EdwHLaMCEvent &anEvent);
	virtual ~EdwHLaMCEvent(void);
	EdwHLaMCEvent& operator=(const EdwHLaMCEvent &anEvent);
	EdwHLaMCEvent& operator=(const EdwEventBase &anEvent);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwHLaMCEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLaMCEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const EdwHLaMCEvent &anEvent) const { return !(*this==anEvent); }
	
	Int_t GetNumBolos(void) const {return fNumBolo;}

	void SetNumBolo(Int_t aNum){fNumBolo = aNum;}
	
	EdwMCSingleBoloSubRecord* GetBolo(Int_t i) const;
	
	TClonesArray* GetBoloSubRecords(void) const {return fBolo;}
	
	EdwMCSingleBoloSubRecord* AddBolo();
	
	
	
private: 
	Int_t fNumBolo;  //the number of EdwMCSingleBoloSubRecord in this event. 
	
	TClonesArray *fBolo; //->  pointer to an array of EdwMCSingleBoloSubRecord found in this event.
	
	template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
	void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
	void CreateArrays(void);
	void InitializeMembers(void);
	void CopyClonesArrays(const EdwHLaMCEvent &anEvent);
	void CopyLocalMembers(const EdwHLaMCEvent &anEvent);

	ClassDef(EdwHLaMCEvent ,1);
};



#endif // __EDWHLAMCEVENT_H__
