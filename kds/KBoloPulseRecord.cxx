//_____________________________________________
//
// KBoloPulseRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Pulse Sub Records. This class holds the channel number (1 or 2)
// and the pulse type information (heat, collectrode, veto, guard). Typically,
// one will work with a KHLABoloPulseRecord or KRawBoloPulseRecord, which are
// derived from this class.
// 
// So you don't have to memorize the numerical value that corresponds
// to each pulse type, the methods GetHeatType(), GetCollectrodeType(),
// GetVetoType() and GetGuardType() are available to return the numeric value
// stored in a local enum structure. 
// however, for your information, the structure is
//
// enum kPulseChannelType {
//   kCollectrodeType = 1,
//   kVetoType = 2,
//   kGuardType = 3,
//   kHeatType = 4
//  };
//
// The channel number should be either 1 or 2, corresponding to both channels
// in the bolometer data. Note, there is no channel 0. In principle, there could
// be 3 or more channels of a particular type as well (in the case of a future
// bolometer design?). 

#include "KBoloPulseRecord.h"
#include <iostream>
#include <cstring>

ClassImp(KBoloPulseRecord);

KBoloPulseRecord::KBoloPulseRecord(void)
{	
	//standard constructors 
  
	InitializeMembers();
	
	//we must take care of members on the heap differently
	//than members created on the stack.
		
}

KBoloPulseRecord::KBoloPulseRecord(const KBoloPulseRecord &aRec)
: KSubRecord(aRec)
{
  //copy constructor 
  
	CopyLocalMembers(aRec);
		
}

KBoloPulseRecord& KBoloPulseRecord::operator=(const KBoloPulseRecord &aRec)
{
  //assignment operator
  
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
		
	return *this;
}

void KBoloPulseRecord::CopyLocalMembers(const KBoloPulseRecord &aRec)
{
  //used by the assignment operator to copy local members (fPulseType and
  //fChanneNumber)
  
	fPulseType = aRec.fPulseType;
	fChannelNumber = aRec.fChannelNumber;
}

KBoloPulseRecord::~KBoloPulseRecord(void)
{
  //destructor
  
	//Does calling clear at destruction take too much computing time?
  Clear("C");
	
}

void KBoloPulseRecord::Clear(Option_t *anopt)
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

void KBoloPulseRecord::InitializeMembers(void)
{
  //init local members to default values (-99)
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	fPulseType = -99;   
	fChannelNumber = -99;
	
}


Bool_t KBoloPulseRecord::IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint) const
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
	
	if(fPulseType != aRec.fPulseType){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fPulseType Not Equal. lhs: " 
			<< fPulseType << " != rhs " << aRec.fPulseType << endl;		
		else
			return false;  
	}
	
	if(fChannelNumber != aRec.fChannelNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fChannelNumber Not Equal. lhs: " 
			<< fChannelNumber << " != rhs " << aRec.fChannelNumber << endl;		
		else
			return false;  
	}
		
	return bIsEqual;
}


void KBoloPulseRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}
