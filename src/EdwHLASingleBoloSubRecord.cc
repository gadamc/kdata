//_____________________________________________
//
// EdwHLASingleBoloSubRecord.cc
// EdwDataStructure
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

#include "EdwHLASingleBoloSubRecord.h"
#include <iostream>
#include <cstring>
using namespace std;

ClassImp(EdwHLASingleBoloSubRecord);

EdwHLASingleBoloSubRecord::EdwHLASingleBoloSubRecord(void)
{
	InitializeMembers();
	
}

EdwHLASingleBoloSubRecord::EdwHLASingleBoloSubRecord(const EdwHLASingleBoloSubRecord &aRec)
: EdwSingleBoloSubRecordBase(aRec), EdwHLASubRecordBase(aRec)
{
	CopyLocalMembers(aRec);
	
}

EdwHLASingleBoloSubRecord& EdwHLASingleBoloSubRecord::operator=(const EdwHLASingleBoloSubRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->EdwSingleBoloSubRecordBase::operator=(aRec);
	this->EdwHLASubRecordBase::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void EdwHLASingleBoloSubRecord::CopyLocalMembers(const EdwHLASingleBoloSubRecord &aRec)
{
	SetQvalue(aRec.GetQvalue());
	SetEnergyRecoil(aRec.GetEnergyRecoil());
	SetEnergyIon(aRec.GetEnergyIon());
	SetEnergyHeat(aRec.GetEnergyHeat());
	
	SetEventFlag(aRec.GetEventFlag());
	SetChi2Flag(aRec.GetChi2Flag());
	SetTriggerBit1(aRec.GetTriggerBit1());
	SetTriggerBit2(aRec.GetTriggerBit2());
	SetVoltageFlag(aRec.GetVoltageFlag());
	SetIonPulseTimeOffset(aRec.GetIonPulseTimeOffset());	
	
	SetFiducialVolume(aRec.GetFiducialVolume());
	
	aRec.GetIonFlags(fIonFlags, kSizeOfIonFlags);
	
	//fSambaRecord = aRec.fSambaRecord;  //don't copy TRef value.
	fSambaRecord = 0;
	
	for(Int_t i = 0; i < 2; i++){
		SetEnergyCollectrode(i, aRec.GetEnergyCollectrode(i) );
		SetEnergyVeto(i, aRec.GetEnergyVeto(i));
		SetEnergyGuard(i, aRec.GetEnergyGuard(i) );
		SetEnergyBaselineCollectrode(i, aRec.GetEnergyBaselineCollectrode(i));
		SetEnergyBaselineVeto(i, aRec.GetEnergyBaselineVeto(i));
		SetEnergyBaselineGuard(i, aRec.GetEnergyBaselineGuard(i) );
		SetBaselineNoiseCollectrode(i, aRec.GetBaselineNoiseCollectrode(i));
		SetBaselineNoiseVeto(i, aRec.GetBaselineNoiseVeto(i));
		SetBaselineNoiseGuard(i, aRec.GetBaselineNoiseGuard(i));
	}
	
	SetEnergyIonFiducial(aRec.GetEnergyIonFiducial());
	SetEnergySumIonChannels(aRec.GetEnergySumIonChannels());
	SetBaselineIonFiducial(aRec.GetBaselineIonFiducial());
	SetEnergyBaselineHeat(aRec.GetEnergyBaselineHeat());
	SetBaselineHeat(aRec.GetBaselineHeat());
	
	fCuts = aRec.fCuts;
	fCuts.Compact();
}


EdwHLASingleBoloSubRecord::~EdwHLASingleBoloSubRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");
}

void EdwHLASingleBoloSubRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	EdwHLASubRecordBase::Clear(opt);
	EdwSingleBoloSubRecordBase::Clear(opt);
	
  //Clear and delete local objects here. 
	
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
	
}

void EdwHLASingleBoloSubRecord::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	SetQvalue(-99.0);
	SetEnergyRecoil(-99.0);
	SetEnergyIon(-99.0);
	SetEnergyHeat(-99.0);
	
	SetEventFlag(-99);
	SetChi2Flag(-99);
	SetTriggerBit1(-99);
	SetTriggerBit2(-99);
	SetVoltageFlag(-99);
	SetIonPulseTimeOffset(-99);	
	
	SetFiducialVolume(-99.0);
	for(int i=0; i< kSizeOfIonFlags; i++){
		SetIonFlagNumber(i,-99);
	}
	
	fSambaRecord = 0;
	
	for(Int_t i = 0; i < 2; i++){
		SetEnergyCollectrode(i, -99.0 );
		SetEnergyVeto(i, -99.0);
		SetEnergyGuard(i, -99.0 );
		SetEnergyBaselineCollectrode(i, -99.0);
		SetEnergyBaselineVeto(i, -99.0);
		SetEnergyBaselineGuard(i, -99.0 );
		SetBaselineNoiseCollectrode(i, -99.0);
		SetBaselineNoiseVeto(i, -99.0);
		SetBaselineNoiseGuard(i, -99.0);
	}
	
	SetEnergyIonFiducial(-99.0);
	SetEnergySumIonChannels(-99.0);
	SetBaselineIonFiducial(-99.0);
	SetEnergyBaselineHeat(-99.0);
	SetBaselineHeat(-99.0);
	
	fCuts.Clear();  //resets all the values to zero and the array size to zero.
}


