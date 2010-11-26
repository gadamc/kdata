//_____________________________________________
//
// KMuonVetoSystemRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>  on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the base class for all Muon Veto System Records.
// Many of the Muon Veto System specific values are found here. 
//

#include "KMuonVetoSystemRecord.h"
#include <iostream>
using namespace std;

const Int_t KMuonVetoSystemRecord::kReconstructedTimeRunBoundary = 60;

ClassImp(KMuonVetoSystemRecord);

KMuonVetoSystemRecord::KMuonVetoSystemRecord(void)
{

  InitializeMembers();
}

KMuonVetoSystemRecord::KMuonVetoSystemRecord(const KMuonVetoSystemRecord &aRec)
:KSystemRecord(aRec)
{
	CopyLocalMembers(aRec);
}

KMuonVetoSystemRecord& KMuonVetoSystemRecord::operator=(const KMuonVetoSystemRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSystemRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	return *this;
}

void KMuonVetoSystemRecord::CopyLocalMembers(const KMuonVetoSystemRecord &aRec)
{
	//copies only the local members. used in the overloaded assignment operator. 
  
	fRunNumber = aRec.fRunNumber;
	fMultiADC = aRec.fMultiADC;  
	fDistanceEst = aRec.fDistanceEst; 
	fDistanceNemo = aRec.fDistanceNemo; 
	fEventNumber = aRec.fEventNumber;  
	fFileNumber = aRec.fFileNumber;  
	fMultiTDC = aRec.fMultiTDC; 
	fPcTimeSec = aRec.fPcTimeSec;
	fPcTimeMuSec = aRec.fPcTimeMuSec;
	fCommonStopTime = aRec.fCommonStopTime;
	fEventQuality = aRec.fEventQuality;
	fEventQuality.Compact();
	
	fRunStartTime = aRec.fRunStartTime;
	fRunEndTime = aRec.fRunEndTime;
  fFileStartTime = aRec.fFileStartTime;
	fFileEndTime = aRec.fFileEndTime;
	
}

KMuonVetoSystemRecord::~KMuonVetoSystemRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KMuonVetoSystemRecord::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KSystemRecord::Clear(opt);
	
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KMuonVetoSystemRecord::InitializeMembers(void)
{
	//Initialize local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	SetMultiADC(-99);
	SetDistanceEst(-99.0);
	SetDistanceNemo(-99.0);
	SetEventNumber(0);
	SetFileNumber(-99);
	SetMultiTDC(-99);
	SetPcTimeSec(-99);
	SetPcTimeMuSec(-99);
	SetRunNumber(-99);
	SetCommonStopTime(-99);
	fEventQuality.Clear();
	fRunStartTime = -99.;
	fRunEndTime = -99.;
  fFileStartTime = -99.;
	fFileEndTime = -99.;
}

Bool_t KMuonVetoSystemRecord::IsTimeReconstructed(void) const
{
	//This checks a hard-coded run number, kReconstructedTimeRunBoundary = 60
	//and compares it to the value returned by GetRunNumber. If the Muon Veto System run is less
	//than 60, then it returns true, meaning that the stamp time of the Muon
	//veto system events were reconstructed. Otherwise it returns false,
  //meaning that for this data, the stamp values were obtained from hardware
  //during data acquisition. 
	
	return (GetRunNumber() > kReconstructedTimeRunBoundary) ? true : false;
}

void KMuonVetoSystemRecord::SetEventQualityBits(TBits *aBits)
{
  //Copies the values in aBits to the local fEventQuality. It then calls
  //fEventQuality.Compact() in order to minimize space. 
  
	fEventQuality = *aBits;
	fEventQuality.Compact();
}

