//_____________________________________________
//
// EdwHLaMCEvent.cc
// EdwDataStructure
//
// Author: Alexander Wunderle @ KIT
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for HLA MC Event.
//

#include "EdwHLaMCEvent.h"
#include "EdwHLAEvent.h"
#include "TClonesArray.h"
#include "EdwMCSingleBoloSubRecord.h"
#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(EdwHLaMCEvent);

EdwHLaMCEvent::EdwHLaMCEvent(void)
{
	InitializeMembers();
	
	fNumBolo = 0;
	fBolo = 0;
	CreateArrays();

}

EdwHLaMCEvent::EdwHLaMCEvent(const EdwHLaMCEvent &anEvent)
	: EdwEventBase(anEvent)
{
	CopyLocalMembers(anEvent);
	
	CopyClonesArrays(anEvent);
	
}

EdwHLaMCEvent& EdwHLaMCEvent::operator=(const EdwEventBase &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "HLaMC = base" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->EdwEventBase::operator=(anEvent);
	
	return *this;
}

EdwHLaMCEvent& EdwHLaMCEvent::operator=(const EdwHLaMCEvent &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "HLaMC = HLaMC" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->EdwHLaMCEvent::operator=(anEvent);

	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);
	return *this;	
}

void EdwHLaMCEvent::CopyLocalMembers(const EdwHLaMCEvent &anEvent)
{
	fNumBolo = anEvent.fNumBolo;

}

EdwHLaMCEvent::~EdwHLaMCEvent(void) 
{
	//Does calling Clear at destructor take up too much computing time?
	Clear("C");
	
	if(fBolo) delete fBolo;
	fBolo = 0;
}

void EdwHLaMCEvent::Clear(Option_t *opt)
{
	ClearArray(opt, fBolo,fNumBolo);
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
	EdwEventBase::Clear(opt);
	
	//Clear and delete local objects here. 
	InitializeMembers();
}

void EdwHLaMCEvent::InitializeMembers(void)
{
	
}

Bool_t EdwHLaMCEvent::IsSame(const EdwHLaMCEvent &anEvent, Bool_t bPrint) const
{
	//Compares two EdwHLaMCEvent and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwHLaMCEvent::IsSame(anEvent,bPrint)){
		if (bPrint) 
			bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fNumBolo == anEvent.fNumBolo){
		for(Int_t i = 0; i < fNumBolo; i++){
			EdwMCSingleBoloSubRecord *s = GetBolo(i);
			EdwMCSingleBoloSubRecord *sOther = anEvent.GetBolo(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "EdwHLaMCEvent EdwMCSingleBoloSubRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "EdwHLaMCEvent fNumBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	return bIsEqual;
}

EdwMCSingleBoloSubRecord *EdwHLaMCEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.
	EdwMCSingleBoloSubRecord *ms = 0;
	if (i < fNumBolo) ms = (EdwMCSingleBoloSubRecord *)fBolo->At(i);
	return ms;
}

EdwMCSingleBoloSubRecord* EdwHLaMCEvent::AddBolo(void)
{
	AddTriggerType(kBoloTriggerType);
	return AddSubRecord<EdwMCSingleBoloSubRecord>(fBolo,fNumBolo);
}

void EdwHLaMCEvent::ClearArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		mArray->Clear( (anOption && *anOption) ? anOption : "C" );
		//we have to delete because our sub-records contain TString members! :(
		//mArray->Delete( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}

template<class T> T* EdwHLaMCEvent::AddSubRecord(TClonesArray *mArray, Int_t &mCount)
{
	TClonesArray &mArrayRef = *mArray;
	T* mNewSubRecord = new(mArrayRef[mCount++]) T;
	return mNewSubRecord;
}

void EdwHLaMCEvent::CopyClonesArrays(const EdwHLaMCEvent &anEvent)
{
	//ClonesArray assignment doesn't appear to work in the following way
	//*fSamba = *anEvent.GetSambaSubRecords();
	//*fBolo = *anEvent.GetBoloSubRecords();
	//*fBoloPulse = *anEvent.GetBoloPulseSubRecords();
	//*fMuonModule = *anEvent.GetMuonModuleSubRecords();
	
	//so, I just clear this object's array, and create as many objects as I need.
	//this might be inefficient, but I don't think so. besides,
	//we likely won't be creating many many instances of the Event class.
	//at least, we shouldn't do that. 
	//if your code is really slow and you think it has something
	//to do with copying HLAEvents, look here for possible fixes. 
	
	ClearArray("C", fBolo,fNumBolo);
	
	
	for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
		EdwMCSingleBoloSubRecord *s = AddBolo();
		EdwMCSingleBoloSubRecord *sO = anEvent.GetBolo(i);
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cout << "EdwHLaMCEvent::operator= Invalid SingleBolo Pointer" << endl;
	}
	
}

void EdwHLaMCEvent::CreateArrays(void)
{
	if(!fBolo)
		fBolo = new TClonesArray("EdwMCSingleBoloSubRecord",2);
	
}

	
