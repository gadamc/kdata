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
  //standard constructor
  
	InitializeMembers();

}

KHLABolometerRecord::KHLABolometerRecord(const KHLABolometerRecord &aRec)
: KBolometerRecord(aRec)
{
  //copy constructor 
  
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
  //copies local data members
  
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
	fPulseRecords.Delete();

	SetEnergyIonFiducial(aRec.GetEnergyIonFiducial());
	SetEnergySumIonChannels(aRec.GetEnergySumIonChannels());
	SetBaselineIonFiducial(aRec.GetBaselineIonFiducial());

	fCuts = aRec.fCuts;
	fCuts.Compact();
}


KHLABolometerRecord::~KHLABolometerRecord(void)
{
  //destructor 
  
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
  //init local data members
  
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
	fPulseRecords.Delete();

	SetEnergyIonFiducial(-99.0);
	SetEnergySumIonChannels(-99.0);
	SetBaselineIonFiducial(-99.0);

	fCuts.Clear();  //resets all the values to zero and the array size to zero.
}

Bool_t KHLABolometerRecord::SetIonFlags(Int_t *anArray, Int_t aSize)
{
  //Set the TBits ion flags using an array of integers. If the integer
  //is greater than 1, the bit flag is set to true. 
  //If anArray is zero, it returns false and the local ion flags are
  //not changed.
  
	if(anArray == 0) return false;
  
  fIonFlags.Clear();
  
	for(Int_t i = 0; i < aSize; i++){
		if(anArray[i] > 0)
			fIonFlags.SetBitNumber(i, true);
	}

	return true;
}


Bool_t KHLABolometerRecord::IsSame(const KHLABolometerRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
  
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



KHLABoloPulseRecord* KHLABolometerRecord::GetPulseRecord(Int_t channel, Int_t aType) const
{
  //this is a private method
  //
  //returns a pointer to a KHLABoloPulseRecord. channel is the channel number (1, 2, ...) and
  //a type is either heat, collectrode, veto or guard. The integers associated 
  //with those types do not need to be known. Instead call the static functions
  //KHLABoloPulseRecord::GetHeatType()
  //KHLABoloPulseRecord::GetVetoType()
  //KHLABoloPulseRecord::GetCollectrodeType()
  //KHLABoloPulseRecord::GetGuardType()
  //
  //However, its much easier to use the methods GetHeatPulse, GetCollectrodePulse, etc...
  //that are found in this class
  //
  KHLABoloPulseRecord* pulse = 0;
  for(Int_t k = 0; k < GetNumPulseRecords(); k++){
    pulse = GetPulseRecord(k);
    if( (pulse->GetPulseType() == aType) && (pulse->GetChannelNumber() == channel))
      return pulse;
  }
  return 0;  //if we get here, then we didn't find the proper pulse
}

KHLABoloPulseRecord* KHLABolometerRecord::GetHeatPulse(Int_t channel) const
{
  //returns a pointer to a Heat pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  
  KHLABoloPulseRecord* pulse = GetPulseRecord(channel, KHLABoloPulseRecord::GetHeatType());
  
  return (pulse != 0) ? pulse : 0;  
}

KHLABoloPulseRecord* KHLABolometerRecord::GetCollectrodePulse(Int_t channel) const
{
  //returns a pointer to a Collectrode pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  
  KHLABoloPulseRecord* pulse = GetPulseRecord(channel, KHLABoloPulseRecord::GetCollectrodeType());
  
  return (pulse != 0) ? pulse : 0;  
}

KHLABoloPulseRecord* KHLABolometerRecord::GetVetoPulse(Int_t channel) const
{
  //returns a pointer to a Veto pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  KHLABoloPulseRecord* pulse = GetPulseRecord(channel, KHLABoloPulseRecord::GetVetoType());
  
  return (pulse != 0) ? pulse : 0; 
}

KHLABoloPulseRecord* KHLABolometerRecord::GetGuardPulse(Int_t channel) const
{
  //returns a pointer to a Guard pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  KHLABoloPulseRecord* pulse = GetPulseRecord(channel, KHLABoloPulseRecord::GetGuardType());
  
  return (pulse != 0) ? pulse : 0; 
}


Double32_t KHLABolometerRecord::GetEnergy(Int_t aChannel, Int_t aType) const
{
  //returns the calibrated pulse energy (in keV) on aChannel of pulse type aType, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2. To determine the correct
  //value for aType, use the static fuctions
  //
  //KHLABoloPulseRecord::GetHeatType()
  //KHLABoloPulseRecord::GetCollectrodeType()
  //KHLABoloPulseRecord::GetVetoType()
  //KHLABoloPulseRecord::GetGuardType()
  //
  //Or, you can just use the methods GetEnergyHeat(aChannel), GetEnergyCollectrode(aChannel),
  //GetEnergyVeto(aChannel) and GetEnergyGuard(aChannel)
  //
  
  
  KHLABoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetEnergy();
  else return -9999.;
}

Double32_t KHLABolometerRecord::GetEnergyCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a collecltrode channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergy(aChannel, KHLABoloPulseRecord::GetCollectrodeType());
}

