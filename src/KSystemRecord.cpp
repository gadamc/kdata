//_____________________________________________
//
// KSystemRecord.cc
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the base class for all System Records
//


#include "KSystemRecord.h"
#include <iostream>
using namespace std;

ClassImp(KSystemRecord);

KSystemRecord::KSystemRecord(void)
{

  InitializeMembers();
}

KSystemRecord::KSystemRecord(const KSystemRecord &aRec)
{
	CopyLocalMembers(aRec);
}

KSystemRecord& KSystemRecord::operator=(const KSystemRecord &aRec)
{
	if(&aRec == this) return *this;
	
	CopyLocalMembers(aRec);
	return *this;
}

void KSystemRecord::CopyLocalMembers(const KSystemRecord &aRec)
{
	fRunStartTime = aRec.fRunStartTime;
	fRunEndTime = aRec.fRunEndTime;
	fIsSystemOn = aRec.fIsSystemOn;
}
KSystemRecord::~KSystemRecord(void)
{
	//
	
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KSystemRecord::Clear(Option_t* /* opt */)
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

void KSystemRecord::InitializeMembers(void)
{
	// Initialize local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	//fRunNumber=0;
	fRunStartTime = -99.;
	fRunEndTime = -99.;
	fIsSystemOn = 0;
}

Bool_t KSystemRecord::IsSame(const KSystemRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	if(fRunStartTime != aRec.fRunStartTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSystemRecord fRunStartTime Not Equal. lhs: " 
			<< fRunStartTime << " != rhs " << aRec.fRunStartTime << endl;		
		else
			return false;  
	}
	
	if(fRunEndTime != aRec.fRunEndTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSystemRecord fRunEndTime Not Equal. lhs: " 
			<< fRunEndTime << " != rhs " << aRec.fRunEndTime << endl;	
		else
			return false;  
	}
	
	if(fIsSystemOn != aRec.fIsSystemOn){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSystemRecord fIsSystemOn Not Equal. lhs: " 
			<< fIsSystemOn << " != rhs " << aRec.fIsSystemOn << endl;		
		else
			return false;  
	}
	
	return bIsEqual;
}
