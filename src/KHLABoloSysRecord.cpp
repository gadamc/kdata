//_____________________________________________
//
// KHLABoloSysRecord.cc
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// There are not any member variables in this System Record. All Muon
// Veto System record values are stored in the base class KBoloSystemRecord.
//

#include "KHLABoloSysRecord.h"
#include "TClonesArray.h"
#include <iostream>

using namespace std;

ClassImp(KHLABoloSysRecord);

KHLABoloSysRecord::KHLABoloSysRecord(void)
{
	
  InitializeMembers();
}

KHLABoloSysRecord::KHLABoloSysRecord(const KHLABoloSysRecord &aRec)
: KBoloSystemRecord(aRec)
{
	CopyLocalMembers(aRec);
}

KHLABoloSysRecord& KHLABoloSysRecord::operator=(const KHLABoloSysRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KBoloSystemRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KHLABoloSysRecord::CopyLocalMembers(const KHLABoloSysRecord &/*aRec*/)
{
	//nothing to do! :)
}

KHLABoloSysRecord::~KHLABoloSysRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");
	
}

void KHLABoloSysRecord::Clear(Option_t *anOption)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KBoloSystemRecord::Clear(anOption);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KHLABoloSysRecord::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}


Bool_t KHLABoloSysRecord::IsSame(const KHLABoloSysRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KBoloSystemRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
		
	return bIsEqual;
	
}

void KHLABoloSysRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KBoloSystemRecord::Compact();
	
}
