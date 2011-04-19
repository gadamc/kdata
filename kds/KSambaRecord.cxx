//_____________________________________________
//
// KSambaRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Samba Sub Records. This contains event information
// that is from the particular samba machine that the data was acquired on.
// While Raw/HLA level record classes that derive from this class exist,
// they do not, at the moment, contain any information that is not stored
// in this class. However, your analysis routine should continue to use
// the Raw/HLA pointers 
//

#include "KSambaRecord.h"
#include <stdio.h>
#include <iostream>
#include <cstring>

ClassImp(KSambaRecord);

KSambaRecord::KSambaRecord(void)
{
  //constructor
  
	InitializeMembers();
	
	//We must deal with members on the heap separately 
	//from members on the stack

}


KSambaRecord::KSambaRecord(const KSambaRecord &aRec)
: KSubRecord(aRec)
{
  //copy constructor
  

	CopyLocalMembers(aRec);

}

KSambaRecord& KSambaRecord::operator=(const KSambaRecord &aRec)
{
  //assignment operator
  
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
		
	return *this;
}

void KSambaRecord::CopyLocalMembers(const KSambaRecord &aRec)
{
	fSambaEventNumber = aRec.fSambaEventNumber; 
	fNtpDateSec = aRec.fNtpDateSec;
	fNtpDateMicroSec = aRec.fNtpDateMicroSec; 
	fSambaDAQNumber = aRec.fSambaDAQNumber;
	fRunName = aRec.fRunName;
	fFileNumber = aRec.fFileNumber;
  fRegenerationFlag = aRec.fRegenerationFlag;
  fTotalDeadTimeSec = aRec.fTotalDeadTimeSec;
  fTotalDeadTimeMicroSec = aRec.fTotalDeadTimeMicroSec;
  fTemperature = aRec.fTemperature;
  fRunType = aRec.fRunType;
  fRunCondition = aRec.fRunCondition;
  fRunStartPcTimeSec = aRec.fRunStartPcTimeSec;
  fRunStartPcTimeMicroSec = aRec.fRunStartPcTimeMicroSec;
  fRunStartTriggerStamp = aRec.fRunStartTriggerStamp;
  fSource1Regen = aRec.fSource1Regen;
  fSource2Regen = aRec.fSource2Regen;
  fSource1Calib = aRec.fSource1Calib;
  fSource2Calib = aRec.fSource2Calib;
}

KSambaRecord::~KSambaRecord(void)
{
  //destructor
  
	Clear("C");
	
	
}

void KSambaRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

	KSubRecord::Clear(opt);
	
  //Clear and delete local objects here. 
	//delete local stuff here

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KSambaRecord::SetRunName(const Char_t* aWord)
{
	
	fRunName = aWord;

}

string KSambaRecord::GetRunName(void) const 
{
	
	return fRunName;
}

void KSambaRecord::InitializeMembers(void)
{
	//Init local members
	
	//WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetSambaEventNumber(-99); 
	SetNtpDateSec(-99); 
	SetNtpDateMicroSec(-99); 
	SetSambaDAQNumber(-99);
	fFileNumber = -99;
  fRunName.resize(0);
  fFileNumber = -99; //such as 000, 001, 002, ....
  fRegenerationFlag = 0;
  fTotalDeadTimeSec = -99;
  fTotalDeadTimeMicroSec = -99;
  fTemperature = -99;
  
  //samba run header information
  
  fRunType.resize(0);
  fRunCondition.resize(0);
  fRunStartPcTimeSec = -99;
  fRunStartPcTimeMicroSec = -99;
  fRunStartTriggerStamp = -99;
  fSource1Regen.resize(0);
  fSource2Regen.resize(0);
  fSource1Calib.resize(0);
  fSource2Calib.resize(0);
}


