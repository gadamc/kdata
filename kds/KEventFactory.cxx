//_____________________________________________
//
// KEventFactory.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/11/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is an 'low-level' class and probably won't be useful to standard
// users and those doing data analysis. This factory class is used mostly 
// by KEvent developers. 
//
// Ideally, one would use this class whenever they want to create
// some type of Event class that derives from KEvent. When 
// an event class is created that derives from KEvent, the author
// must make sure to declare GetClassName method 
// their class. See the KHLAEvent and KRawEvent source files for 
// an example. 
//
// To use this class, one simply asks the object for a new event of 
// whatever type of even is needed. Also, the methods which produce
// a new event are static methods, which means an instance of a KEventFactory
// is not needed to use the method. 
//
// For example, 
//
// KHLAEvent *myEvent == KEventFactory::NewHLAEvent();
//
// KDS Developer's Note: If you want the KDataReader and KDataWriter classes
// to recognize your KEvent-based class, after you derive from KEvent, 
// you must implement your class here. The KHLaMCEvent class (along with 
// KHLAEvent and KRawEvent) is an example of this. 
// The KDataReader and KDataWriter use this class to generate 
// instances of objects needed for reading and writing the data. 

#include "KEventFactory.h"
#include "KHLAEvent.h"
#include "KRawEvent.h"
#include "KHLaMCEvent.h"
#include "TClass.h"
//#include "KMCEvent.h"

#include <iostream>

using namespace std;

ClassImp(KEventFactory);

KEventFactory::KEventFactory(void)
{

}

KEventFactory::~KEventFactory(void)
{

}


KEvent* KEventFactory::NewEvent(const Char_t* type) 
{
  //Allocates memory for a new KEvent of a particular 'type'.
  //Use KEvent::Class()
  // For example, 
  // KHLAEvent *e = KEventFactory::NewEvent( KHLAEvent::GetClassName() );
  //
  //Most users will want to use the NewHLAEvent and NewRawEvent method, if
  //they are even using this class. 
  //
  //you own the memory pointed to by this pointer, so you must delete it.
  //
  
	KEvent *event = 0;
	string sType = type;
	if(sType.compare(KHLAEvent::GetClassName())==0){
		event = new KHLAEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(KRawEvent::GetClassName())==0){
		event = new KRawEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(KHLaMCEvent::GetClassName())==0){
		event = new KHLaMCEvent;
		if(event != 0) BuildEvent(event);
	}
	/*
	 else if(sType.compare("MC") == 0){
	 event = new KMCEvent;
	 //do any other initialization stuff here, if needed in the future
	 }
	 */
	else {
		cout << "KEventFactory::NewEvent. Invalid event type: " << type << endl;
	}

	return event;
}

KHLAEvent* KEventFactory::NewHLAEvent(void)
{
	//returns a pointer to an KHLAEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<KHLAEvent*>(NewEvent(KHLAEvent::GetClassName()));
}

KRawEvent* KEventFactory::NewRawEvent(void)
{
	//returns a pointer to an KRawEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<KRawEvent*>(NewEvent(KRawEvent::GetClassName()));
}

KHLaMCEvent* KEventFactory::NewHLaMCEvent(void)
{
	//returns a pointer to an KHLaMCEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<KHLaMCEvent*>(NewEvent(KHLaMCEvent::GetClassName()));
}

Bool_t KEventFactory::DeleteEvent(KEvent *event)
{
	//can use this method to delete objects if you like. This might be 
	//useful in the future if the objects become more complicated.
	
	//returns true if it successfully deletes the object.
	if(event == 0) return true;
	
	//could check for event type and perform type specific cleanup if necessary
	
  event->Clear("C"); // make sure that we clear out the event's TClonesArrays.
	delete event; 
	event = 0; //why doesn't this set event to zero outside of this method? stupid scope
	
	return true;
}

KEvent* KEventFactory::NewEvent(const KEvent* event)
{
	//make a new Event object, but copy it from an already existing event.
	//This method checks which type of KEvent you passed in,
	//then creates a new Event of that type and returns a pointer. The return
	//here is KEvent, but you can cast that pointer to whatever
	//KEvent-derived object type you really have (KRawEvent, or KHLAEvent). 
	
	//have to check the type, then call that type's copy constructur and 
	//return the pointer. 
	
	KEvent *mNewEvent = 0;
	
	if(event == 0) return mNewEvent; 
	
  /*
	if(const KHLAEvent *mHLAEvent = dynamic_cast<const KHLAEvent*>(event)){
		mNewEvent = NewEvent(mHLAEvent);
	}
	else if(const KRawEvent *mRawEvent = dynamic_cast<const KRawEvent*>(event)){
		mNewEvent = NewEvent(mRawEvent);
	}
	else if(const KHLaMCEvent *mHLaMCEvent = dynamic_cast<const KHLaMCEvent*>(event)){
		mNewEvent = NewEvent(mHLaMCEvent);
	}
	else {
		cout << "KEventFactory::NewEvent: Cannot create a new Event of this type" << endl;
	}
	*/
  mNewEvent = NewEvent(event->GetClassName());
	return mNewEvent;
	
}

/*
KHLAEvent* KEventFactory::NewEvent(const KHLAEvent* event)
{
	//make a new Event object, but copy it from event.
	
	KHLAEvent *mNewEvent = new KHLAEvent(*event);
	
	return mNewEvent;
	
}


KRawEvent* KEventFactory::NewEvent(const KRawEvent* event)
{
	//make a new Event object, but copy it from event.
		
	KRawEvent *mNewEvent = new KRawEvent(*event);
	
	return mNewEvent;	
	
}

KHLaMCEvent* KEventFactory::NewEvent(const KHLaMCEvent* event)
{
	//make a new Event opject, but copy it from event
	
	KHLaMCEvent *mNewEvent = new KHLaMCEvent(*event);
	
	return mNewEvent;
}
*/

void KEventFactory::BuildEvent(KEvent *event)
{
	if(const KHLAEvent *mHLAEvent = dynamic_cast<const KHLAEvent*>(event)){
		//do whatever is needed. nothing for now. 
		if(mHLAEvent){
			//do stuff. 
		}
	}
	
	else if(const KRawEvent *mRawEvent = dynamic_cast<const KRawEvent*>(event)){
		//do whatever is needed. nothing for now.
		if(mRawEvent){
			//do stuff. 
		}
	}
	else if(const KHLaMCEvent *mHLaMCEvent = dynamic_cast<const KHLaMCEvent*>(event)){
		//do whatever is needed. nothing for now. 
		if(mHLaMCEvent){
			//do stuff. 
		}
	}
	else {
		//cout << "KEventFactory::NewEvent: Cannot create a new Event of this type" << endl;
	}
}
