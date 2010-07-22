//_____________________________________________
//
// EdwRawEvent.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for Raw level analysis. One typically
// begins all analysis by getting a pointer to this object from 
// the EdwDSReader object. By calling EdwDSReader::GetEntry(Int_t i)
// or EdwDSReader::GetNextEntry, one can loop through the EdwDS data
// file and examine each event separately. From this object
// one can obtain pointers to they Muon and Bolometer System Records
// GetBoloSystemRecord(void), GetMuonVetoSystemRecord()
// and to each of the event's Sub Records. The raw level data file
// should contain the bolometer pulse traces for each event.
//
// To allocate memory and get a pointer to an object of this type from the EdwEventFactory, 
// use the global varaibles
// const Char_t kRawEventName[] = "Raw";
// const Char_t kRawClassName[] = "EdwRawEvent";
//
// to call
//
// EdwRawEvent *rawEvent = EdwEventFactory::NewEvent("Raw");
// EdwRawEvent *rawEvent = EdwEventFactory::NewEvent("EdwRawEvent");
//  
// or, simply use
//
// EdwRawEvent *rawEvent = EdwEventFactory::NewRawEvent();
//

#include "EdwRawEvent.h"
#include "EdwHLAEvent.h"
#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(EdwRawEvent);

EdwRawEvent::EdwRawEvent(void)
{
	//Default constructor 
	
	InitializeMembers();

}

EdwRawEvent::EdwRawEvent(const EdwRawEvent &anEvent)
: EdwEventBase(anEvent)
{
	CopyLocalMembers(anEvent);
	
}
EdwRawEvent& EdwRawEvent::operator=(const EdwEventBase &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "raw = base" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->EdwEventBase::operator=(anEvent);
	
	return *this;
}

EdwRawEvent& EdwRawEvent::operator=(const EdwRawEvent &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "raw = raw" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->EdwEventBase::operator=(anEvent);

	CopyLocalMembers(anEvent);
	
	return *this;	
}

EdwRawEvent& EdwRawEvent::operator=(const EdwHLAEvent &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "raw = hla" << endl;
#endif
	try {
		const EdwEventBase &rhsBase = dynamic_cast<const EdwEventBase&>(anEvent);
		if(&rhsBase == this) return *this;
		//copy the base class info.
		this->EdwEventBase::operator=(rhsBase);

	}
	catch (bad_cast) {
		cout << "THROW!" << endl;
		//what should i do? this should never throw!
		throw;  
	}
	
	return *this;
}

void EdwRawEvent::CopyLocalMembers(const EdwRawEvent &/*anEvent*/)
{
	//nothing to do
}

EdwRawEvent::~EdwRawEvent(void) 
{
	//Does calling Clear at destructor take up too much computing time?
	Clear("C");
}

void EdwRawEvent::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
	EdwEventBase::Clear(opt);
	
	//Clear and delete local objects here. 
	InitializeMembers();
}

void EdwRawEvent::InitializeMembers(void)
{
	
}

Bool_t EdwRawEvent::IsSame(const EdwRawEvent &anEvent, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwEventBase::IsSame(anEvent,bPrint)){
		if (bPrint) 
			bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	
	return bIsEqual;
}


void EdwRawEvent::Compact(void)
{
	//Make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwEventBase::Compact();
}

	