Bool_t KSambaRecord::IsSame(const KSambaRecord &aRec, Bool_t bPrint) const
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
	
	if(fSambaEventNumber != aRec.fSambaEventNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSambaEventNumber Not Equal. lhs: " 
			<< fSambaEventNumber << " != rhs " << aRec.fSambaEventNumber << endl;	
		else
			return false;  
	}
	
	if(fNtpDateSec != aRec.fNtpDateSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fNtpDateSec Not Equal. lhs: " 
			<< fNtpDateSec << " != rhs " << aRec.fNtpDateSec << endl;		
		else
			return false;  
	}
	
	if(fNtpDateMicroSec != aRec.fNtpDateMicroSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fNtpDateMicroSec Not Equal. lhs: " 
			<< fNtpDateMicroSec << " != rhs " << aRec.fNtpDateMicroSec << endl;		
		else
			return false;  
	}
	
	if(fSambaDAQNumber != aRec.fSambaDAQNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSambaDAQNumber Not Equal. lhs: " 
			<< fSambaDAQNumber << " != rhs " << aRec.fSambaDAQNumber << endl;		
		else
			return false;  
	}
	
  if(fRegenerationFlag != aRec.fRegenerationFlag){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRegenerationFlag Not Equal. lhs: " 
			<< fRegenerationFlag << " != rhs " << aRec.fRegenerationFlag << endl;		
		else
			return false;  
	}
  if(fTotalDeadTimeSec != aRec.fTotalDeadTimeSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fTotalDeadTimeSec Not Equal. lhs: " 
			<< fTotalDeadTimeSec << " != rhs " << aRec.fTotalDeadTimeSec << endl;		
		else
			return false;  
	}
  if(fTotalDeadTimeMicroSec != aRec.fTotalDeadTimeMicroSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fTotalDeadTimeMicroSec Not Equal. lhs: " 
			<< fTotalDeadTimeMicroSec << " != rhs " << aRec.fTotalDeadTimeMicroSec << endl;		
		else
			return false;  
	}
  if(fTemperature != aRec.fTemperature){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fTemperature Not Equal. lhs: " 
			<< fTemperature << " != rhs " << aRec.fTemperature << endl;		
		else
			return false;  
	}
  
	if(fRunName != aRec.fRunName){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunName Not Equal. lhs: " 
			<< fRunName << " != rhs " << aRec.fRunName << endl;		
		else
			return false;  
	}
	
  if(fFileNumber != aRec.fFileNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fFileNumber Not Equal. lhs: " 
			<< fFileNumber << " != rhs " << aRec.fFileNumber << endl;		
		else
			return false;  
	}
  
  if(fRunType != aRec.fRunType){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunType Not Equal. lhs: " 
			<< fRunType << " != rhs " << aRec.fRunType << endl;		
		else
			return false;  
	}
  
  if(fRunCondition != aRec.fRunCondition){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunCondition Not Equal. lhs: " 
			<< fRunCondition << " != rhs " << aRec.fRunCondition << endl;		
		else
			return false;  
	}
  
  if(fRunStartPcTimeSec != aRec.fRunStartPcTimeSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunStartPcTimeSec Not Equal. lhs: " 
			<< fRunStartPcTimeSec << " != rhs " << aRec.fRunStartPcTimeSec << endl;		
		else
			return false;  
	}
  
  if(fRunStartPcTimeMicroSec != aRec.fRunStartPcTimeMicroSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunStartPcTimeMicroSec Not Equal. lhs: " 
			<< fRunStartPcTimeMicroSec << " != rhs " << aRec.fRunStartPcTimeMicroSec << endl;		
		else
			return false;  
	}
  
  if(fRunStartTriggerStamp != aRec.fRunStartTriggerStamp){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fRunStartTriggerStamp Not Equal. lhs: " 
			<< fRunStartTriggerStamp << " != rhs " << aRec.fRunStartTriggerStamp << endl;		
		else
			return false;  
	}
  
  if(fSource1Regen != aRec.fSource1Regen){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSource1Regen Not Equal. lhs: " 
			<< fSource1Regen << " != rhs " << aRec.fSource1Regen << endl;		
		else
			return false;  
	}
  
  if(fSource2Regen != aRec.fSource2Regen){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSource2Regen Not Equal. lhs: " 
			<< fSource2Regen << " != rhs " << aRec.fSource2Regen << endl;		
		else
			return false;  
	}
  
  if(fSource1Calib != aRec.fSource1Calib){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSource1Calib Not Equal. lhs: " 
			<< fSource1Calib << " != rhs " << aRec.fSource1Calib << endl;		
		else
			return false;  
	}
  
  if(fSource2Calib != aRec.fSource2Calib){
		bIsEqual = false;
		if (bPrint) 
			cout << "KSambaRecord fSource2Calib Not Equal. lhs: " 
			<< fSource2Calib << " != rhs " << aRec.fSource2Calib << endl;		
		else
			return false;  
	}
  
	return bIsEqual;
}


void KSambaRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSubRecord::Compact();
}

void KSambaRecord::print(void)
{
  //should probably change this to be the operator<< instead of
  //print. 
  
	cout << "KSambaRecord. this = 0x" << this << endl;
	//cout << "Me* " << *this << endl;
	cout << "fSambaEventNumber " << fSambaEventNumber << endl;
	cout << "fNtpDataSec " << fNtpDateSec << endl;
	cout << "fSambaDAQNumber "  << fSambaDAQNumber << endl;
	cout << "fRunName " << fRunName << endl;
	
}
