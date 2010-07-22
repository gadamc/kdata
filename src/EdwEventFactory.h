/*
 *  EdwEventFactory.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 5/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWEVENTFACTORY_H__
#define __EDWEVENTFACTORY_H__

//#include "TList.h"
#include "Rtypes.h"
//#include <boost/smart_ptr.hpp>
//using namespace boost;

class EdwEventBase;
class EdwHLAEvent;
class EdwRawEvent;
class EdwHLaMCEvent;

class EdwEventFactory  { 

	//eventually, I should use the Boost libraries 
	//and make the pointers returned by this class 
	//to be shared pointers, which will then 
	//make them safer to delete. 
	//when an instance of the factory is destroyed,
	//it will attempt to delete all of the events that it created
	//unless another client is using it. then it won't delete it.
	//when that client wants to delete the event they
	//will call EdwEventFactory::DeleteEvent(EdwEventBase*);
	//or just let it go out of scope
	
public:
	//Constructors
  EdwEventFactory(void);
  virtual ~EdwEventFactory(void);
	virtual void Clear(Option_t* opt);
	
	static EdwEventBase* NewEvent(const Char_t* type); //support polymorphism
	static EdwEventBase* NewEvent(const EdwEventBase* event);  //support polymorphism -- this is like a copy constructor
	static EdwHLAEvent* NewEvent(const EdwHLAEvent* event);  //support polymorphism -- this is like a copy constructor
	static EdwHLaMCEvent* NewEvent(const EdwHLaMCEvent* event);
	static EdwRawEvent* NewEvent(const EdwRawEvent* event);  //support polymorphism -- this is like a copy constructor
	static EdwHLAEvent* NewHLAEvent(void);
	static EdwHLaMCEvent* NewHLaMCEvent(void);
	static EdwRawEvent* NewRawEvent(void);
	static Bool_t DeleteEvent(EdwEventBase* event);
	//shared_ptr<EdwHLAEvent> NewHLAEventTest(void);
	
private:
	
	//void AddToList(EdwEventBase *event);
	
	//TList mListOfEvents;
	
  //private methods
  void InitializeMembers(void);
	static void BuildEvent(EdwEventBase *event);
	
  ClassDef(EdwEventFactory,1);
};


#endif // __EDWEVENTFACTORY_H__
