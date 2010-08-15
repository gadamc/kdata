//_____________________________________________
//
// KEventCollection.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/26/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Based on TObjArray, this is a simple collection class to hold a list
// of KEvent objects. It can hold any type of event that inherits 
// from KEvent. 
//
#include "KEventCollection.h"
#include "KEvent.h"
#include "KEventFactory.h"

ClassImp(KEventCollection);

KEventCollection::KEventCollection(Int_t s, Int_t lowerBound)
: TObjArray(s, lowerBound)
{
	SetOwner();  
	InitializeMembers();
}

KEventCollection::~KEventCollection(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");
	
}

void KEventCollection::Clear(Option_t* anOpt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	TObjArray::Clear(anOpt);
	
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KEventCollection::InitializeMembers(void)
{
	//
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

}

void KEventCollection::AddEvent(const KEvent *anEvent)
{
	//Add an event to your collection. This calls TObjArray::Add.
	
	TObjArray::Add(KEventFactory::NewEvent(anEvent));
	
}
/*
void KEventCollection::AddInitialEvent(const KEvent *anEvent)
{
	
	this->AddEvent(anEvent);
	
	//record information about this initial event 
	// bolo sub record, muon sub record number, etc...
	
}*/

