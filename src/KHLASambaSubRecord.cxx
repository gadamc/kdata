//_____________________________________________
//
// KHLASambaSubRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Most of the Samba Sub Record data are in the base class KSambaSubRecord.
// In fact, there's nothing in this class at the moment. 
// 


#include "KHLASambaSubRecord.h"
#include <iostream>
using namespace std;

ClassImp(KHLASambaSubRecord);

KHLASambaSubRecord::KHLASambaSubRecord(void)
{

  InitializeMembers();
}

KHLASambaSubRecord::KHLASambaSubRecord(const KHLASambaSubRecord &aRec)
: KSambaSubRecord(aRec)
{
	CopyLocalMembers(aRec);
	
}

KHLASambaSubRecord& KHLASambaSubRecord::operator=(const KHLASambaSubRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSambaSubRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KHLASambaSubRecord::CopyLocalMembers(const KHLASambaSubRecord &/*aRec*/)
{
	//nothing to do
	
}

KHLASambaSubRecord::~KHLASambaSubRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KHLASambaSubRecord::Clear(Option_t  *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KSambaSubRecord::Clear(opt);
	
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KHLASambaSubRecord::InitializeMembers(void)
{
	//Init local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

Bool_t KHLASambaSubRecord::IsSame(const KHLASambaSubRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KSambaSubRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	

	
	return bIsEqual;
}


void KHLASambaSubRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSambaSubRecord::Compact();
	
}

