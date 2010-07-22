//_____________________________________________
//
// EdwBoloSystemRecordBase.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is base class for all Bolo System Records.
// It contains the run name, given as the first four alpha-numeric values
// of the run name from Samba. For example, kc21, is 2010, March 21st. 
// 


#include "EdwBoloSystemRecordBase.h"
#include <iostream>

ClassImp(EdwBoloSystemRecordBase);

EdwBoloSystemRecordBase::EdwBoloSystemRecordBase(void)
{
	
	InitializeMembers();
}

EdwBoloSystemRecordBase::~EdwBoloSystemRecordBase(void)
{
	
	//Does calling clear at destruction take too much computing time?
  Clear("C");
}

EdwBoloSystemRecordBase::EdwBoloSystemRecordBase(const EdwBoloSystemRecordBase &aRec)
:EdwSystemRecordBase(aRec)
{
	CopyLocalMembers(aRec);
}

EdwBoloSystemRecordBase& EdwBoloSystemRecordBase::operator=(const EdwBoloSystemRecordBase &aRec)
{
	if(&aRec == this) return *this;
	
	this->EdwSystemRecordBase::operator=(aRec);
	CopyLocalMembers(aRec);
	return *this;
}

void EdwBoloSystemRecordBase::CopyLocalMembers(const EdwBoloSystemRecordBase &aRec)
{

	fRunName = aRec.fRunName;

}

void EdwBoloSystemRecordBase::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
	EdwSystemRecordBase::Clear(opt);
	
	//Clear and delete local objects here. 
	
	//Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
}


void EdwBoloSystemRecordBase::InitializeMembers(void)
{
	//Initialize local members.
	
	fRunName = "";
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

/*
Double_t EdwBoloSystemRecordBase::GetTimeSinceLastBoloSysEvent(void) const
{
	return GetTimeSinceLastSystemRecord();  //call the base class's TSLSystemRecord
}

void EdwBoloSystemRecordBase::SetTimeSinceLastBoloSysEvent(Double_t aTime)
{
	SetTimeSinceLastSystemRecord(aTime);  //call the base class's TSLSystemRecord
}
*/

Bool_t EdwBoloSystemRecordBase::IsSame(const EdwBoloSystemRecordBase &aRec, Bool_t bPrint) const
{
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwSystemRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fRunName != aRec.fRunName){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwBoloSystemRecordBase fDistanceEst Not Equal. lhs: " 
			<< fRunName << " != rhs " << aRec.fRunName << endl;	
		else
			return false;  
	}

	
	return bIsEqual;
	
}

void EdwBoloSystemRecordBase::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwSystemRecordBase::Compact();
	
}
