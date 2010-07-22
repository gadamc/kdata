//_____________________________________________
//
// EdwEventBase.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/3/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the "mother" of all EdwDS Event classes. It contains some basic
// event parameters. Most importantly, however, is that many analysis objects
// can exploit C++ polymorphism by writing code that pass a pointer of
// this type (instead of a pointer to EdwHLAEvent or EdwRawEvent) and then
// handling the event pointer appropriately using dynamic casting. 
// 
// 

#include "EdwEventBase.h"
#include <iostream>

using namespace std;

ClassImp(EdwEventBase);

EdwEventBase::EdwEventBase(void) 
{
	//
	
	//note from the ROOT documentation
/*	The Default Constructor
	ROOT object I/O requires every class to have either a default constructor or an I/O constructor. 
 A default constructor is a constructor with zero parameters or with one or more parameters all with default values. 
 An I/O constructor is a constructor with exactly one parameter which type is a pointer to one of the type 
 marked as an 'io constructor type'. We will come back to this context in a few paragraphs. This default or I/O 
 constructor is called whenever an object is being read from a ROOT database. Be sure that you do not allocate 
 any space for embedded pointer objects in this constructor. This space will be lost (memory leak) while reading 
 in the object. 
 
 And, I broke that rule in the EdwHLAEvent because I allocate memory for the TClonesArrays.
 */
	InitializeMembers();

}

EdwEventBase::EdwEventBase(const EdwEventBase &anEvent)
:TObject()
{
	CopyLocalMembers(anEvent);
	
}

EdwEventBase& EdwEventBase::operator=(const EdwEventBase &anEvent)
{
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "base = base" << endl;
#endif
	if(&anEvent == this) return *this;
	
	CopyLocalMembers(anEvent);
	
	return *this;
}

void EdwEventBase::CopyLocalMembers(const EdwEventBase &anEvent)
{
	SetRunNumber(anEvent.GetRunNumber());
	SetEventTriggerTime(anEvent.GetEventTriggerTime()); 
	SetStamp(anEvent.GetStamp());
	SetRunStartTime(anEvent.GetRunStartTime());
	SetRunEndTime(anEvent.GetRunEndTime());
	SetTriggerType(anEvent.GetTriggerType()); 
	SetDataType(anEvent.GetDataType());
	SetDetectorStatusWord(anEvent.GetDetectorStatusWord());
	SetBlindnessWord(anEvent.GetBlindnessWord());
	SetGSEventNumber(anEvent.GetGSEventNumber());	
	
}

EdwEventBase::~EdwEventBase(void) 
{
	Clear("C");
}

void EdwEventBase::Clear(Option_t * /*anOption*/)
{
	
	//delete any memory allocated by the EdwEvent class here that
	//needs to be deleted for each event -- but not the TClonesArrays. Clear should be called 
	//after every event so that the TClonesArrays are cleared. 
	
	InitializeMembers();
}


void EdwEventBase::InitializeMembers(void)	
{
	//WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
	//ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetRunNumber(-99);
	SetEventTriggerTime(-99.0); 
	SetStamp(-99);
	SetRunStartTime(-99.0);
	SetRunEndTime(-99.0);
	SetTriggerType(0); //initialize this to zero, instead of -99 so that AddTriggerType works.
	SetDataType(-99);
	SetDetectorStatusWord(-99);
	SetBlindnessWord(-99);
	SetGSEventNumber(-99);	
}

Bool_t EdwEventBase::operator==(const EdwEventBase &anEvent) const
{
	//returns true of both events are EXACTLY the same
	//this is most useful as a test of the copy constructors
	return IsSame(anEvent,false);
}

Bool_t EdwEventBase::IsSame(const EdwEventBase &anEvent, Bool_t bPrint) const
{
	//same as operator==, but with the option to print out whenever it finds
	//a difference.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences


	if(fRunNumber != anEvent.fRunNumber){
		if (bPrint) 
			cout << "EdwEventBase fRunNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fRunStartTime != anEvent.fRunStartTime){
		if (bPrint) 
			cout << "EdwEventBase fRunStartTime Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fRunEndTime != anEvent.fRunEndTime){
		if (bPrint) 
			cout << "EdwEventBase fRunEndTime Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fEventTriggerTime != anEvent.fEventTriggerTime){
		if (bPrint) 
			cout << "EdwEventBase fEventTriggerTime Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fStamp != anEvent.fStamp){
		if (bPrint) 
			cout << "EdwEventBase fStamp Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fTriggerType != anEvent.fTriggerType){
		if (bPrint) 
			cout << "EdwEventBase fTriggerType Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fDataType != anEvent.fDataType){
		if (bPrint) 
			cout << "EdwEventBase fDataType Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fDetectorStatusWord != anEvent.fDetectorStatusWord){
		if (bPrint) 
			cout << "EdwEventBase fDetectorStatusWord Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fBlindnessWord != anEvent.fBlindnessWord){
		if (bPrint) 
			cout << "EdwEventBase fBlindnessWord Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fGSEventNumber != anEvent.fGSEventNumber){
		if (bPrint) 
			cout << "EdwEventBase fGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	return bIsEqual;
}

Bool_t EdwEventBase::IsBlind(void) const 
{
	//returns true if you are officially Blind by this event. 
	//If this returns true, then you should not analyze this event.
	
	//check the fBlindnessWord for a particular bit pattern
	return false;

}  

Double_t EdwEventBase::GetSecPerStamp(void) const
{
	//returns the number of seconds that each Opera stamp spans.
	//Currently, each stamp value is 10 microseconds long. 
	return 1.e-5;
}

Double_t EdwEventBase::GetStampTime(void) const
{
	//returns the stamp time in seconds.  
	//return GetSecPerStamp() * GetStamp();
	
	return GetSecPerStamp() * GetStamp();
}

void EdwEventBase::myPrintB() const
{
	//prints out a number of data member variables to std out. 
	//this method needs some work. 
	
	//cout << "fNumSambas : fNumSingleBolos : fNumBoloPulses : fNumMuonModules"<<endl;
	//cout << fNumSambas  <<" : " << fNumSingleBolos <<" : " << fNumBoloPulses <<" : " << fNumMuonModules << endl;
	std::cout <<  "fRunNumber : fStamp : fTriggerType : fDataType : fDetectorStatusWord : fBlindnessWord : fGSEventNumber" << endl;
	cout << fRunNumber  <<" : " << fStamp <<" : " << fTriggerType <<" : " << fDataType <<" : " <<fDetectorStatusWord <<" : " << fBlindnessWord <<" : " << fGSEventNumber  << endl;
	
}

/* settled for one complete stamp
 Double_t EdwEventBase::GetTimeStamp(void)
 {
 return fGigaStamp*1.0e9 + fStamp;
 }*/
