//_____________________________________________
//
// KHLABoloPulseRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The HLA class for all Pulse Sub Records. Currently, however, this data
// is not stored in the Event classes because we don't read directly from the Samba files
// at the moment. This will possibly change one day in the future. 
//

#include "KHLABoloPulseRecord.h"
#include <iostream>
using namespace std;

ClassImp(KHLABoloPulseRecord);

KHLABoloPulseRecord::KHLABoloPulseRecord(void)
{

  InitializeMembers();
}

KHLABoloPulseRecord::KHLABoloPulseRecord(const KHLABoloPulseRecord &aRec)
: KBoloPulseRecord(aRec)
{
	CopyLocalMembers(aRec);
	
}

KHLABoloPulseRecord& KHLABoloPulseRecord::operator=(const KHLABoloPulseRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KBoloPulseRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KHLABoloPulseRecord::CopyLocalMembers(const KHLABoloPulseRecord &aRec)
{
	//nothing to do
	
	//fBolometerRecordNum = aRec.fBolometerRecordNum;  have to set this value manually
	fBolometerRecord = 0;
	
	fEnergy = aRec.fEnergy;
	fEnergyBaseline = aRec.fEnergyBaseline;
	fBaselineNoise = aRec.fBaselineNoise;
}

KHLABoloPulseRecord::~KHLABoloPulseRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KHLABoloPulseRecord::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KBoloPulseRecord::Clear(opt);
	
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KHLABoloPulseRecord::InitializeMembers(void)
{
	//init local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	fBolometerRecord = 0; 
	
	fEnergy = -99;
	fEnergyBaseline = -99;
	fBaselineNoise = -99;

	
}

Bool_t KHLABoloPulseRecord::IsSame(const KHLABoloPulseRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KBoloPulseRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	//HMM...if the following lines of code produce a lot of Not Equal messages to cout, its probably
	//because I'm asking if two floating point values are exactly the same!
	//this is inherently dangerous.  its probably better to specify an allowed 
	//range that's on the order of machine error. 
	
	if(fEnergy != aRec.fEnergy){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABoloPulseRecord fEnergy Not Equal. lhs: " 
			<< fEnergy << " != rhs " << aRec.fEnergy << endl;		
		else
			return false;  
	}
	
	if(fEnergyBaseline != aRec.fEnergyBaseline){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABoloPulseRecord fEnergyBaseline Not Equal. lhs: " 
			<< fEnergyBaseline << " != rhs " << aRec.fEnergyBaseline << endl;		
		else
			return false;  
	}
	
	if(fBaselineNoise != aRec.fBaselineNoise){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABoloPulseRecord fBaselineNoise Not Equal. lhs: " 
			<< fBaselineNoise << " != rhs " << aRec.fBaselineNoise << endl;		
		else
			return false;  
	}
	
	
	return bIsEqual;
}


void KHLABoloPulseRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KBoloPulseRecord::Compact();

}


