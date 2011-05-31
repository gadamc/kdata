//_____________________________________________
//
// KEvent.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/3/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the "mother" of all KDS Event classes. It contains some basic
// event parameters. Most importantly, however, is that many analysis objects
// can exploit C++ polymorphism by writing code that pass a pointer of
// this type (instead of a pointer to KHLAEvent or KRawEvent) and then
// handling the event pointer appropriately using dynamic casting. 
// 
// It makes sense for this class to become an abstract class in order to avoid confusion, forcing 
// the user to realize that data is stored in KRawEvent objects or KHLAEvent objects.
//
// If you are here to learn how to analyze Kdata, start with the KHLAEvent or KRawEvent
// classes, depending upon whether you're looking at "High Level" or Raw data.
//
//

#include "KEvent.h"
#include <iostream>
//#include "TClonesArray.h"

using namespace std;

ClassImp(KEvent);

KEvent::KEvent(void) 
{
	//constructor
	
	//note from the ROOT documentation
/*	The Default Constructor
	ROOT object I/O requires every class to have either a default constructor or an I/O constructor. 
 A default constructor is a constructor with zero parameters or with one or more parameters all with default values. 
 An I/O constructor is a constructor with exactly one parameter which type is a pointer to one of the type 
 marked as an 'io constructor type'. We will come back to this context in a few paragraphs. This default or I/O 
 constructor is called whenever an object is being read from a ROOT database. Be sure that you do not allocate 
 any space for embedded pointer objects in this constructor. This space will be lost (memory leak) while reading 
 in the object. 
 
 And, I broke that rule in the KHLAEvent because I allocate memory for the TClonesArrays.
 */
	InitializeMembers();

}

KEvent::KEvent(const KEvent &anEvent)
:TObject()
{
  //Copy Constructor. This only copies the KEvent variables and has no knowledge of 
  //HLA or Raw dara. However, when you're using the KRawEvent and KHLAEvent, those
  //classes have their own copy constructors.
  
	CopyLocalMembers(anEvent);
	
}

KEvent& KEvent::operator=(const KEvent &anEvent)
{
  //Assignment operator - copyies local KEvent variables only
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "base = base" << endl;
#endif
	if(&anEvent == this) return *this;
	
	CopyLocalMembers(anEvent);
	
	return *this;
}

void KEvent::CopyLocalMembers(const KEvent &anEvent)
{
	
	SetEventTriggerStamp(anEvent.GetEventTriggerStamp());
	
	SetTriggerType(anEvent.GetTriggerType()); 
	//SetDetectorStatusWord(anEvent.GetDetectorStatusWord());
	//SetBlindnessWord(anEvent.GetBlindnessWord());
	
	
}

KEvent::~KEvent(void) 
{
	Clear("C");
}

void KEvent::Clear(Option_t * /*anOption*/)
{
	//
  
	//delete any memory allocated by the KEvent class here that
	//needs to be deleted for each event -- but not the TClonesArrays. Clear should be called 
	//after every event so that the TClonesArrays are cleared. 
	
	InitializeMembers();
}


void KEvent::InitializeMembers(void)	
{
  //initializes local data members
  
	//WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
	//ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetEventTriggerStamp(-99);
	
	SetTriggerType(0); //initialize this to zero, instead of -99 so that AddTriggerType works.
	//SetDetectorStatusWord(-99);
	//SetBlindnessWord(-99);
		
}

Bool_t KEvent::operator==(const KEvent &anEvent) const
{
	//returns true of both events are EXACTLY the same
	//this is most useful as a test of the copy constructors
	return IsSame(anEvent,false);
}

Bool_t KEvent::IsSame(const KEvent &anEvent, Bool_t bPrint) const
{
	//same as operator==, but with the option to print out whenever it finds
	//a difference.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	if(fEventTriggerStamp != anEvent.fEventTriggerStamp){
		if (bPrint) 
			cout << "KEvent fEventTriggerStamp Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fTriggerType != anEvent.fTriggerType){
		if (bPrint) 
			cout << "KEvent fTriggerType Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	
	/*if(fDetectorStatusWord != anEvent.fDetectorStatusWord){
		if (bPrint) 
			cout << "KEvent fDetectorStatusWord Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}*/
	
	/*if(fBlindnessWord != anEvent.fBlindnessWord){
		if (bPrint) 
			cout << "KEvent fBlindnessWord Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}*/
		
	return bIsEqual;
}


Bool_t KEvent::IsBlind(void) const 
{
	//returns true if you are officially Blind by this event. 
	//If this returns true, then you should not analyze this event.
	//
  //currently, we do not have a blindness scheme for Edelweiss
  //so this method will always return false.
  
	//check the fBlindnessWord for a particular bit pattern
	return false;

}  

Double_t KEvent::GetSecPerStamp(void) const
{
	//returns the number of seconds that each Opera stamp spans.
	//Currently, each stamp value is 10 microseconds long. 
	//Actually there is an .8% deviation from 10 microseconds due to 
	//6 MHz base frequency in the Super Cluzel which is reduced to 
	//to a 10.08? microsends period length. This seems to be a reduction of 65 
	//cycles to one. A redcution of 60 cycles to one should be much closer to 
	//10 microseconds in fact 1.00000000000000008e-05 s long.
	return 1.e-5;
}

Double_t KEvent::GetStampTime(void) const
{
	//returns the stamp time in seconds.  
	//return GetSecPerStamp() * GetEventTriggerStamp();
	
	return GetSecPerStamp() * GetEventTriggerStamp();
}
