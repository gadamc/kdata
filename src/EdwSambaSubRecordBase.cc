//_____________________________________________
//
// EdwSambaSubRecordBase.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Samba Sub Records. This contains event information
// that is from the particular samba machine that the data was acquired on.
// 
//

#include "EdwSambaSubRecordBase.h"
#include <stdio.h>
#include <iostream>
#include <cstring>

ClassImp(EdwSambaSubRecordBase);

EdwSambaSubRecordBase::EdwSambaSubRecordBase(void)
{
	InitializeMembers();
	
	//We must deal with members on the heap separately 
	//from members on the stack
	fRunName=0;
	fFileName=0;
	fRunNameSize=0;
	fFileNameSize=0;

	
}


EdwSambaSubRecordBase::EdwSambaSubRecordBase(const EdwSambaSubRecordBase &aRec)
: EdwSubRecordBase(aRec)
{
	CopyLocalMembers(aRec);
	
	fRunName=0;
	fFileName=0;
	fRunNameSize=0;
	fFileNameSize=0;
	
	SetRunName(aRec.GetRunName().data(), aRec.GetRunNameSize());
	SetFileName(aRec.GetFileName().data(), aRec.GetFileNameSize());
	
}

EdwSambaSubRecordBase& EdwSambaSubRecordBase::operator=(const EdwSambaSubRecordBase &aRec)
{
	if(&aRec == this) return *this;
	
	this->EdwSubRecordBase::operator=(aRec);
	CopyLocalMembers(aRec);
	
	SetRunName(aRec.GetRunName().data(), aRec.GetRunNameSize());
	SetFileName(aRec.GetFileName().data(), aRec.GetFileNameSize());
	
	return *this;
}

void EdwSambaSubRecordBase::CopyLocalMembers(const EdwSambaSubRecordBase &aRec)
{
	SetSambaEventNumber(aRec.GetSambaEventNumber()); 
	SetNtpDateSec(aRec.GetNtpDateSec()); 
	SetNtpDateMicroSec(aRec.GetNtpDateMicroSec()); 
	SetSambaDAQNumber(aRec.GetSambaDAQNumber());
	
	SetPosition(aRec.GetPosition());
	SetDelai(aRec.GetDelai());
	
	SetSambaMajorVersionNum(aRec.GetSambaMajorVersionNum());
	SetSambaMinorVersionNum(aRec.GetSambaMinorVersionNum());
}

EdwSambaSubRecordBase::~EdwSambaSubRecordBase(void)
{
	Clear("C");

	if(fRunName!=0){
		delete[] fRunName;
		fRunName=0;	
		fRunNameSize=0;
	}
	if(fFileName!=0){
		delete[] fFileName;	
		fFileName=0;
		fFileNameSize=0;
	}
	
}

void EdwSambaSubRecordBase::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

	EdwSubRecordBase::Clear(opt);
	
  //Clear and delete local objects here. 
	//delete local stuff here

	if(fRunName!=0){
		delete[] fRunName;
		fRunNameSize=0;	
		fRunName=0;
	}

	if(fFileName!=0){
		delete[] fFileName;
		fFileNameSize=0;
		fFileName=0;
	}
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void EdwSambaSubRecordBase::SetRunName(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fRunNameSize){
		if(fRunName!=0){
			delete[] fRunName;
			fRunName=0;
			fRunNameSize=0;	
		}
		if(aSize>0)fRunName= new Char_t[aSize];
		fRunNameSize=aSize;
	}
	if(aSize>0)memcpy(fRunName, aWord, fRunNameSize*sizeof(Char_t));
}



void EdwSambaSubRecordBase::SetFileName(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fFileNameSize){
		if(fFileName!=0){
			delete[] fFileName;	
			fFileName=0;
			fFileNameSize=0;	
		}
		if(aSize>0)fFileName= new Char_t[aSize];
		fFileNameSize=aSize;
	}
	if(aSize>0)memcpy(fFileName, aWord, fFileNameSize*sizeof(Char_t));
}

