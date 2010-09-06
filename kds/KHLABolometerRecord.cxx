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
	SetEnergyHeat(aRec.GetEnergyHeat());
	
	SetEventFlag(aRec.GetEventFlag());
	SetChi2Flag(aRec.GetChi2Flag());
	SetTriggerBit1(aRec.GetTriggerBit1());
	SetTriggerBit2(aRec.GetTriggerBit2());
	SetVoltageFlag(aRec.GetVoltageFlag());
	SetIonPulseTimeOffset(aRec.GetIonPulseTimeOffset());	
	
	SetFiducialVolume(aRec.GetFiducialVolume());
	
	aRec.GetIonFlags(fIonFlags, kSizeOfIonFlags);
	
	fSambaRecordNum = aRec.fSambaRecordNum;  //don't copy this value?, it might need to change.
	//fSambaRecordNum = -1;
	
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
	SetBaselineNoiseHeat(aRec.GetBaselineNoiseHeat());
	
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
	
	fSambaRecordNum = -1;
	
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
	SetBaselineNoiseHeat(-99.0);
	
	fCuts.Clear();  //resets all the values to zero and the array size to zero.
}


Int_t KHLABolometerRecord::GetIonFlagNumber(Int_t index) const
{
	if(index < kSizeOfIonFlags && index >= 0){
		return fIonFlags[index];
	}
	else
		return -1;
}

Bool_t KHLABolometerRecord::GetIonFlags(Int_t* anArray, Int_t aSize) const
{
	if(aSize == kSizeOfIonFlags){
		memcpy(anArray, fIonFlags, sizeof(Int_t)*kSizeOfIonFlags);
		return true;
	}
	else
		return false;
}

Int_t KHLABolometerRecord::SetIonFlagNumber(Int_t index, Int_t aVal)
{
	if(index < kSizeOfIonFlags && index >= 0){
		fIonFlags[index] = aVal;
		return fIonFlags[index];
	}
	else
		return -1;
}

