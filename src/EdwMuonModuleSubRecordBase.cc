//_____________________________________________
//
// EdwMuonModuleSubRecordBase.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class of all Muon Module Sub Record classes. This base class
// contains a number of the data values for the Muon Module sub records.

#include "EdwMuonModuleSubRecordBase.h"
#include <iostream>
using namespace std;

ClassImp(EdwMuonModuleSubRecordBase);

EdwMuonModuleSubRecordBase::EdwMuonModuleSubRecordBase(void)
{
	InitializeMembers();
}

EdwMuonModuleSubRecordBase::~EdwMuonModuleSubRecordBase(void)
{
	Clear("C");
}

EdwMuonModuleSubRecordBase::EdwMuonModuleSubRecordBase(const EdwMuonModuleSubRecordBase &aRec)
: EdwSubRecordBase(aRec)
{
	CopyLocalMembers(aRec);
	
}

EdwMuonModuleSubRecordBase& EdwMuonModuleSubRecordBase::operator=(const EdwMuonModuleSubRecordBase &aRec)
{
	if(&aRec == this) return *this;
	
	this->EdwSubRecordBase::operator=(aRec);
	CopyLocalMembers(aRec);
	
	return *this;
}

void EdwMuonModuleSubRecordBase::CopyLocalMembers(const EdwMuonModuleSubRecordBase &aRec)
{
	SetModuleNumber(aRec.GetModuleNumber());
	SetTdc(0, aRec.GetTdc(0));
	SetAdc(0, aRec.GetAdc(0));
	SetTdc(1, aRec.GetTdc(1));
	SetAdc(1, aRec.GetAdc(1));
	SetAdcGroup(aRec.GetAdcGroup());
}

void EdwMuonModuleSubRecordBase::Clear(Option_t *anopt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	EdwSubRecordBase::Clear(anopt);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void EdwMuonModuleSubRecordBase::InitializeMembers(void)
{
	//Init local members. 
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	SetModuleNumber(-99);
	SetTdc(0, -99);
	SetAdc(0, -99);
	SetTdc(1, -99);
	SetAdc(1, -99);
	SetAdcGroup(-99);
}



Int_t EdwMuonModuleSubRecordBase::GetTdc(Int_t i) const
{
	if(i < kNumPmtsPerMuonModule)
		return fTdc[i];
	else
		return -1;
} 

Int_t EdwMuonModuleSubRecordBase::GetAdc(Int_t i) const
{
	if(i < kNumPmtsPerMuonModule && i >= 0)
		return fAdc[i];
	else
		return -1;
}

Int_t EdwMuonModuleSubRecordBase::SetTdc(Int_t i, Int_t val)
{
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fTdc[i] = val;
		return val;
	}
	else
		return -1;
}

Int_t EdwMuonModuleSubRecordBase::SetAdc(Int_t i, Int_t val)
{
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fAdc[i] = val;
		return val;
	}
	else
		return -1;
}

Bool_t EdwMuonModuleSubRecordBase::IsSame(const EdwMuonModuleSubRecordBase &aRec, Bool_t bPrint) const
{
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwSubRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fModuleNumber != aRec.fModuleNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwMuonModuleSubRecordBase fModuleNumber Not Equal. lhs: " 
			<< fModuleNumber << " != rhs " << aRec.fModuleNumber << endl;	
		else
			return false;  
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fTdc[i] != aRec.fTdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwMuonModuleSubRecordBase fTdc["<<i<<"] Not Equal. lhs: " 
				<< fTdc[i] << " != rhs " << aRec.fTdc[i] << endl;			
			else
				return false;  
		}
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fAdc[i] != aRec.fAdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwMuonModuleSubRecordBase fAdc["<<i<<"] Not Equal. lhs: " 
				<< fAdc[i] << " != rhs " << aRec.fAdc[i] << endl;			
			else
				return false;  
		}
	}
	
	if(fAdcGroup != aRec.fAdcGroup){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwMuonModuleSubRecordBase fAdcGroup Not Equal. lhs: " 
			<< fAdcGroup << " != rhs " << aRec.fAdcGroup << endl;		
		else
			return false;  
	}
	return bIsEqual;
}

Bool_t EdwMuonModuleSubRecordBase::IsNeutronCounter(void)
{
	//If fMuonModuleNumber is set to 50 or 51, then this event
	//contains information for the Neutron Counter and not for the Muon
	//Veto System. 
	
	return (fModuleNumber == 50 || fModuleNumber == 51);
}

void EdwMuonModuleSubRecordBase::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwSubRecordBase::Compact();
}

Bool_t EdwMuonModuleSubRecordBase::IsHardTrigger()
{
	//returns true if both TDC values have a value > -1.
	//Note, this means that a TDC value of 0 indicates a
	//trigger. 
	
	return (  (fTdc[0] > -1) && (fTdc[1] > -1)  );
}