string EdwSambaSubRecordBase::GetRunName(void) const 
{
	string str=""; 
	
	if(fRunName==0) 
		return str;
	
	else return str.assign(fRunName, fRunNameSize);
	
}

string EdwSambaSubRecordBase::GetFileName(void) const 
{
	string str=""; 
	
	if(fFileName==0) 
		return str;
	
	else return str.assign(fFileName, fFileNameSize);
	
}

void EdwSambaSubRecordBase::InitializeMembers(void)
{
	//Init local members
	
	//WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetSambaEventNumber(-99); 
	SetNtpDateSec(-99); 
	SetNtpDateMicroSec(-99); 
	SetSambaDAQNumber(-99);
	
	SetPosition(-99);
	SetDelai(-99.0);
	
	SetSambaMajorVersionNum(-99);
	SetSambaMinorVersionNum(-99);
}


Bool_t EdwSambaSubRecordBase::IsSame(const EdwSambaSubRecordBase &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwSubRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fSambaEventNumber != aRec.fSambaEventNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fSambaEventNumber Not Equal. lhs: " 
			<< fSambaEventNumber << " != rhs " << aRec.fSambaEventNumber << endl;	
		else
			return false;  
	}
	
	if(fNtpDateSec != aRec.fNtpDateSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fNtpDateSec Not Equal. lhs: " 
			<< fNtpDateSec << " != rhs " << aRec.fNtpDateSec << endl;		
		else
			return false;  
	}
	
	if(fNtpDateMicroSec != aRec.fNtpDateMicroSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fNtpDateMicroSec Not Equal. lhs: " 
			<< fNtpDateMicroSec << " != rhs " << aRec.fNtpDateMicroSec << endl;		
		else
			return false;  
	}
	
	if(fSambaDAQNumber != aRec.fSambaDAQNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fSambaDAQNumber Not Equal. lhs: " 
			<< fSambaDAQNumber << " != rhs " << aRec.fSambaDAQNumber << endl;		
		else
			return false;  
	}
	
	if(fRunNameSize != aRec.fRunNameSize){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fRunNameSize Not Equal. lhs: " 
			<< fRunNameSize << " != rhs " << aRec.fRunNameSize << endl;		
		else
			return false;  
	}
	
	if(strncmp(fRunName, aRec.fRunName, sizeof(Char_t)*fRunNameSize)){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fDetectorName Not Equal. lhs: " 
			<< fRunName << " != rhs " << aRec.fRunName << endl;		
		else
			return false;  
	}
	
	if(fFileNameSize != aRec.fFileNameSize){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fFileNameSize Not Equal. lhs: " 
			<< fFileNameSize << " != rhs " << aRec.fFileNameSize << endl;		
		else
			return false;  
	}
	
	if(strncmp(fFileName, aRec.fFileName, sizeof(Char_t)*fFileNameSize)){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fFileName Not Equal" << fFileName << " != " << aRec.fFileName << "_" << endl;		
		else
			return false;  
	}
	
	if(fPosition != aRec.fPosition){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fPosition Not Equal. lhs: " 
			<< fPosition << " != rhs " << aRec.fPosition << endl;		
		else
			return false;  
	}
	
	if(fDelai != aRec.fDelai){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fDelai Not Equal. lhs: " 
			<< fDelai << " != rhs " << aRec.fDelai << endl;		
		else
			return false;  
	}
	
	if(fSambaMajorVersionNum != aRec.fSambaMajorVersionNum){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fSambaMajorVersionNum Not Equal. lhs: " 
			<< fSambaMajorVersionNum << " != rhs " << aRec.fSambaMajorVersionNum << endl;		
		else
			return false;  
	}
	
	if(fSambaMinorVersionNum != aRec.fSambaMinorVersionNum){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwSambaSubRecordBase fSambaMinorVersionNum Not Equal. lhs: " 
			<< fSambaMinorVersionNum << " != rhs " << aRec.fSambaMinorVersionNum << endl;		
		else
			return false;  
	}
	
	return bIsEqual;
}


void EdwSambaSubRecordBase::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwSubRecordBase::Compact();
}


