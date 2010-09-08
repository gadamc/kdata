//_____________________________________________
//
// KBoloPulseRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Pulse Sub Records. Currently, however, this data
// is not stored in the Event classes because we don't read directly from the Samba files
// at the moment. This will possibly change one day in the future. 
//

#include "KBoloPulseRecord.h"
#include <iostream>
#include <cstring>

ClassImp(KBoloPulseRecord);

KBoloPulseRecord::KBoloPulseRecord(void)
{	
	
	InitializeMembers();
	
	//we must take care of members on the heap differently
	//than members created on the stack.
	//fChannelNameSize=0;
	//fChannelName=0;
	
}

KBoloPulseRecord::KBoloPulseRecord(const KBoloPulseRecord &aRec)
: KSubRecord(aRec)
{
	CopyLocalMembers(aRec);
	//fChannelNameSize=0;
	//fChannelName=0;
	//SetChannelName(aRec.GetChannelName().data(), aRec.GetChannelNameSize());
	
}

KBoloPulseRecord& KBoloPulseRecord::operator=(const KBoloPulseRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	
	//SetChannelName(aRec.GetChannelName().data(), aRec.GetChannelNameSize());
	
	return *this;
}

void KBoloPulseRecord::CopyLocalMembers(const KBoloPulseRecord &aRec)
{
	//SetPeakAmp(aRec.GetPeakAmp());
	//SetBaselineMean(aRec.GetBaselineMean());
	//SetBaselineFWHM(aRec.GetBaselineFWHM());
	fPulseType = aRec.fPulseType;
	fChannelNumber = aRec.fChannelNumber;
}

KBoloPulseRecord::~KBoloPulseRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");
	
	/*
	if(fChannelName!=0){
		delete[] fChannelName;
		fChannelNameSize=0;
		fChannelName=0;
	}
	 */
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
	/*if(fChannelName!=0){
		delete[] fChannelName;
		fChannelNameSize=0;
		fChannelName=0;
	}*/
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KBoloPulseRecord::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	//SetPeakAmp(-99.0);
	//SetBaselineMean(-99.0);
	//SetBaselineFWHM(-99.0);
	fPulseType = -99;   
	fChannelNumber = -99;
	
}


/*void KBoloPulseRecord::SetChannelName(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fChannelNameSize){
		if(fChannelName!=0){
			delete[] fChannelName;
			fChannelName = 0;
			fChannelNameSize = 0;
		}
		if(aSize>0) fChannelName= new Char_t[aSize];
		fChannelNameSize=aSize;
	}
	if(aSize>0) memcpy(fChannelName, aWord, fChannelNameSize*sizeof(Char_t));
}
*/
Bool_t KBoloPulseRecord::IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint) const
{
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
	
	/*if(fChannelNameSize != aRec.fChannelNameSize){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fChannelNameSize Not Equal. lhs: " 
			<< fChannelNameSize << " != rhs " << aRec.fChannelNameSize << endl;		
		else
			return false;  
	}
	
	if(strncmp(fChannelName, aRec.fChannelName, sizeof(Char_t)*fChannelNameSize)){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fChannelName Not Equal. lhs: " 
			<< fChannelName << " != rhs " << aRec.fChannelName << endl;		
		else
			return false;  
	}
	*/
	
	/*if(fPeakAmp != aRec.fPeakAmp){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fPeakAmp Not Equal. lhs: " 
			<< fPeakAmp << " != rhs " << aRec.fPeakAmp << endl;		
		else
			return false;  
	}
	
	if(fBaselineMean != aRec.fBaselineMean){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fBaselineMean Not Equal. lhs: " 
			<< fBaselineMean << " != rhs " << aRec.fBaselineMean << endl;		
		else
			return false;  
	}
	
	if(fBaselineFWHM != aRec.fBaselineFWHM){
		bIsEqual = false;
		if (bPrint) 
			cout << "KBoloPulseRecord fBaselineFWHM Not Equal. lhs: " 
			<< fBaselineFWHM << " != rhs " << aRec.fBaselineFWHM << endl;		
		else
			return false;  
	}*/
	
	return bIsEqual;
}


void KBoloPulseRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}
