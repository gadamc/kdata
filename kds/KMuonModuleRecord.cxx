//_____________________________________________
//
// KMuonModuleRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class of all Muon Module Sub Record classes. This base class
// contains a all of the data values for the Muon Module sub records.
// The HLA and Raw level records, at the moment, do not add any information
// beyond this class. However, you should continue to access the Raw/HLA data
// pointers for your analysis in case more data is added. 

#include "KMuonModuleRecord.h"
#include <iostream>
using namespace std;

ClassImp(KMuonModuleRecord);

KMuonModuleRecord::KMuonModuleRecord(void)
{
  //default constructor
  
	InitializeMembers();
}

KMuonModuleRecord::~KMuonModuleRecord(void)
{
  //destructor
  
	Clear("C");
}

KMuonModuleRecord::KMuonModuleRecord(const KMuonModuleRecord &aRec)
  :TObject()
{
  //copy constructor
  
	CopyLocalMembers(aRec);
	
}

KMuonModuleRecord& KMuonModuleRecord::operator=(const KMuonModuleRecord &aRec)
{
  //assignment operator
  
	if(&aRec == this) return *this;
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KMuonModuleRecord::CopyLocalMembers(const KMuonModuleRecord &aRec)
{
	SetModuleNumber(aRec.GetModuleNumber());
	SetTdc(0, aRec.GetTdc(0));
	SetAdc(0, aRec.GetAdc(0));
	SetTdc(1, aRec.GetTdc(1));
	SetAdc(1, aRec.GetAdc(1));
	SetAdcGroup(aRec.GetAdcGroup());
}

void KMuonModuleRecord::Clear(Option_t *anopt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  TObject::Clear(anopt);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KMuonModuleRecord::InitializeMembers(void)
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



Int_t KMuonModuleRecord::GetTdc(Int_t i) const
{
  //returns the TDC value for this muon module for channel i. 
  //i can be either 0 or 1. 0 corresponds to the channel end 
  //that is listed first in the database (currently an Excel file)
  //and 1 corresponds to the second channel listed.
  //If a -1 is returned, this is because the value was not set in the 
  //data stream, or i was invalid (not equal to 0 or 1).
  
	if(i < kNumPmtsPerMuonModule && i >= 0)
		return fTdc[i];
	else
		return -1;
} 

Int_t KMuonModuleRecord::GetAdc(Int_t i) const
{
  //returns the ADC value for this muon module for channel i. 
  //i can be either 0 or 1. 0 corresponds to the channel end 
  //that is listed first in the database (currently an Excel file)
  //and 1 corresponds to the second channel listed.
  //If a -1 is returned, this is because the value was not set in the 
  //data stream, or i was invalid (not equal to 0 or 1).
  
	if(i < kNumPmtsPerMuonModule && i >= 0)
		return fAdc[i];
	else
		return -1;
}

Int_t KMuonModuleRecord::SetTdc(Int_t i, Int_t val)
{
  //sets the TDC value for this muon module for channel i. 
  //i can be either 0 or 1. 0 corresponds to the channel end 
  //that is listed first in the database (currently an Excel file)
  //and 1 corresponds to the second channel listed.
  //the val is returns if successfule
  //If a -1 is returned, this is i was invalid (not equal to 0 or 1).

  
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fTdc[i] = val;
		return val;
	}
	else
		return -1;
}

Int_t KMuonModuleRecord::SetAdc(Int_t i, Int_t val)
{
  //sets the ADC value for this muon module for channel i. 
  //i can be either 0 or 1. 0 corresponds to the channel end 
  //that is listed first in the database (currently an Excel file)
  //and 1 corresponds to the second channel listed.
  //the val is returns if successfule
  //If a -1 is returned, this is i was invalid (not equal to 0 or 1).

  
	if(i < kNumPmtsPerMuonModule && i >= 0){
		fAdc[i] = val;
		return val;
	}
	else
		return -1;
}

Bool_t KMuonModuleRecord::IsSame(const KMuonModuleRecord &aRec, Bool_t bPrint) const
{
  //same as the comparison operator, but gives you the opportunity
  //for it to be verbose and list to standard out the differences.
  //Use bPrint = true for verbosity. 
  
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	
	
	if(fModuleNumber != aRec.fModuleNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonModuleRecord fModuleNumber Not Equal. lhs: " 
			<< fModuleNumber << " != rhs " << aRec.fModuleNumber << endl;	
		else
			return false;  
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fTdc[i] != aRec.fTdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KMuonModuleRecord fTdc["<<i<<"] Not Equal. lhs: " 
				<< fTdc[i] << " != rhs " << aRec.fTdc[i] << endl;			
			else
				return false;  
		}
	}
	
	for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fAdc[i] != aRec.fAdc[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KMuonModuleRecord fAdc["<<i<<"] Not Equal. lhs: " 
				<< fAdc[i] << " != rhs " << aRec.fAdc[i] << endl;			
			else
				return false;  
		}
	}
	
	if(fAdcGroup != aRec.fAdcGroup){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonModuleRecord fAdcGroup Not Equal. lhs: " 
			<< fAdcGroup << " != rhs " << aRec.fAdcGroup << endl;		
		else
			return false;  
	}
	return bIsEqual;
}

Bool_t KMuonModuleRecord::IsNeutronCounter(void)
{
	//If fMuonModuleNumber is set to 50 or 51, then this event
	//contains information for the Neutron Counter and not for the Muon
	//Veto System. 
	
	return (fModuleNumber == 50 || fModuleNumber == 51);
}

void KMuonModuleRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
}

Bool_t KMuonModuleRecord::IsHardTrigger()
{
	//returns true if both TDC and ADC values have a value > -1.
	//Note, this means that a TDC value of 0 indicates a
	//trigger. 
	
	return (  (fTdc[0] > -1) && (fTdc[1] > -1) && (fAdc[0] > -1) && (fAdc[1] > -1) );
}
