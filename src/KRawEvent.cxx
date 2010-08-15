//_____________________________________________
//
// KRawEvent.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for Raw level analysis. One typically
// begins all analysis by getting a pointer to this object from 
// the KDataReader object. By calling KDataReader::GetEntry(Int_t i)
// or KDataReader::GetNextEntry, one can loop through the KDS data
// file and examine each event separately. From this object
// one can obtain pointers to they Muon and Bolometer System Records
// GetBoloSystemRecord(void), GetMuonVetoSystemRecord()
// and to each of the event's Sub Records. The raw level data file
// should contain the bolometer pulse traces for each event.
//
// To allocate memory and get a pointer to an object of this type from the KEventFactory, 
// use the global varaibles
// const Char_t kRawEventName[] = "Raw";
// const Char_t kRawClassName[] = "KRawEvent";
//
// to call
//
// KRawEvent *rawEvent = KEventFactory::NewEvent("Raw");
// KRawEvent *rawEvent = KEventFactory::NewEvent("KRawEvent");
//  
// or, simply use
//
// KRawEvent *rawEvent = KEventFactory::NewRawEvent();
//

#include "KRawEvent.h"
#include "KHLAEvent.h"
#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(KRawEvent);

KRawEvent::KRawEvent(void)
{
	//Default constructor 
	
	InitializeMembers();

}

KRawEvent::KRawEvent(const KRawEvent &anEvent)
: KEvent(anEvent)
{
	CopyLocalMembers(anEvent);
	
}
KRawEvent& KRawEvent::operator=(const KEvent &anEvent)
{
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = base" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KEvent::operator=(anEvent);
	
	return *this;
}

KRawEvent& KRawEvent::operator=(const KRawEvent &anEvent)
{
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = raw" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KEvent::operator=(anEvent);

	CopyLocalMembers(anEvent);
	
	return *this;	
}

KRawEvent& KRawEvent::operator=(const KHLAEvent &anEvent)
{
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = hla" << endl;
#endif
	try {
		const KEvent &rhs = dynamic_cast<const KEvent&>(anEvent);
		if(&rhs == this) return *this;
		//copy the base class info.
		this->KEvent::operator=(rhs);

	}
	catch (bad_cast) {
		cout << "THROW!" << endl;
		//what should i do? this should never throw!
		throw;  
	}
	
	return *this;
}

void KRawEvent::CopyLocalMembers(const KRawEvent &/*anEvent*/)
{
	//nothing to do
}

KRawEvent::~KRawEvent(void) 
{
	//Does calling Clear at destructor take up too much computing time?
	Clear("C");
}

void KRawEvent::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
	KEvent::Clear(opt);
	
	//Clear and delete local objects here. 
	InitializeMembers();
}

void KRawEvent::InitializeMembers(void)
{
	
}

Bool_t KRawEvent::IsSame(const KRawEvent &anEvent, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KEvent::IsSame(anEvent,bPrint)){
		if (bPrint) 
			bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	
	return bIsEqual;
}


void KRawEvent::Compact(void)
{
	//Make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KEvent::Compact();
}

	
