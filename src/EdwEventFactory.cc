//_____________________________________________
//
// EdwEventFactory.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/11/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Ideally, one would use this class whenever they want to create
// some type of Event class that derives from EdwEventBase. When 
// an event class is created that derives from EdwEventBase, the author
// must make sure to declare the kEventName and the kClassName for 
// their class. See the EdwHLAEvent and EdwRawEvent source files for 
// an example. 
//
// To use this class, one simply asks the object for a new event of 
// whatever type of even is needed. Also, the methods which produce
// a new event are static methods, which means an instance of a EdwEventFactory
// is not needed to use the method. 
//
// EdwDS Developer's Note: If you want the EdwDSReader and EdwDSWriter classes
// to recognize your Event class, you must derive from EdwEventBase and 
// you must implement your class here. The EdwHLaMCEvent class (along with 
// EdwHLAEvent and EdwRawEvent) is an example of this. 

#include "EdwEventFactory.h"
#include "EdwHLAEvent.h"
#include "EdwRawEvent.h"
#include "EdwHLaMCEvent.h"
//#include "EdwMCEvent.h"

#include <iostream>

using namespace std;

ClassImp(EdwEventFactory);

EdwEventFactory::EdwEventFactory(void)
{

  InitializeMembers();
}

EdwEventFactory::~EdwEventFactory(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void EdwEventFactory::Clear(Option_t* /* opt */)
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

void EdwEventFactory::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

EdwEventBase* EdwEventFactory::NewEvent(const Char_t* type) 
{
	EdwEventBase *event = 0;
	string sType = type;
	if(sType.compare(kHLAEventName)==0 || sType.compare(kHLAClassName)==0){
		event = new EdwHLAEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(kRawEventName)==0 || sType.compare(kRawClassName)==0){
		event = new EdwRawEvent;
		if(event != 0) BuildEvent(event);
	}
	else if(sType.compare(kHLaMCEventName)==0 || sType.compare(kHLaMCClassName)==0){
		event = new EdwHLaMCEvent;
		if(event != 0) BuildEvent(event);
	}
	/*
	 else if(sType.compare("MC") == 0){
	 event = new EdwMCEvent;
	 //do any other initialization stuff here, if needed in the future
	 }
	 */
	else {
		cout << "EdwEventFactory::NewEvent. Invalid event type: " << type << endl;
	}

	return event;
}

EdwHLAEvent* EdwEventFactory::NewHLAEvent(void)
{
	//returns a pointer to an EdwHLAEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<EdwHLAEvent*>(NewEvent(kHLAEventName));
}

EdwRawEvent* EdwEventFactory::NewRawEvent(void)
{
	//returns a pointer to an EdwRawEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<EdwRawEvent*>(NewEvent(kRawEventName));
}

EdwHLaMCEvent* EdwEventFactory::NewHLaMCEvent(void)
{
	//returns a pointer to an EdwHLaMCEvent object. You own the memory, so
	//you must delete it.
	
	return dynamic_cast<EdwHLaMCEvent*>(NewEvent(kHLaMCEventName));
}

Bool_t EdwEventFactory::DeleteEvent(EdwEventBase *event)
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

EdwEventBase* EdwEventFactory::NewEvent(const EdwEventBase* event)
{
	//make a new Event object, but copy it from an already existing event.
	//This method checks which type of EdwEventBase you passed in,
	//then creates a new Event of that type and returns a pointer. The return
	//here is EdwEventBase, but you should cast that pointer to whatever
	//event type you really have. 
	
	//have to check the type, then call that type's copy constructur and 
	//return the pointer. 
	
	EdwEventBase *mNewEvent = 0;
	
	if(event == 0) return mNewEvent; 
	
	if(const EdwHLAEvent *mHLAEvent = dynamic_cast<const EdwHLAEvent*>(event)){
		mNewEvent = NewEvent(mHLAEvent);
	}
	else if(const EdwRawEvent *mRawEvent = dynamic_cast<const EdwRawEvent*>(event)){
		mNewEvent = NewEvent(mRawEvent);
	}
	else if(const EdwHLaMCEvent *mHLaMCEvent = dynamic_cast<const EdwHLaMCEvent*>(event)){
		mNewEvent = NewEvent(mHLaMCEvent);
	}
	else {
		cout << "EdwEventFactory::NewEvent: Cannot create a new Event of this type" << endl;
	}
	
	return mNewEvent;
	
}


EdwHLAEvent* EdwEventFactory::NewEvent(const EdwHLAEvent* event)
{
	//make a new Event object, but copy it from event.
	
	EdwHLAEvent *mNewEvent = new EdwHLAEvent(*event);
	
	return mNewEvent;
	
}


EdwRawEvent* EdwEventFactory::NewEvent(const EdwRawEvent* event)
{
	//make a new Event object, but copy it from event.
		
	EdwRawEvent *mNewEvent = new EdwRawEvent(*event);
	
	return mNewEvent;	
	
}

EdwHLaMCEvent* EdwEventFactory::NewEvent(const EdwHLaMCEvent* event)
{
	//make a new Event opject, but copy it from event
	
	EdwHLaMCEvent *mNewEvent = new EdwHLaMCEvent(*event);
	
	return mNewEvent;
}


void EdwEventFactory::BuildEvent(EdwEventBase *event)
{
	if(const EdwHLAEvent *mHLAEvent = dynamic_cast<const EdwHLAEvent*>(event)){
		//do whatever is needed. nothing for now. 
		if(mHLAEvent){
			//do stuff. 
		}
	}
	
	else if(const EdwRawEvent *mRawEvent = dynamic_cast<const EdwRawEvent*>(event)){
		//do whatever is needed. nothing for now.
		if(mRawEvent){
			//do stuff. 
		}
	}
	else if(const EdwHLaMCEvent *mHLaMCEvent = dynamic_cast<const EdwHLaMCEvent*>(event)){
		//do whatever is needed. nothing for now. 
		if(mHLaMCEvent){
			//do stuff. 
		}
	}
	else {
		//cout << "EdwEventFactory::NewEvent: Cannot create a new Event of this type" << endl;
	}
}

/*
shared_ptr<EdwHLAEvent> EdwEventFactory::NewHLAEventTest(void)
{
	shared_ptr<EdwHLAEvent> mEvent(new EdwHLAEvent);
	return mEvent;
}*/
