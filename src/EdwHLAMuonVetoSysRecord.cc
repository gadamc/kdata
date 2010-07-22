//_____________________________________________
//
// EdwHLAMuonVetoSysRecord.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// There are not any member variables in this System Record. All Muon
// Veto System record values are stored in the base class EdwMuonVetoSystemRecordBase.
//

#include "EdwHLAMuonVetoSysRecord.h"
#include <iostream>
using namespace std;

ClassImp(EdwHLAMuonVetoSysRecord);

EdwHLAMuonVetoSysRecord::EdwHLAMuonVetoSysRecord(void)
{

  InitializeMembers();
}

EdwHLAMuonVetoSysRecord::~EdwHLAMuonVetoSysRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

EdwHLAMuonVetoSysRecord::EdwHLAMuonVetoSysRecord(const EdwHLAMuonVetoSysRecord &aRec)
: EdwMuonVetoSystemRecordBase(aRec), EdwHLASystemRecordBase(aRec)
{
	CopyLocalMembers(aRec);
}

EdwHLAMuonVetoSysRecord& EdwHLAMuonVetoSysRecord::operator=(const EdwHLAMuonVetoSysRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->EdwMuonVetoSystemRecordBase::operator=(aRec);
	this->EdwHLASystemRecordBase::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void EdwHLAMuonVetoSysRecord::CopyLocalMembers(const EdwHLAMuonVetoSysRecord &/*aRec*/)
{
	//nothing to do! :)
}


void EdwHLAMuonVetoSysRecord::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	EdwMuonVetoSystemRecordBase::Clear(opt);
	EdwHLASystemRecordBase::Clear(opt);
	
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void EdwHLAMuonVetoSysRecord::InitializeMembers(void)
{
	//Init local members.
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

Bool_t EdwHLAMuonVetoSysRecord::IsSame(const EdwHLAMuonVetoSysRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwMuonVetoSystemRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}

	if(!this->EdwHLASystemRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	return bIsEqual;

}

void EdwHLAMuonVetoSysRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwMuonVetoSystemRecordBase::Compact();
	EdwHLASystemRecordBase::Compact();
	
}