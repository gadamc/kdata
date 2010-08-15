//_____________________________________________
//
// KSingleBoloSubRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// A base class for all SingleBolo Sub Records. The SingleBolo Sub records
// contain data from an individual bolometer. 
//

#include "KSingleBoloSubRecord.h"
#include <stdio.h>
#include <iostream>
#include <cstring> 

using namespace std;

ClassImp(KSingleBoloSubRecord);

KSingleBoloSubRecord::KSingleBoloSubRecord(void)
{
	InitializeMembers();
	
	//we must take care of members on the heap outside of 
	//InitializeMembers...
	
	fDetectorName=0;
	fDetectorNameSize=0;
	
}

KSingleBoloSubRecord::KSingleBoloSubRecord(const KSingleBoloSubRecord &aRec)
: KSubRecord(aRec)
{
	CopyLocalMembers(aRec);
	fDetectorNameSize = 0;
	fDetectorName = 0;
	SetDetectorName(aRec.GetDetectorName().c_str(), aRec.GetDetectorNameSize());

}

KSingleBoloSubRecord& KSingleBoloSubRecord::operator=(const KSingleBoloSubRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	
	SetDetectorName(aRec.GetDetectorName().c_str(), aRec.GetDetectorNameSize());
	
	return *this;
}

void KSingleBoloSubRecord::CopyLocalMembers(const KSingleBoloSubRecord &aRec)
{

	SetDetectorNumber(aRec.GetDetectorNumber());
	//SetVoie(aRec.GetVoie());
	//SetHeatPositiveThreshold(aRec.GetHeatPositiveThreshold());
	//SetHeatNegativeThreshold(aRec.GetHeatNegativeThreshold());
	//SetTimeOfLastThresholdChange(aRec.GetTimeOfLastThresholdChange());
}

KSingleBoloSubRecord::~KSingleBoloSubRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");
	
	//clean up 
	if(fDetectorName!=0){
		delete[] fDetectorName;
		fDetectorName=0;
		fDetectorNameSize=0;
	}
}

void KSingleBoloSubRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KSubRecord::Clear(opt);
	
  //Clear and delete local objects here. 
	if(fDetectorName!=0){
		delete[] fDetectorName;
		fDetectorNameSize=0;
		fDetectorName=0;
	}
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KSingleBoloSubRecord::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	SetDetectorNumber(-99);
	//SetDetectorName("",1);
	//SetVoie(-99);
	//SetHeatPositiveThreshold(-99);
	//SetHeatNegativeThreshold(-99);
	//SetTimeOfLastThresholdChange(-99);
}

void KSingleBoloSubRecord::SetDetectorName(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fDetectorNameSize){
		if(fDetectorName!=0){
			delete[] fDetectorName;	
			fDetectorName=0;
			fDetectorNameSize=0;
		}
		if(aSize>0)fDetectorName= new Char_t[aSize];
		fDetectorNameSize=aSize;
	}
	if(aSize>0)memcpy(fDetectorName, aWord, fDetectorNameSize*sizeof(Char_t));
}

string KSingleBoloSubRecord::GetDetectorName(void) const 
{
	string str=""; 
	
	if(fDetectorName==0) 
		return str;
	
	else return str.assign(fDetectorName, fDetectorNameSize);
	
}


Bool_t KSingleBoloSubRecord::IsSame(const KSingleBoloSubRecord &aRec, Bool_t bPrint) const
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
	
	if(fDetectorNumber != aRec.fDetectorNumber){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fDetectorNumber Not Equal. lhs: " 
			<< fDetectorNumber << " != rhs " << aRec.fDetectorNumber << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fDetectorNameSize != aRec.fDetectorNameSize){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fDetectorNameSize Not Equal. lhs: " 
			<< fDetectorNameSize << " != rhs " << aRec.fDetectorNameSize << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(strncmp(fDetectorName, aRec.fDetectorName, sizeof(Char_t)*fDetectorNameSize)){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fDetectorName Not Equal. lhs: " 
			<< fDetectorName << " != rhs " << aRec.fDetectorName << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	/*if(fHeatPositiveThreshold != aRec.fHeatPositiveThreshold){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fHeatPositiveThreshold Not Equal. lhs: " 
			<< fHeatPositiveThreshold << " != rhs " << aRec.fHeatPositiveThreshold << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}*/
	
	/*if(fHeatNegativeThreshold != aRec.fHeatNegativeThreshold){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fHeatNegativeThreshold Not Equal. lhs: " 
			<< fHeatNegativeThreshold << " != rhs " << aRec.fHeatNegativeThreshold << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}*/
	
	/*if(fTimeOfLastThresholdChange != aRec.fTimeOfLastThresholdChange){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fTimeOfLastThresholdChange Not Equal. lhs: " 
			<< fTimeOfLastThresholdChange << " != rhs " << aRec.fTimeOfLastThresholdChange << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}*/
	
	/*if(fVoie != aRec.fVoie){
		if (bPrint) 
			cout << "KSingleBoloSubRecord fVoie Not Equal. lhs: " 
			<< fVoie << " != rhs " << aRec.fVoie << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}*/
	
	return bIsEqual;
}


void KSingleBoloSubRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}