Int_t EdwHLASingleBoloSubRecord::GetIonFlagNumber(Int_t index) const
{
	if(index < kSizeOfIonFlags && index >= 0){
		return fIonFlags[index];
	}
	else
		return -1;
}

Bool_t EdwHLASingleBoloSubRecord::GetIonFlags(Int_t* anArray, Int_t aSize) const
{
	if(aSize == kSizeOfIonFlags){
		memcpy(anArray, fIonFlags, sizeof(Int_t)*kSizeOfIonFlags);
		return true;
	}
	else
		return false;
}

Int_t EdwHLASingleBoloSubRecord::SetIonFlagNumber(Int_t index, Int_t aVal)
{
	if(index < kSizeOfIonFlags && index >= 0){
		fIonFlags[index] = aVal;
		return fIonFlags[index];
	}
	else
		return -1;
}

Bool_t EdwHLASingleBoloSubRecord::SetIonFlags(Int_t *anArray, Int_t aSize)
{
	if(aSize == kSizeOfIonFlags){
		memcpy(fIonFlags, anArray, sizeof(Int_t)*kSizeOfIonFlags);
		return true;
	}
	else
		return false;
}

Bool_t EdwHLASingleBoloSubRecord::IsSame(const EdwHLASingleBoloSubRecord &aRec, Bool_t bPrint) const
{
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	if(bPrint) cout.precision(16);
	
	//call the base class's IsSame methods
	if(!this->EdwHLASubRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(!this->EdwSingleBoloSubRecordBase::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fQvalue != aRec.fQvalue){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fQvalue Not Equal. lhs: " 
			<< fQvalue << " != rhs " << aRec.fQvalue << endl;		
		else
			return false;  
	}
	
	if(fEnergyRecoil != aRec.fEnergyRecoil){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyRecoil Not Equal. lhs: " 
			<< fEnergyRecoil << " != rhs " << aRec.fEnergyRecoil << endl;	
		else
			return false;  
	}
	
	if(fEnergyIon != aRec.fEnergyIon){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyIon Not Equal. lhs: " 
			<< fEnergyIon << " != rhs " << aRec.fEnergyIon << endl;		
		else
			return false;  
	}
	
	if(fEnergyHeat != aRec.fEnergyHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyHeat Not Equal. lhs: " 
			<< fEnergyHeat << " != rhs " << aRec.fEnergyHeat << endl;		
		else
			return false;  
	}
	
	for(Int_t i = 0; i < kSizeOfIonFlags; i++){
		if(fIonFlags[i] != aRec.fIonFlags[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fIonFlags["<<i<<"] Not Equal. lhs: " 
				<< fIonFlags[i] << " != rhs " << aRec.fIonFlags[i] << endl;		
			else
				return false;  
		}
	}
	
	if(fTriggerBit1 != aRec.fTriggerBit1){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fTriggerBit1 Not Equal. lhs: " 
			<< fTriggerBit1 << " != rhs " << aRec.fTriggerBit1 << endl;		
		else
			return false;  
	}
	
	if(fTriggerBit2 != aRec.fTriggerBit2){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fTriggerBit2 Not Equal. lhs: " 
			<< fTriggerBit2 << " != rhs " << aRec.fTriggerBit2 << endl;		
		else
			return false;  
	}
	
	if(fVoltageFlag != aRec.fVoltageFlag){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fVoltageFlag Not Equal. lhs: " 
			<< fVoltageFlag << " != rhs " << aRec.fVoltageFlag << endl;		
		else
			return false;  
	}
	
	if(fIonPulseTimeOffset != aRec.fIonPulseTimeOffset){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fIonPulseTimeOffset Not Equal. lhs: " 
			<< fIonPulseTimeOffset << " != rhs " << aRec.fIonPulseTimeOffset << endl;		
		else
			return false;  
	}
	
	if(fFiducialVolume != aRec.fFiducialVolume){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fFiducialVolume Not Equal. lhs: " 
			<< fFiducialVolume << " != rhs " << aRec.fFiducialVolume << endl;		
		else
			return false;  
	}
	
	/*
	if(fSambaRecord != aRec.fSambaRecord){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fSambaRecord Not Equal" << endl;		
		else
			return false;  
	}
	 WE DON'T EXPECT THE TREFS TO BE EQUAL! in fact, we don't want it to be equal.
	*/
	for(Int_t i = 0; i < 2; i++){
		if(fEnergyCollectrode[i] != aRec.fEnergyCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyCollectrode[" << i << "] Not Equal. lhs: " 
				<< fEnergyCollectrode[i] << " != rhs " << aRec.fEnergyCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyVeto[i] != aRec.fEnergyVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyVeto[" << i << "] Not Equal. lhs: " 
				<< fEnergyVeto[i] << " != rhs " << aRec.fEnergyVeto[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyGuard[i] != aRec.fEnergyGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyGuard[" << i << "] Not Equal. lhs: " 
				<< fEnergyGuard[i] << " != rhs " << aRec.fEnergyGuard[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineCollectrode[i] != aRec.fEnergyBaselineCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyBaselineCollectrode[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineCollectrode[i] << " != rhs " << aRec.fEnergyBaselineCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineVeto[i] != aRec.fEnergyBaselineVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyBaselineVeto[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineVeto[i] << " != rhs " << aRec.fEnergyBaselineVeto[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineGuard[i] != aRec.fEnergyBaselineGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fEnergyBaselineGuard[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineGuard[i] << " != rhs " << aRec.fEnergyBaselineGuard[i] << endl;			
			else
				return false;  
		}
		
		if(fBaselineNoiseCollectrode[i] != aRec.fBaselineNoiseCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fBaselineNoiseCollectrode[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseCollectrode[i] << " != rhs " << aRec.fBaselineNoiseCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fBaselineNoiseVeto[i] != aRec.fBaselineNoiseVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fBaselineNoiseVeto[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseVeto[i] << " != rhs " << aRec.fBaselineNoiseVeto[i] << endl;	
			else
				return false;  
		}
		
		if(fBaselineNoiseGuard[i] != aRec.fBaselineNoiseGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "EdwHLASingleBoloSubRecord fBaselineNoiseGuard[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseGuard[i] << " != rhs " << aRec.fBaselineNoiseGuard[i] << endl;		
			else
				return false;  
		}
	}
		
	if(fEnergyIonFiducial != aRec.fEnergyIonFiducial){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyIonFiducial Not Equal. lhs: " 
			<< fEnergyIonFiducial << " != rhs " << aRec.fEnergyIonFiducial << endl;	
		else
			return false;  
	}

	
	if(fEnergyIonSum != aRec.fEnergyIonSum){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyIonSum Not Equal. lhs: " 
			<< fEnergyIonSum << " != rhs " << aRec.fEnergyIonSum << endl;		
		else
			return false;  
	}
	
	if(fBaselineIonFiducial != aRec.fBaselineIonFiducial){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fBaselineIonFiducial Not Equal. lhs: " 
			<< fBaselineIonFiducial << " != rhs " << aRec.fBaselineIonFiducial << endl;		
		else
			return false;  
	}
	
	if(fEnergyBaselineHeat != aRec.fEnergyBaselineHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fEnergyBaselineHeat Not Equal. lhs: " 
			<< fEnergyBaselineHeat << " != rhs " << aRec.fEnergyBaselineHeat << endl;		
		else
			return false;  
	}
	
	if(fBaselineHeat != aRec.fBaselineHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fBaselineHeat Not Equal. lhs: " 
			<< fBaselineHeat << " != rhs " << aRec.fBaselineHeat << endl;		
		else
			return false;  
	}
	
	if(fCuts != aRec.fCuts){
		bIsEqual = false;
		if (bPrint) 
			cout << "EdwHLASingleBoloSubRecord fCuts Not Equal" << endl;		
		else
			return false;  
	}
	cout.precision(8);
	
	return bIsEqual;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyCollectrode[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyVeto[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyGuard[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyBaselineCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineCollectrode[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyBaselineVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineVeto[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetEnergyBaselineGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyBaselineGuard[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetBaselineNoiseCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseCollectrode[i];
	
	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetBaselineNoiseVeto(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseVeto[i];

	else return -99999.;
}

Double32_t EdwHLASingleBoloSubRecord::GetBaselineNoiseGuard(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fBaselineNoiseGuard[i];
	
	else return -99999.;
}


void EdwHLASingleBoloSubRecord::SetEnergyCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyCollectrode[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetEnergyVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyVeto[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetEnergyGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyGuard[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetEnergyBaselineCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineCollectrode[i] = aVal;

}

void EdwHLASingleBoloSubRecord::SetEnergyBaselineVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineVeto[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetEnergyBaselineGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineGuard[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetBaselineNoiseCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseCollectrode[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetBaselineNoiseVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseVeto[i] = aVal;
	
}

void EdwHLASingleBoloSubRecord::SetBaselineNoiseGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseGuard[i] = aVal;

}


void EdwHLASingleBoloSubRecord::SetCuts(const TBits *mCuts)
{
	fCuts = *mCuts;
	fCuts.Compact();
}


void EdwHLASingleBoloSubRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwSingleBoloSubRecordBase::Compact();
	EdwHLASubRecordBase::Compact();
	
	fCuts.Compact();
}

Bool_t EdwHLASingleBoloSubRecord::TestCutsBitNumber(Int_t i) const 
{
	//The ERA cuts file for each bolometer has been read and each cut has been packed into this
	//TBits object. The cuts correspond to the various cuts listed below.
	//cuts.CutChi2Col1 -- bit 0
	//cuts.CutChi2Col2 -- bit 1
	//cuts.CutChi2Vet1 -- bit 2
	//cuts.CutChiVet2  -- 3
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
