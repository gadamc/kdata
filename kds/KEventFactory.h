/*
 *  KEventFactory.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 5/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KEVENTFACTORY_H__
#define __KEVENTFACTORY_H__

//#include "TList.h"
#include "Rtypes.h"
//#include <boost/smart_ptr.hpp>
//using namespace boost;

class KEvent;
class KHLAEvent;
class KRawEvent;
class KHLaMCEvent;

class KEventFactory  { 

	//eventually, I should use the Boost libraries 
	//and make the pointers returned by this class 
	//to be shared pointers, which will then 
	//make them safer to delete. 
	//when an instance of the factory is destroyed,
	//it will attempt to delete all of the events that it created
	//unless another client is using it. then it won't delete it.
	//when that client wants to delete the event they
	//will call KEventFactory::DeleteEvent(KEvent*);
	//or just let it go out of scope
  //However, c++ will soon be updated to include smart pointers
  //which could be implemented here without the need for boost.
	
public:
	//Constructors
  KEventFactory(void);
  virtual ~KEventFactory(void);
	
	static KEvent* NewEvent(const Char_t* type); //support polymorphism
	static KEvent* NewEvent(const KEvent* event);  //support polymorphism -- this is like a copy constructor
	static KHLAEvent* NewHLAEvent(void);
	static KHLaMCEvent* NewHLaMCEvent(void);
	static KRawEvent* NewRawEvent(void);
	static Bool_t DeleteEvent(KEvent* event);
	//shared_ptr<KHLAEvent> NewHLAEventTest(void);
	
private:
	
	
  //private methods
	static void BuildEvent(KEvent *event);
	
  ClassDef(KEventFactory,2);
};


#endif // __KEVENTFACTORY_H__
