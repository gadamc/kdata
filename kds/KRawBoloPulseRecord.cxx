//______________________________________________________________________
//
// KRawBoloPulseRecord.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Holds the Raw-level pulse information, including the pulse trace
// This class is based on KBoloPulseRecord (which stores the channel
// number and type). 
//


#include "KRawBoloPulseRecord.h"
#include <iostream>

using namespace std;

ClassImp(KRawBoloPulseRecord);

KRawBoloPulseRecord::KRawBoloPulseRecord(void)
{
  //standard constructor
  
  InitializeMembers();
}


KRawBoloPulseRecord::KRawBoloPulseRecord(const KRawBoloPulseRecord &aRec)
: KBoloPulseRecord(aRec)
{
  //copy constructor 
  
	CopyLocalMembers(aRec);
	
}

KRawBoloPulseRecord& KRawBoloPulseRecord::operator=(const KRawBoloPulseRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KBoloPulseRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KRawBoloPulseRecord::CopyLocalMembers(const KRawBoloPulseRecord &aRec)
{
	//used in the assignment operator method, this copies over the local 
  //data members. It also set sets fBolometerRecord (the TRef pointer) to zero.
  
	//fBolometerRecordNum = aRec.fBolometerRecordNum;  have to set this value manually
	fBolometerRecord = 0;
	
  fChannelName = aRec.fChannelName;  
  fPulseTimeWidth = aRec.fPulseTimeWidth;  
  fTrace = aRec.fTrace;   
  fAmplitude = aRec.fAmplitude;   
  fAmplitudeBaseline = aRec.fAmplitudeBaseline;   
  fAmplitudeBaselineNoise = aRec.fAmplitudeBaselineNoise;   
 
}


KRawBoloPulseRecord::~KRawBoloPulseRecord(void)
{
  //destructor

}

void KRawBoloPulseRecord::Clear(Option_t* /* opt */)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KRawBoloPulseRecord::InitializeMembers(void)
{
  //init local members.
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  
  fChannelName = "";  //the name of the channel. for example: "chaleur ID4"
  fPulseTimeWidth = 10; //returns number of ns for each point.  = 1 / f
  fTrace.clear(); //the raw trace.
  fAmplitude = -1; //the pulse amplitude calcuated by the DAQ
  fAmplitudeBaseline = -1; //the baseline amplitude calculated by DAQ
  fAmplitudeBaselineNoise = -1; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
 
  
}

Bool_t KRawBoloPulseRecord::IsSame(const KRawBoloPulseRecord &aRec, Bool_t bPrint) const
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
	
	if(fChannelName != aRec.fChannelName){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fChannelName Not Equal. lhs: " 
			<< fChannelName << " != rhs " << aRec.fChannelName << endl;		
		else
			return false;  
	}
	
	if(fPulseTimeWidth != aRec.fPulseTimeWidth){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fPulseTimeWidth Not Equal. lhs: " 
			<< fPulseTimeWidth << " != rhs " << aRec.fPulseTimeWidth << endl;		
		else
			return false;  
	}
  
  if(fTrace != aRec.fTrace){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fTrace Not Equal. " << endl;
		else
			return false;  
	}
  
	
	if(fAmplitude != aRec.fAmplitude){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitude Not Equal. lhs: " 
			<< fAmplitude << " != rhs " << aRec.fAmplitude << endl;		
		else
			return false;  
	}
	
  if(fAmplitudeBaseline != aRec.fAmplitudeBaseline){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitudeBaseline Not Equal. lhs: " 
			<< fAmplitudeBaseline << " != rhs " << aRec.fAmplitudeBaseline << endl;		
		else
			return false;  
	}
  
  if(fAmplitudeBaselineNoise != aRec.fAmplitudeBaselineNoise){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitudeBaselineNoise Not Equal. lhs: " 
			<< fAmplitudeBaselineNoise << " != rhs " << aRec.fAmplitudeBaselineNoise << endl;		
		else
			return false;  
	}
  
	
	return bIsEqual;
}


void KRawBoloPulseRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KBoloPulseRecord::Compact();
  
}

void KRawBoloPulseRecord::SetTrace(UInt_t size, const Short_t* aData)
{
  fTrace.clear();
  for(Int_t i = 0; i < size; i++)
    fTrace.push_back(aData[i]);
}


