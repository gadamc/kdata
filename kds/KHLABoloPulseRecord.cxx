//_____________________________________________
//
// KHLABoloPulseRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The HLA class for all Pulse Sub Records. The 'high-level' class
// contains variables that are derived from analysis of raw-level data.
// The data stored here are the results of a particular pulse-trace analysis
// and energy calibration. (At this time, only the calcualted pulse energies
// from the ERA analysis process are stored here. More values could be added,
// or perhaps we could think of a better storage solution in the future.) 
// 
// Stored here are the energy (fEnergy), the energy of the pulse in the baseline region
// (fEnergyBaseline), and the standard deviation of the distribution of energies measured in the 
// baseline region for this particular analysis period (fBaselineNoise). 
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
//
// In addition, there is a TRef object that will point you back to the 
// KHLABolometerRecord that this pulse record is associated with.


#include "KHLABoloPulseRecord.h"
#include <iostream>
using namespace std;

ClassImp(KHLABoloPulseRecord);

KHLABoloPulseRecord::KHLABoloPulseRecord(void)
{
  //standard constructor
  
  InitializeMembers();
}

KHLABoloPulseRecord::KHLABoloPulseRecord(const KHLABoloPulseRecord &aRec)
: KBoloPulseRecord(aRec)
{
  //copy constructor 
  
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
	//used in the assignment operator method, this copies over the local 
  //data members. 

  	fEnergy = aRec.fEnergy;
	fEnergyBaseline = aRec.fEnergyBaseline;
	fBaselineNoise = aRec.fBaselineNoise;
  
  
	fPulseType = aRec.fPulseType;
	fChannelNumber = aRec.fChannelNumber;
  
}

KHLABoloPulseRecord::~KHLABoloPulseRecord(void)
{
  //destructor 
  
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
	
	
	fEnergy = -99;
	fEnergyBaseline = -99;
	fBaselineNoise = -99;

  fPulseType = -99;   
	fChannelNumber = -99;
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
	
  if(fPulseType != aRec.fPulseType){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABoloPulseRecord fPulseType Not Equal. lhs: " 
			<< fPulseType << " != rhs " << aRec.fPulseType << endl;		
		else
			return false;  
	}
	
	if(fChannelNumber != aRec.fChannelNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABoloPulseRecord fChannelNumber Not Equal. lhs: " 
			<< fChannelNumber << " != rhs " << aRec.fChannelNumber << endl;		
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


