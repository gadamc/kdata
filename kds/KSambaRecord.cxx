//_____________________________________________
//
// KSambaRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Samba Sub Records. This contains event information
// that is from the particular samba machine that the data was acquired on.
// 
//

#include "KSambaRecord.h"
#include <stdio.h>
#include <iostream>
#include <cstring>

ClassImp(KSambaRecord);

KSambaRecord::KSambaRecord(void)
{
	InitializeMembers();
	
	//We must deal with members on the heap separately 
	//from members on the stack
	//fRunName=0;
	//fRunName = new TString;
		
}


KSambaRecord::KSambaRecord(const KSambaRecord &aRec)
: KSubRecord(aRec)
{
	//fRunName=0;
	//fRunName = new TString;
	CopyLocalMembers(aRec);

}

KSambaRecord& KSambaRecord::operator=(const KSambaRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
		
	return *this;
}

void KSambaRecord::CopyLocalMembers(const KSambaRecord &aRec)
{
	SetSambaEventNumber(aRec.GetSambaEventNumber()); 
	SetNtpDateSec(aRec.GetNtpDateSec()); 
	SetNtpDateMicroSec(aRec.GetNtpDateMicroSec()); 
	SetSambaDAQNumber(aRec.GetSambaDAQNumber());
	
	SetRunName(aRec.fRunName.c_str());
	
}

KSambaRecord::~KSambaRecord(void)
{
	Clear("C");

	//if(fRunName!=0){
	//	delete fRunName;
	//	fRunName=0;	
	//}
	
	
}

void KSambaRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

	KSubRecord::Clear(opt);
	
  //Clear and delete local objects here. 
	//delete local stuff here

	//if(fRunName!=0){
	//	delete fRunName;
	//	fRunName=0;
	//}

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KSambaRecord::SetRunName(const Char_t* aWord)
{
	//if(fRunName == 0)
	//	fRunName = new TString;
	
	fRunName = aWord;

}

string KSambaRecord::GetRunName(void) const 
{
	//if(fRunName != 0){
	//	string str = fRunName->Data();
	//	return str;
	//}
	//else{
	//	return "";
	//}
	return fRunName;
}

void KSambaRecord::InitializeMembers(void)
{
	//Init local members
	
	//WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetSambaEventNumber(-99); 
	SetNtpDateSec(-99); 
	SetNtpDateMicroSec(-99); 
	SetSambaDAQNumber(-99);
	

}


Bool_t KSambaRecord::IsSame(const KSambaRecord &aRec, Bool_t bPrint) const
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
	
	if(fSambaEventNumber != aRec.fSambaEventNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSambaEventNumber Not Equal. lhs: " 
			<< fSambaEventNumber << " != rhs " << aRec.fSambaEventNumber << endl;	
		else
			return false;  
	}
	
	if(fNtpDateSec != aRec.fNtpDateSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fNtpDateSec Not Equal. lhs: " 
			<< fNtpDateSec << " != rhs " << aRec.fNtpDateSec << endl;		
		else
			return false;  
	}
	
	if(fNtpDateMicroSec != aRec.fNtpDateMicroSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fNtpDateMicroSec Not Equal. lhs: " 
			<< fNtpDateMicroSec << " != rhs " << aRec.fNtpDateMicroSec << endl;		
		else
			return false;  
	}
	
	if(fSambaDAQNumber != aRec.fSambaDAQNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSambaDAQNumber Not Equal. lhs: " 
			<< fSambaDAQNumber << " != rhs " << aRec.fSambaDAQNumber << endl;		
		else
			return false;  
	}
	
	if(fRunName != aRec.fRunName){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunName Not Equal. lhs: " 
			<< fRunName << " != rhs " << aRec.fRunName << endl;		
		else
			return false;  
	}
	
	/*
	if(fRunName != 0 && aRec.fRunName != 0){
		if(*fRunName != *aRec.fRunName){
			bIsEqual = false;
			if (bPrint) 
				cout << "KSambaRecord fRunName Not Equal. lhs: " 
				<< *fRunName << " != rhs " << *aRec.fRunName << endl;		
			else
				return false;  
		}
	}
	else {
		if (bPrint) 
			cout << "KSambaRecord a fRunName pointer is zero " 
			<< fRunName << " != rhs " << aRec.fRunName << endl;		
		else
			return false;
	}*/
	
	return bIsEqual;
}


void KSambaRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}

void KSambaRecord::print(void)
{
		
	cout << "Me 0x" << this << endl;
	//cout << "Me* " << *this << endl;
	cout << "fSambaEventNumber " << fSambaEventNumber << endl;
	cout << "fNtpDataSec " << fNtpDateSec << endl;
	cout << "fSambaDAQNumber "  << fSambaDAQNumber << endl;
	cout << "fRunName " << fRunName << endl;
	
}