Bool_t KHLABolometerRecord::SetIonFlags(Int_t *anArray, Int_t aSize)
{
	if(aSize == kSizeOfIonFlags){
		memcpy(fIonFlags, anArray, sizeof(Int_t)*kSizeOfIonFlags);
		return true;
	}
	else
		return false;
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
	
	if(fEnergyHeat != aRec.fEnergyHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyHeat Not Equal. lhs: " 
			<< fEnergyHeat << " != rhs " << aRec.fEnergyHeat << endl;		
		else
			return false;  
	}
	
	for(Int_t i = 0; i < kSizeOfIonFlags; i++){
		if(fIonFlags[i] != aRec.fIonFlags[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fIonFlags["<<i<<"] Not Equal. lhs: " 
				<< fIonFlags[i] << " != rhs " << aRec.fIonFlags[i] << endl;		
			else
				return false;  
		}
	}
	
	if(fTriggerBit1 != aRec.fTriggerBit1){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fTriggerBit1 Not Equal. lhs: " 
			<< fTriggerBit1 << " != rhs " << aRec.fTriggerBit1 << endl;		
		else
			return false;  
	}
	
	if(fTriggerBit2 != aRec.fTriggerBit2){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fTriggerBit2 Not Equal. lhs: " 
			<< fTriggerBit2 << " != rhs " << aRec.fTriggerBit2 << endl;		
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
	
	if(fFiducialVolume != aRec.fFiducialVolume){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fFiducialVolume Not Equal. lhs: " 
			<< fFiducialVolume << " != rhs " << aRec.fFiducialVolume << endl;		
		else
			return false;  
	}
	

	for(Int_t i = 0; i < 2; i++){
		if(fEnergyCollectrode[i] != aRec.fEnergyCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyCollectrode[" << i << "] Not Equal. lhs: " 
				<< fEnergyCollectrode[i] << " != rhs " << aRec.fEnergyCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyVeto[i] != aRec.fEnergyVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyVeto[" << i << "] Not Equal. lhs: " 
				<< fEnergyVeto[i] << " != rhs " << aRec.fEnergyVeto[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyGuard[i] != aRec.fEnergyGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyGuard[" << i << "] Not Equal. lhs: " 
				<< fEnergyGuard[i] << " != rhs " << aRec.fEnergyGuard[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineCollectrode[i] != aRec.fEnergyBaselineCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyBaselineCollectrode[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineCollectrode[i] << " != rhs " << aRec.fEnergyBaselineCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineVeto[i] != aRec.fEnergyBaselineVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyBaselineVeto[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineVeto[i] << " != rhs " << aRec.fEnergyBaselineVeto[i] << endl;		
			else
				return false;  
		}
		
		if(fEnergyBaselineGuard[i] != aRec.fEnergyBaselineGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fEnergyBaselineGuard[" << i << "] Not Equal. lhs: " 
				<< fEnergyBaselineGuard[i] << " != rhs " << aRec.fEnergyBaselineGuard[i] << endl;			
			else
				return false;  
		}
		
		if(fBaselineNoiseCollectrode[i] != aRec.fBaselineNoiseCollectrode[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fBaselineNoiseCollectrode[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseCollectrode[i] << " != rhs " << aRec.fBaselineNoiseCollectrode[i] << endl;		
			else
				return false;  
		}
		
		if(fBaselineNoiseVeto[i] != aRec.fBaselineNoiseVeto[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fBaselineNoiseVeto[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseVeto[i] << " != rhs " << aRec.fBaselineNoiseVeto[i] << endl;	
			else
				return false;  
		}
		
		if(fBaselineNoiseGuard[i] != aRec.fBaselineNoiseGuard[i]){
			bIsEqual = false;
			if (bPrint) 
				cout << "KHLABolometerRecord fBaselineNoiseGuard[" << i << "] Not Equal. lhs: " 
				<< fBaselineNoiseGuard[i] << " != rhs " << aRec.fBaselineNoiseGuard[i] << endl;		
			else
				return false;  
		}
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
	
	if(fBaselineIonFiducial != aRec.fBaselineIonFiducial){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fBaselineIonFiducial Not Equal. lhs: " 
			<< fBaselineIonFiducial << " != rhs " << aRec.fBaselineIonFiducial << endl;		
		else
			return false;  
	}
	
	if(fEnergyBaselineHeat != aRec.fEnergyBaselineHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fEnergyBaselineHeat Not Equal. lhs: " 
			<< fEnergyBaselineHeat << " != rhs " << aRec.fEnergyBaselineHeat << endl;		
		else
			return false;  
	}
	
	if(fBaselineNoiseHeat != aRec.fBaselineNoiseHeat){
		bIsEqual = false;
		if (bPrint) 
			cout << "KHLABolometerRecord fBaselineNoiseHeat Not Equal. lhs: " 
			<< fBaselineNoiseHeat << " != rhs " << aRec.fBaselineNoiseHeat << endl;		
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
	cout.precision(8);
	
	return bIsEqual;
}

Double32_t KHLABolometerRecord::GetEnergyCollectrode(Int_t i) const 
{
	if(i >= 0 && i < 2)
		return fEnergyCollectrode[i];
	
	else return -99999.;
}

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


void KHLABolometerRecord::SetEnergyCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyCollectrode[i] = aVal;
	
}

void KHLABolometerRecord::SetEnergyVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyVeto[i] = aVal;
	
}

void KHLABolometerRecord::SetEnergyGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyGuard[i] = aVal;
	
}

void KHLABolometerRecord::SetEnergyBaselineCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineCollectrode[i] = aVal;

}

void KHLABolometerRecord::SetEnergyBaselineVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineVeto[i] = aVal;
	
}

void KHLABolometerRecord::SetEnergyBaselineGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fEnergyBaselineGuard[i] = aVal;
	
}

void KHLABolometerRecord::SetBaselineNoiseCollectrode(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseCollectrode[i] = aVal;
	
}

void KHLABolometerRecord::SetBaselineNoiseVeto(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseVeto[i] = aVal;
	
}

void KHLABolometerRecord::SetBaselineNoiseGuard(Int_t i, Double_t aVal) 
{
	if(i >= 0 && i < 2)
		fBaselineNoiseGuard[i] = aVal;

}


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
}

Bool_t KHLABolometerRecord::TestCutsBitNumber(Int_t i) const 
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
