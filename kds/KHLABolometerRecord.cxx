//_____________________________________________
//
// KHLABolometerRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The HLA class for Single Bolo Sub Records. It stores a significant amount
// of information for each recorded bolometer event. It contains all of the 
// calculated values from the ERA processing. See the descriptions below
// of each of the parameters.
//

#include "KHLABolometerRecord.h"
#include <iostream>
#include <cstring>
#include <stdexcept> 
#include "KHLABoloPulseRecord.h"

using namespace std;

ClassImp(KHLABolometerRecord);

KHLABolometerRecord::KHLABolometerRecord(void)
{
	InitializeMembers();
	 
}

KHLABolometerRecord::KHLABolometerRecord(const KHLABolometerRecord &aRec)
: KBolometerRecord(aRec)
{
	CopyLocalMembers(aRec);
	
}

KHLABolometerRecord& KHLABolometerRecord::operator=(const KHLABolometerRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KBolometerRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KHLABolometerRecord::CopyLocalMembers(const KHLABolometerRecord &aRec)
{
	SetQvalue(aRec.GetQvalue());
	SetEnergyRecoil(aRec.GetEnergyRecoil());
	SetEnergyIon(aRec.GetEnergyIon());
	
	SetEventFlag(aRec.GetEventFlag());
	SetChi2Flag(aRec.GetChi2Flag());
	//SetTriggerBit1(aRec.GetTriggerBit1());
	//SetTriggerBit2(aRec.GetTriggerBit2());
	SetVoltageFlag(aRec.GetVoltageFlag());
	SetIonPulseTimeOffset(aRec.GetIonPulseTimeOffset());	
	
	fIonFlags = aRec.fIonFlags;
	fIonFlags.Compact();
	
	fSambaRecord = 0;  //will need to set these by hand!
	fPulseRecords.Clear();
	
	SetEnergyIonFiducial(aRec.GetEnergyIonFiducial());
	SetEnergySumIonChannels(aRec.GetEnergySumIonChannels());
	SetBaselineIonFiducial(aRec.GetBaselineIonFiducial());
	
	fCuts = aRec.fCuts;
	fCuts.Compact();
}


KHLABolometerRecord::~KHLABolometerRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KHLABolometerRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KBolometerRecord::Clear(opt);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void KHLABolometerRecord::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	SetQvalue(-99.0);
	SetEnergyRecoil(-99.0);
	SetEnergyIon(-99.0);
	
	SetEventFlag(-99);
	SetChi2Flag(-99);
	//SetTriggerBit1(-99);
	//SetTriggerBit2(-99);
	SetVoltageFlag(-99);
	SetIonPulseTimeOffset(-99);	
		
	fIonFlags.Clear();
	
	fSambaRecord = 0;
	fPulseRecords.Clear();  
	
	SetEnergyIonFiducial(-99.0);
	SetEnergySumIonChannels(-99.0);
	SetBaselineIonFiducial(-99.0);
	
	fCuts.Clear();  //resets all the values to zero and the array size to zero.
}

Bool_t KHLABolometerRecord::SetIonFlags(Int_t *anArray, Int_t aSize)
{
	if(anArray == 0) return false;
	
	for(Int_t i = 0; i < aSize; i++){
		if(anArray[i] > 0)
			fIonFlags.SetBitNumber(i, true);
	}
	
	return true;
}