Double32_t KHLABolometerRecord::GetEnergyVeto(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a veto channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergy(aChannel, KHLABoloPulseRecord::GetVetoType());
}

Double32_t KHLABolometerRecord::GetEnergyGuard(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a guard channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergy(aChannel, KHLABoloPulseRecord::GetGuardType());
}

Double32_t KHLABolometerRecord::GetEnergyHeat(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a heat channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergy(aChannel, KHLABoloPulseRecord::GetHeatType());
}

Double32_t KHLABolometerRecord::GetEnergyBaseline(Int_t aChannel, Int_t aType) const
{
  KHLABoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetEnergyBaseline();
  else return -9999.;
}


Double32_t KHLABolometerRecord::GetEnergyBaselineCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a collecltrode channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergyBaseline(aChannel, KHLABoloPulseRecord::GetCollectrodeType());
}

Double32_t KHLABolometerRecord::GetEnergyBaselineVeto(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a veto channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergyBaseline(aChannel, KHLABoloPulseRecord::GetVetoType());
}

Double32_t KHLABolometerRecord::GetEnergyBaselineGuard(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a guard channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergyBaseline(aChannel, KHLABoloPulseRecord::GetGuardType());
}

Double32_t KHLABolometerRecord::GetEnergyBaselineHeat(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a heat channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetEnergyBaseline(aChannel, KHLABoloPulseRecord::GetHeatType());
}

Double32_t KHLABolometerRecord::GetBaselineNoise(Int_t aChannel, Int_t aType) const
{
  KHLABoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetBaselineNoise();
  else return -9999.;
}


Double32_t KHLABolometerRecord::GetBaselineNoiseCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a collecltrode channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetBaselineNoise(aChannel, KHLABoloPulseRecord::GetCollectrodeType());
}

Double32_t KHLABolometerRecord::GetBaselineNoiseVeto(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a veto channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetBaselineNoise(aChannel, KHLABoloPulseRecord::GetVetoType());
}

Double32_t KHLABolometerRecord::GetBaselineNoiseGuard(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a guard channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetBaselineNoise(aChannel, KHLABoloPulseRecord::GetGuardType());
}

Double32_t KHLABolometerRecord::GetBaselineNoiseHeat(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a heat channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetBaselineNoise(aChannel, KHLABoloPulseRecord::GetHeatType());
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
	fIonFlags.Compact();
	fPulseRecords.Compress();
}

Bool_t KHLABolometerRecord::TestCutsBit(Int_t i) const
{
	//The ERA cuts file for each bolometer has been read and each cut has been packed into this
	//TBits object. The cuts correspond to the various cuts listed below.
  //A value of '1' indicates that this bolometer record was NOT CUT due to 
  //that particular cut. If the value is '0', then the event didn't pass
  //the cut and should be excluded from WIMP analysis.
  //
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
  //
	//i = 0, Collectrode 1
	//i = 1, Collectrode 2
	//i = 2, Veto 1
	//i = 3, Veto 2
	//i = 4, Guard 1
	//i = 5, Guard 2
  //i = 6, ??
  //i = 7, ??

	return fIonFlags.TestBitNumber(i);

}
/*
Bool_t KHLABolometerRecord::TestIonFlags(UInt_t f) const
{
  //returns true if the bit-pattern of the first 6 bits in 
  //f is the same as that found in fIonFlags. We only test 6 bits here
  //because there are only 6 possible ion channels in an ID or FID bolometer
  //This uses the TBits::TestBits(f) method. 
  //For example, if you want to test for fiducial only event, then call
  //TestIonFlags(0x03)... 0x03 = 0000 0011 in binary
  //
  //for guard only 
  //
  //TestIonFlags(0x30)
  //
  //For surface only
  //
  //TestIonFlags(0xC0)
  //
  //for fiducial+guard
  //
  //TestIonFlags(0x33)
  
  Bool_t theRet = true;
  
  for(Int_t i = 0; i < 6; i++)
    
  
  return fIonFlags.TestBits(f) == f;
  
}
*/


string KHLABolometerRecord::GetEventCategory(void) const
{
  //returns a string corresponding the the name of the event category 
  //as stored in the return value of GetEventFlag()
  //event categories are noise (0), heat only (1), fiducial (2),
  //surface (3), pure guard (4), and other (5)
  //
  string category;
  
  switch (GetEventFlag()) {
    case 0:
      category = "noise";
      break;
    case 1:
      category = "heat only";
      break;
    case 2:
      category = "fiducial";
      break;
    case 3:
      category = "surface";
      break;
    case 4:
      category = "pure guard";
      break;
    case 5:
      category = "other";
      break;
    default:
      category = "";
      break;
  }
  
  return category;
}


void KHLABolometerRecord::AddPulseRecord(KHLABoloPulseRecord* aPulseRecord)
{
  //add a pulse record to this object
  
	//fPulseRecords.Add((TObject *)aPulseRecord);
	fPulseRecords.Add((TObject *)aPulseRecord);
}

KHLABoloPulseRecord* KHLABolometerRecord::GetPulseRecord(Int_t i) const
{
  //this returns the i^th pulse record for this particular bolometer record.
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
	return (KHLABoloPulseRecord*) fPulseRecords.At(i);
}

