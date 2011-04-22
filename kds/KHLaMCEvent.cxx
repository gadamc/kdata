//_____________________________________________
//
// KHLaMCEvent.cxx
// KDataStructure
//
// Author: Alexander Wunderle @ KIT
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for HLA MC Event.
//

#include "KHLaMCEvent.h"
#include "KHLAEvent.h"
#include "TClonesArray.h"
#include "KMCBolometerRecord.h"
#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(KHLaMCEvent);

KHLaMCEvent::KHLaMCEvent(void)
{
	InitializeMembers();
	
	fNumBolo = 0;
	fBolo = 0;
	CreateArrays();

}

KHLaMCEvent::KHLaMCEvent(const KHLaMCEvent &anEvent)
	: KEvent(anEvent)
{
	CopyLocalMembers(anEvent);
	
	CopyClonesArrays(anEvent);
	
}

KHLaMCEvent& KHLaMCEvent::operator=(const KEvent &anEvent)
{
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "HLaMC = base" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KEvent::operator=(anEvent);
	
	return *this;
}

KHLaMCEvent& KHLaMCEvent::operator=(const KHLaMCEvent &anEvent)
{
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "HLaMC = HLaMC" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KHLaMCEvent::operator=(anEvent);

	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);
	return *this;	
}

void KHLaMCEvent::CopyLocalMembers(const KHLaMCEvent &anEvent)
{
	fNumBolo = anEvent.fNumBolo;

}

KHLaMCEvent::~KHLaMCEvent(void) 
{
  //destructor 
  
	//Does calling Clear at destructor take up too much computing time?
	Clear("C");
	
	if(fBolo) delete fBolo;
	fBolo = 0;
}

void KHLaMCEvent::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
  
	ClearArray(opt, fBolo,fNumBolo);
	
	KEvent::Clear(opt);
	
	//Clear and delete local objects here. 
	InitializeMembers();
}

void KHLaMCEvent::InitializeMembers(void)
{
	//init local members. does nothing.
}

Bool_t KHLaMCEvent::IsSame(const KHLaMCEvent &anEvent, Bool_t bPrint) const
{
	//Compares two KHLaMCEvent and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KHLaMCEvent::IsSame(anEvent,bPrint)){
		if (bPrint) 
			bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fNumBolo == anEvent.fNumBolo){
		for(Int_t i = 0; i < fNumBolo; i++){
			KMCBolometerRecord *s = static_cast<KMCBolometerRecord *>(GetBolo(i));
			KMCBolometerRecord *sOther = static_cast<KMCBolometerRecord *>(anEvent.GetBolo(i));
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "KHLaMCEvent KMCBolometerRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "KHLaMCEvent fNumBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	return bIsEqual;
}

KBolometerRecord *KHLaMCEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.
	KMCBolometerRecord *ms = 0;
	if (i < fNumBolo) ms = static_cast<KMCBolometerRecord *>(fBolo->At(i));
	return ms;
}

KMCBolometerRecord* KHLaMCEvent::AddBolo(void)
{
	AddTriggerType(kBoloTriggerType);
	return AddSubRecord<KMCBolometerRecord>(fBolo,fNumBolo);
}

void KHLaMCEvent::ClearArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		mArray->Clear( (anOption && *anOption) ? anOption : "C" );
		//we have to delete because our sub-records contain TString members! :(
		//mArray->Delete( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}

template<class T> T* KHLaMCEvent::AddSubRecord(TClonesArray *mArray, Int_t &mCount)
{
	TClonesArray &mArrayRef = *mArray;
	T* mNewSubRecord = new(mArrayRef[mCount++]) T;
	return mNewSubRecord;
}

void KHLaMCEvent::CopyClonesArrays(const KHLaMCEvent &anEvent)
{
	//ClonesArray assignment doesn't appear to work in the following way
	//*fBolo = *anEvent.GetBoloSubRecords();
	
	//so, I just clear this object's array, and create as many objects as I need.
	//this might be inefficient, but I don't think so. besides,
	//we likely won't be creating many many instances of the Event class.
	//at least, we shouldn't do that. 
	//if your code is really slow and you think it has something
	//to do with copying HLAEvents, look here for possible fixes. 
	
	ClearArray("C", fBolo,fNumBolo);
	
	
	for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
		KMCBolometerRecord *s = AddBolo();
		KMCBolometerRecord *sO = static_cast<KMCBolometerRecord *>(anEvent.GetBolo(i));
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cout << "KHLaMCEvent::operator= Invalid SingleBolo Pointer" << endl;
	}
	
}

void KHLaMCEvent::CreateArrays(void)
{
	if(!fBolo)
		fBolo = new TClonesArray("KMCBolometerRecord",2);
	
}

	
