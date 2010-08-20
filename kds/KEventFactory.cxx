//_____________________________________________
//
// KEventFactory.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/11/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Ideally, one would use this class whenever they want to create
// some type of Event class that derives from KEvent. When 
// an event class is created that derives from KEvent, the author
// must make sure to declare the kEventName and the kClassName for 
// their class. See the KHLAEvent and KRawEvent source files for 
// an example. 
//
// To use this class, one simply asks the object for a new event of 
// whatever type of even is needed. Also, the methods which produce
// a new event are static methods, which means an instance of a KEventFactory
// is not needed to use the method. 
//
// KDS Developer's Note: If you want the KDataReader and KDataWriter classes
// to recognize your Event class, you must derive from KEvent and 
// you must implement your class here. The KHLaMCEvent class (along with 
// KHLAEvent and KRawEvent) is an example of this. 

#include "KEventFactory.h"
#include "KHLAEvent.h"
#include "KRawEvent.h"
#include "KHLaMCEvent.h"
//#include "KMCEvent.h"

#include <iostream>

using namespace std;

ClassImp(KEventFactory);

KEventFactory::KEventFactory(void)
{

  InitializeMembers();
}

KEventFactory::~KEventFactory(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KEventFactory::Clear(Option_t* /* opt */)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KEventFactory::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

KEvent* KEventFactory::NewEvent(const Char_t* type) 
{
	KEvent *event = 0;
	string sType = type;
	if(sType.compare(kHLAEventName)==0 || sType.compare(kHLAClassName)==0){
		event = new KHLAEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(kRawEventName)==0 || sType.compare(kRawClassName)==0){
		event = new KRawEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(kHLaMCEventName)==0 || sType.compare(kHLaMCClassName)==0){
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
	
	return dynamic_cast<KHLAEvent*>(NewEvent(kHLAEventName));
}

KRawEvent* KEventFactory::NewRawEvent(void)
{
	//returns a pointer to an KRawEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<KRawEvent*>(NewEvent(kRawEventName));
}

KHLaMCEvent* KEventFactory::NewHLaMCEvent(void)
{
	//returns a pointer to an KHLaMCEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<KHLaMCEvent*>(NewEvent(kHLaMCEventName));
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
	//here is KEvent, but you should cast that pointer to whatever
	//event type you really have. 
	
	//have to check the type, then call that type's copy constructur and 
	//return the pointer. 
	
	KEvent *mNewEvent = 0;
	
	if(event == 0) return mNewEvent; 
	
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
	
	return mNewEvent;
	
}


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

/*
shared_ptr<KHLAEvent> KEventFactory::NewHLAEventTest(void)
{
	shared_ptr<KHLAEvent> mEvent(new KHLAEvent);
	return mEvent;
}*/
