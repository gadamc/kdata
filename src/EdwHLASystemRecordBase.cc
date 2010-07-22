//_____________________________________________
//
// EdwHLASystemRecordBase.cc
// EdwDataStructure
//
// //Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the base class for all HLA System Records. Currently, there
// isn't much in this class, but it could be useful in the future.
//

#include "EdwHLASystemRecordBase.h"
//#include <iostream>

//using namespace std;

ClassImp(EdwHLASystemRecordBase);

EdwHLASystemRecordBase::EdwHLASystemRecordBase(void)
{

  InitializeMembers();
}

EdwHLASystemRecordBase::EdwHLASystemRecordBase(const EdwHLASystemRecordBase &aRec)
{
	CopyLocalMembers(aRec);
	
}

EdwHLASystemRecordBase& EdwHLASystemRecordBase::operator=(const EdwHLASystemRecordBase &aRec)
{
	if(&aRec == this) return *this;
	
	CopyLocalMembers(aRec);
	
	return *this;
}


EdwHLASystemRecordBase::~EdwHLASystemRecordBase(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void EdwHLASystemRecordBase::CopyLocalMembers(const EdwHLASystemRecordBase &/*aRec*/)
{
 
}

void EdwHLASystemRecordBase::Clear(Option_t* /* opt */)
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

void EdwHLASystemRecordBase::InitializeMembers(void)
{
	//Initialize local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	/*
	fDtPreviousBolo = -99;
	fDtNextBolo = -99;
	fDtPreviousMuon = -99;
	fDtNextMuon = -99;
	
	fPreviousBoloGSEventNumber = -99;
	fNextBoloGSEventNumber = -99;
	fPreviousMuonGSEventNumber = -99;
	fNextMuonGSEventNumber = -99;
	 */
}

Bool_t EdwHLASystemRecordBase::IsSame(const EdwHLASystemRecordBase &/*aRec*/, 
																			Bool_t /*bPrint*/) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	/*
	if(fDtPreviousBolo != aRec.fDtPreviousBolo){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fDtPreviousBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fDtNextBolo != aRec.fDtNextBolo){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fDtNextBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fDtPreviousMuon != aRec.fDtPreviousMuon){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fDtPreviousMuon Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fDtNextMuon != aRec.fDtNextMuon){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fDtNextMuon Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fPreviousBoloGSEventNumber != aRec.fPreviousBoloGSEventNumber){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fPreviousBoloGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fNextBoloGSEventNumber != aRec.fNextBoloGSEventNumber){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fNextBoloGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fPreviousMuonGSEventNumber != aRec.fPreviousMuonGSEventNumber){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fPreviousMuonGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fNextMuonGSEventNumber != aRec.fNextMuonGSEventNumber){
		if (bPrint) 
			cout << "EdwHLASystemRecordBase fNextMuonGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	*/
	return bIsEqual;
}