Bool_t KMuonVetoSystemRecord::IsSame(const KMuonVetoSystemRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KSystemRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fRunNumber != aRec.fRunNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fRunNumber Not Equal. lhs: " 
			<< fRunNumber << " != rhs " << aRec.fRunNumber << endl;		
		else
			return false;  
	}

	
	if(fMultiADC != aRec.fMultiADC){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fMultiADC Not Equal. lhs: " 
			<< fMultiADC << " != rhs " << aRec.fMultiADC << endl;		
		else
			return false;  
	}
	
	if(fDistanceEst != aRec.fDistanceEst){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fDistanceEst Not Equal. lhs: " 
			<< fDistanceEst << " != rhs " << aRec.fDistanceEst << endl;	
		else
			return false;  
	}
	
	if(fDistanceNemo != aRec.fDistanceNemo){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fDistanceNemo Not Equal. lhs: " 
			<< fDistanceNemo << " != rhs " << aRec.fDistanceNemo << endl;		
		else
			return false;  
	}
	
	if(fEventNumber != aRec.fEventNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fEventNumber Not Equal. lhs: " 
			<< fEventNumber << " != rhs " << aRec.fEventNumber << endl;		
		else
			return false;  
	}
	
	if(fFileNumber != aRec.fFileNumber){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fFileNumber Not Equal. lhs: " 
			<< fFileNumber << " != rhs " << aRec.fFileNumber << endl;	
		else
			return false;  
	}
	
	if(fMultiTDC != aRec.fMultiTDC){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fMultiTDC Not Equal. lhs: " 
			<< fMultiTDC << " != rhs " << aRec.fMultiTDC << endl;		
		else
			return false;  
	}
	
	if(fPcTimeSec != aRec.fPcTimeSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fPcTimeSec Not Equal. lhs: " 
			<< fPcTimeSec << " != rhs " << aRec.fPcTimeSec << endl;	
		else
			return false;  
	}
	
	if(fPcTimeMuSec != aRec.fPcTimeMuSec){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fPcTimeMuSec Not Equal. lhs: " 
			<< fPcTimeMuSec << " != rhs " << aRec.fPcTimeMuSec << endl;		
		else
			return false;  
	}
	
	if(fCommonStopTime != aRec.fCommonStopTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fCommonStopTime Not Equal. lhs: " 
			<< fCommonStopTime << " != rhs " << aRec.fCommonStopTime << endl;		
		else
			return false;  
	}
	
	if(fEventQuality != aRec.fEventQuality){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fEventQuality Not Equal" << endl;		
		else
			return false;  
	}
	
	if(fRunStartTime != aRec.fRunStartTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fRunStartTime Not Equal. lhs: " 
			<< fRunStartTime << " != rhs " << aRec.fRunStartTime << endl;		
		else
			return false;  
	}
	
	if(fRunEndTime != aRec.fRunEndTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fRunEndTime Not Equal. lhs: " 
			<< fRunEndTime << " != rhs " << aRec.fRunEndTime << endl;	
		else
			return false;  
	}

  if(fFileStartTime != aRec.fFileStartTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fFileStartTime Not Equal. lhs: " 
			<< fFileStartTime << " != rhs " << aRec.fFileStartTime << endl;		
		else
			return false;  
	}
  
  if(fFileEndTime != aRec.fFileEndTime){
		bIsEqual = false;
		if (bPrint) 
			cout << "KMuonVetoSystemRecord fFileEndTime Not Equal. lhs: " 
			<< fFileEndTime << " != rhs " << aRec.fFileEndTime << endl;		
		else
			return false;  
	}
	
	return bIsEqual;
}

void KMuonVetoSystemRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KSystemRecord::Compact();
	
	fEventQuality.Compact();
}
 
Bool_t KMuonVetoSystemRecord::TestEventQualityBit(Int_t i) const
{
	/*
	 These bits are set by the Neider processing, which provides some event quality 
	 information. From the TTree::Draw/Scan methods, the array of UChar_t which hold these
	 bits can be accessed from the variable fMuonSystem.fEventQuality.fAllBits[].  
	 
	 One can test these bits in the TTree Draw/Scan methods via the bit-wise operators &, |.
	 For example
	 
	 t->Draw("fAdc[][0]","fMuonSystem.fEventQuality.fAllBits & 0x1") 
	 
	 would draw the zeroth ADC value for only events where there was a 'reliable stamp'. 
	 
	 0: reliable stamp (mrs)
	 1: corrected stamp (jump)
	 2: stamp available
	 3: stamp is pctime
	 4: ADC mismatches in this event
	 5: Multi is not the same as in binaries
	 6: *NO* TDC End of Buffer found!
	 7: TDC afterpulse
	 8: ADC afterpulse (should never happen)
	 9: no TDC Data available
	 10: LED fired bit. If this bit has been set, then this event is due to the 
       firing of an LED installed in the new modules.
	 12: adc header found after noTDCEOB
   13: is GOOD muon data. --> determined offline by B. Schmidt (for Run12). 
        the muon veto system was determined to be in a good data taking period during this event. 
        good muon data taking periods are periods in time when the veto system was taking
        data, the measured position of the shield indicated that it was closed, and there is a 
        reliable stamp recorded by the Muon Veto DAQ.

   */
	return fEventQuality.TestBitNumber(i);

}	

Bool_t KMuonVetoSystemRecord::IsGoodMuonVetoData(void) const
{
  //return TestEventQualityBit(13);
  
  return TestEventQualityBit(13);
}