Bool_t KHLABolometerRecord::IsSame(const KHLABolometerRecord &aRec, Bool_t bPrint) const
{
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	if(bPrint) cout.precision(16);
	
	
	if(!this->KBolometerRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fQvalue != aRec.fQvalue){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fQvalue Not Equal. lhs: " 
			<< fQvalue << " != rhs " << aRec.fQvalue << endl;		
		else
			return false;  
	}
	
	if(fEnergyRecoil != aRec.fEnergyRecoil){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyRecoil Not Equal. lhs: " 
			<< fEnergyRecoil << " != rhs " << aRec.fEnergyRecoil << endl;	
		else
			return false;  
	}
	
	if(fEnergyIon != aRec.fEnergyIon){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyIon Not Equal. lhs: " 
			<< fEnergyIon << " != rhs " << aRec.fEnergyIon << endl;		
		else
			return false;  
	}
	
	if(fVoltageFlag != aRec.fVoltageFlag){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fVoltageFlag Not Equal. lhs: " 
			<< fVoltageFlag << " != rhs " << aRec.fVoltageFlag << endl;		
		else
			return false;  
	}
	
	if(fIonPulseTimeOffset != aRec.fIonPulseTimeOffset){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fIonPulseTimeOffset Not Equal. lhs: " 
			<< fIonPulseTimeOffset << " != rhs " << aRec.fIonPulseTimeOffset << endl;		
		else
			return false;  
	}
	
	
	if(fEnergyIonFiducial != aRec.fEnergyIonFiducial){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyIonFiducial Not Equal. lhs: " 
			<< fEnergyIonFiducial << " != rhs " << aRec.fEnergyIonFiducial << endl;	
		else
			return false;  
	}

	
	if(fEnergyIonSum != aRec.fEnergyIonSum){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyIonSum Not Equal. lhs: " 
			<< fEnergyIonSum << " != rhs " << aRec.fEnergyIonSum << endl;		
		else
			return false;  
	}
	
	if(fBaselineNoiseIonFiducial != aRec.fBaselineNoiseIonFiducial){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fBaselineNoiseIonFiducial Not Equal. lhs: " 
			<< fBaselineNoiseIonFiducial << " != rhs " << aRec.fBaselineNoiseIonFiducial << endl;		
		else
			return false;  
	}
	
	if(fCuts != aRec.fCuts){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fCuts Not Equal" << endl;		
		else
			return false;  
	}
	
	if(fIonFlags != aRec.fIonFlags){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fIonFlags Not Equal" << endl;		
		else
			return false;  
	}
	
	cout.precision(8);
	
	return bIsEqual;
}
/*
Double32_t KHLABolometerRecord::GetEnergyCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyCollectrode[i];
	
	else return -99999.;
}
*
Double32_t KHLABolometerRecord::GetEnergyVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyVeto[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetEnergyGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyGuard[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetEnergyBaselineCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineCollectrode[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetEnergyBaselineVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineVeto[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetEnergyBaselineGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineGuard[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetBaselineNoiseCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseCollectrode[i];
	
	else return -99999.;
}

Double32_t KHLABolometerRecord::GetBaselineNoiseVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseVeto[i];

	else return -99999.;
}

Double32_t KHLABolometerRecord::GetBaselineNoiseGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseGuard[i];
	
	else return -99999.;
}

*/

void KHLABolometerRecord::SetCuts(const TBits *mCuts)
{
	fCuts = *mCuts;
	fCuts.Compact();
}


void KHLABolometerRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KBolometerRecord::Compact();
	
	fCuts.Compact();
	fIonFlags.Compact();
	fPulseRecords.Compress();
}

Bool_t KHLABolometerRecord::TestCutsBit(Int_t i) const 
{
	//The ERA cuts file for each bolometer has been read and each cut has been packed into this
	//TBits object. The cuts correspond to the various cuts listed below.
	//cuts.CutChi2Col1 -- bit 0
	//cuts.CutChi2Col2 -- bit 1
	//cuts.CutChi2Vet1 -- bit 2
	//cuts.CutChi2Vet2 -- 3
	//cuts.CutChi2Gar1 -- 4
	//cuts.CutChi2Gar2 -- 5
	//cuts.CutChi2Heat -- 6
	//cuts.CutChi2     -- 7
	//cuts.CutCol1Base -- 8 //the baseline "too large" check
	//cuts.CutCol2Base -- 9
	//cuts.CutVet1Base -- 10
	//cuts.CutVet2Base -- 11
	//cuts.CutGar1Base -- 12
	//cuts.CutGar2Base -- 13
	//cuts.CutHeatBase -- 14
	//cuts.CutBases    -- 15
	//cuts.CutRun      -- 16
	
	
	return fCuts.TestBitNumber(i);

}

Bool_t KHLABolometerRecord::TestIonFlag(Int_t i) const 
{
	//each element in this array corresponds to a particular ion channel 
	//(collectrode 1, 2, veto 1, 2, and guard 1, 2). When the value is set, 
	//there was a pulse seen on that channel, as defined by the ERA.
	//Your analysis may be different, however, and you may define an event
	//on the ion channel with a different set of conditions. 
	
	//i = 0, Collectrode 1
	//i = 1, Collectrode 2
	//i = 2, Veto 1
	//i = 3, Veto 2
	//i = 4, Guard 1
	//i = 5, Guard 2
	
	return fIonFlags.TestBitNumber(i);
	
}

void KHLABolometerRecord::AddPulseRecord(KHLABoloPulseRecord* aPulseRecord)
{
	//fPulseRecords.Add((TObject *)aPulseRecord);
	fPulseRecords.Add((TObject *)aPulseRecord);
}

KHLABoloPulseRecord* KHLABolometerRecord::GetPulseRecord(Int_t i) const 
{ 
	return (KHLABoloPulseRecord*) fPulseRecords.At(i);
}

