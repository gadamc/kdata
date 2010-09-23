//_____________________________________________
//
// KBolometerRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// A base class for all SingleBolo Sub Records. The SingleBolo Sub records
// contain data from an individual bolometer. 
//

#include "KBolometerRecord.h"
#include <stdio.h>
#include <iostream>
#include <cstring> 

using namespace std;

ClassImp(KBolometerRecord);

KBolometerRecord::KBolometerRecord(void)
{
	InitializeMembers();
	
	//we must take care of members on the heap outside of 
	//InitializeMembers...
	
}

KBolometerRecord::KBolometerRecord(const KBolometerRecord &aRec)
: KSubRecord(aRec)
{
	
	CopyLocalMembers(aRec);
	
}

KBolometerRecord& KBolometerRecord::operator=(const KBolometerRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	
	
	return *this;
}

void KBolometerRecord::CopyLocalMembers(const KBolometerRecord &aRec)
{
	
	SetDetectorName(aRec.fDetectorName.c_str());
	
  
}

KBolometerRecord::~KBolometerRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");
	
  
}

void KBolometerRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KSubRecord::Clear(opt);
	
  //Clear and delete local objects here. 
	//if(fDetectorName!=0){
	//	delete fDetectorName;
	//	fDetectorName=0;
	//}
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KBolometerRecord::InitializeMembers(void)
{
  //
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
}

void KBolometerRecord::SetDetectorName(const Char_t* aWord)
{
	//if(fDetectorName == 0)
	//	fDetectorName = new TString;
	fDetectorName = aWord;
}

string KBolometerRecord::GetDetectorName(void) const 
{
	/*if(fDetectorName != 0){
		string str = fDetectorName->Data();
		return str;
	}
	else{
		return "";
	}
	*/
	return fDetectorName;
}


Bool_t KBolometerRecord::IsSame(const KBolometerRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KSubRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}

	if(fDetectorName != aRec.fDetectorName){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBolometerRecord fDetectorName Not Equal. lhs: " 
			<< fDetectorName << " != rhs " << aRec.fDetectorName << endl;		
		else
			return false;  
	}
		
	return bIsEqual;
}


void KBolometerRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}

Bool_t KBolometerRecord::IsDetector(const char* name)
{
  //returns true if name is equal to the string fDetectorName.
  //This method is quite useful when using the TTree::Draw methods 
  //and accessing data via the class methods instead of the variable names
  //
  if(name == 0) return false;
  
  if(strcmp(fDetectorName.c_str(), name) == 0)
    return true;
  
  else return false;

}



