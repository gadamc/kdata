//_____________________________________________
//
// KMuonModuleSubRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class of all Muon Module Sub Record classes. This base class
// contains a number of the data values for the Muon Module sub records.

#include "KMuonModuleSubRecord.h"
#include <iostream>
using namespace std;

ClassImp(KMuonModuleSubRecord);

KMuonModuleSubRecord::KMuonModuleSubRecord(void)
{
	InitializeMembers();
}

KMuonModuleSubRecord::~KMuonModuleSubRecord(void)
{
	Clear("C");
}

KMuonModuleSubRecord::KMuonModuleSubRecord(const KMuonModuleSubRecord &aRec)
: KSubRecord(aRec)
{
	CopyLocalMembers(aRec);
	
}

KMuonModuleSubRecord& KMuonModuleSubRecord::operator=(const KMuonModuleSubRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	
	return *this;
}

void KMuonModuleSubRecord::CopyLocalMembers(const KMuonModuleSubRecord &aRec)
{
	SetModuleNumber(aRec.GetModuleNumber());
	SetTdc(0, aRec.GetTdc(0));
	SetAdc(0, aRec.GetAdc(0));
	SetTdc(1, aRec.GetTdc(1));
	SetAdc(1, aRec.GetAdc(1));
	SetAdcGroup(aRec.GetAdcGroup());
}

void KMuonModuleSubRecord::Clear(Option_t *anopt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KSubRecord::Clear(anopt);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KMuonModuleSubRecord::InitializeMembers(void)
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



Int_t KMuonModuleSubRecord::GetTdc(Int_t i) const
{
	if(i < kNumPmtsPerMuonModule)
		return fTdc[i];
	else
		return -1;
} 

Int_t KMuonModuleSubRecord::GetAdc(Int_t i) const
{
	if(i < kNumPmtsPerMuonModule && i >= 0)
		return fAdc[i];
	else
		return -1;
}

Int_t KMuonModuleSubRecord::SetTdc(Int_t i, Int_t val)
{
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fTdc[i] = val;
		return val;
	}
	else
		return -1;
}

Int_t KMuonModuleSubRecord::SetAdc(Int_t i, Int_t val)
{
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fAdc[i] = val;
		return val;
	}
	else
		return -1;
}

Bool_t KMuonModuleSubRecord::IsSame(const KMuonModuleSubRecord &aRec, Bool_t bPrint) const
{
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KSubRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fModuleNumber != aRec.fModuleNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonModuleSubRecord fModuleNumber Not Equal. lhs: " 
			<< fModuleNumber << " != rhs " << aRec.fModuleNumber << endl;	
		else
			return false;  
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fTdc[i] != aRec.fTdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KMuonModuleSubRecord fTdc["<<i<<"] Not Equal. lhs: " 
				<< fTdc[i] << " != rhs " << aRec.fTdc[i] << endl;			
			else
				return false;  
		}
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fAdc[i] != aRec.fAdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KMuonModuleSubRecord fAdc["<<i<<"] Not Equal. lhs: " 
				<< fAdc[i] << " != rhs " << aRec.fAdc[i] << endl;			
			else
				return false;  
		}
	}
	
	if(fAdcGroup != aRec.fAdcGroup){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonModuleSubRecord fAdcGroup Not Equal. lhs: " 
			<< fAdcGroup << " != rhs " << aRec.fAdcGroup << endl;		
		else
			return false;  
	}
	return bIsEqual;
}

Bool_t KMuonModuleSubRecord::IsNeutronCounter(void)
{
	//If fMuonModuleNumber is set to 50 or 51, then this event
	//contains information for the Neutron Counter and not for the Muon
	//Veto System. 
	
	return (fModuleNumber == 50 || fModuleNumber == 51);
}

void KMuonModuleSubRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}

Bool_t KMuonModuleSubRecord::IsHardTrigger()
{
	//returns true if both TDC values have a value > -1.
	//Note, this means that a TDC value of 0 indicates a
	//trigger. 
	
	return (  (fTdc[0] > -1) && (fTdc[1] > -1)  );
}
