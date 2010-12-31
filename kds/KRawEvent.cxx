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
#include "KRawBoloPulseRecord.h"
#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(KRawEvent);

KRawEvent::KRawEvent(void)
{
	//Default constructor 
	
  fNumBoloPulse = 0;
  fBoloPulse = 0;
  
	InitializeMembers();
  CreateArrays();

}

KRawEvent::KRawEvent(const KRawEvent &anEvent)
: KEvent(anEvent)
{
  //copy constructor
  
  fNumBoloPulse = 0;
  fBoloPulse = 0;
	CopyLocalMembers(anEvent);
  CreateArrays();
  CopyClonesArrays(anEvent);

}


KRawEvent::~KRawEvent(void) 
{
  //destructor 
  
	//Does calling Clear at destructor take up too much computing time?
	Clear("C");
  
  if(fBoloPulse) delete fBoloPulse;
	fBoloPulse = 0;
}


KRawEvent& KRawEvent::operator=(const KEvent &anEvent)
{
  //assignment operator
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = base" << endl;
#endif
	if(&anEvent == this) return *this;

  try {
		const KRawEvent &rhs = dynamic_cast<const KRawEvent&>(anEvent);
		this->operator=(rhs);
		
	}
	catch(bad_cast) {
		
		try{
			const KHLAEvent &rhs = dynamic_cast<const KHLAEvent&>(anEvent);
			this->operator=(rhs);
		}
		
		catch(bad_cast) {
			this->KEvent::operator=(anEvent);
		}
	}
	
	return *this;
}

KRawEvent& KRawEvent::operator=(const KRawEvent &anEvent)
{
  //assignment operator
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = raw" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KEvent::operator=(anEvent);

	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);
  
	return *this;	
}

KRawEvent& KRawEvent::operator=(const KHLAEvent &anEvent)
{
  //assignment operator - only assigns the common data found in the 
  //base class.
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = hla" << endl;
#endif
	try {
		const KEvent &rhs = dynamic_cast<const KEvent&>(anEvent);
		if(&rhs == this) return *this;
		//copy the base class info.
		this->KEvent::operator=(rhs);
    //CopyFromHlaEvent(anEvent);
	}
	catch (bad_cast) {
		cout << "THROW!" << endl;
		//what should i do? this should never throw!
		throw;  
	}
	
	return *this;
}

void KRawEvent::CopyLocalMembers(const KRawEvent &anEvent)
{
  //copies local data members
  
  fRunName = anEvent.fRunName; 
	fSambaEventNumber = anEvent.fSambaEventNumber;
	fDateSec = anEvent.fDateSec;
	fDateMuSec = anEvent.fDateMuSec;
	fSambaDelay = anEvent.fSambaDelay;
	fTriggerBit1 = anEvent.fTriggerBit1;
	fTriggerBit2 = anEvent.fTriggerBit2;
	fNumBoloPulse = anEvent.fNumBoloPulse;
}

Int_t KRawEvent::AddSubRecords(const KRawEvent &anEvent)
{
  //add all of the subrecord in anEvent to this.
  //returns the number of records added
  
  Int_t numRecords = 0;
  
  for(Int_t i = 0; i < anEvent.fNumBoloPulse; i++){
    KRawBoloPulseRecord *pulse0 = anEvent.GetBoloPulse(i);
    KRawBoloPulseRecord *pulse = AddBoloPulse();
    numRecords++;
    if(pulse0 != 0 && pulse != 0){
      *pulse = *pulse0;
    }
  }
  
  return numRecords;
}

void KRawEvent::CopyClonesArrays(const KRawEvent &anEvent)
{
  ClearArrays("C");
  AddSubRecords(anEvent);
}

void KRawEvent::ClearArrays(Option_t *opt)
{
  DeleteArray(opt, fBoloPulse, fNumBoloPulse); // have to call delete because the pulse contains a string
}

void KRawEvent::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
  ClearArrays(opt);
  
	KEvent::Clear(opt);
	
	//Clear and delete local objects here. 
	InitializeMembers();
}

void KRawEvent::InitializeMembers(void)
{
	fRunName = "";
  fSambaEventNumber = -99;
  fDateSec = -99;
  fDateMuSec = -99;
  fSambaDelay = -99;
  fTriggerBit1 = 0;
  fTriggerBit2 = 0;
}

void KRawEvent::CreateArrays(void)
{
  //Allocates memory for the TClonesArrays if they haven't already
	//been allocated.
	
  if(!fBoloPulse)
		fBoloPulse = new TClonesArray("KRawBoloPulseRecord",20);
  
}

template<class T> T* KRawEvent::AddSubRecord(TClonesArray *mArray, Int_t &mCount)
{
	TClonesArray &mArrayRef = *mArray;
	T* mNewSubRecord = new(mArrayRef[mCount++]) T;
	return mNewSubRecord;
}

void KRawEvent::DeleteArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		//we have to delete because our sub-records contain TString members! :(
		mArray->Delete( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}


void KRawEvent::ClearArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		mArray->Clear( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}


KRawBoloPulseRecord* KRawEvent::AddBoloPulse(void)
{
  //Use this event only when creating an event and you want to add
	//a new SubRecord.
  
  AddTriggerType(kBoloTriggerType);
  return AddSubRecord<KRawBoloPulseRecord>(fBoloPulse,fNumBoloPulse);
}

KRawBoloPulseRecord *KRawEvent::GetBoloPulse(Int_t i) const
{
  // Return the i'th Bolometer Pulse Sub Record for this event.
	
  KRawBoloPulseRecord *ms = 0;
  if (i < fNumBoloPulse && i >= 0) ms = (KRawBoloPulseRecord *)fBoloPulse->At(i);
  return ms;
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

	
