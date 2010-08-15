//_____________________________________________
//
// KHLAMuonModuleSubRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The HLA class for Muon Module Sub Records. It only stores two values
// for the calibrated energy deposited in the Module. However, no such calibration
// exists and these values are always initialized to -99, taking up space in the data
// file, no doubt. 
//

#include "KHLAMuonModuleSubRecord.h"
#include <iostream>
using namespace std;

ClassImp(KHLAMuonModuleSubRecord);

KHLAMuonModuleSubRecord::KHLAMuonModuleSubRecord(void)
{

  InitializeMembers();
}

KHLAMuonModuleSubRecord::KHLAMuonModuleSubRecord(const KHLAMuonModuleSubRecord &aRec)
: KMuonModuleSubRecord(aRec)
{
	CopyLocalMembers(aRec);
	
}

KHLAMuonModuleSubRecord& KHLAMuonModuleSubRecord::operator=(const KHLAMuonModuleSubRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KMuonModuleSubRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KHLAMuonModuleSubRecord::CopyLocalMembers(const KHLAMuonModuleSubRecord &aRec)
{
	//SetEnergy(0, aRec.GetEnergy(0));
	//SetEnergy(1, aRec.GetEnergy(1));
	
}

KHLAMuonModuleSubRecord::~KHLAMuonModuleSubRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KHLAMuonModuleSubRecord::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KMuonModuleSubRecord::Clear(opt);
  
	//Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KHLAMuonModuleSubRecord::InitializeMembers(void)
{
	//init local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	//SetEnergy(0, -99.0);
	//SetEnergy(1, -99.0);
}

Bool_t KHLAMuonModuleSubRecord::IsSame(const KHLAMuonModuleSubRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KMuonModuleSubRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	
	
	/*for(Int_t i = 0; i < kNumPmtsPerMuonModule; i++){
		if(fEnergy[i] != aRec.fEnergy[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLAMuonModuleSubRecord fEnergy["<<i<<"] Not Equal. lhs: " 
				<< fEnergy[i] << " != rhs " << aRec.fEnergy[i] << endl;		
			else
				return false;  
		}
	}*/
	
	return bIsEqual;
}

void KHLAMuonModuleSubRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KMuonModuleSubRecord::Compact();
	
}


