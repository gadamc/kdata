//_____________________________________________
//
// KRawBolometerRecord.cxx
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

#include "KRawBolometerRecord.h"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include "KRawBoloPulseRecord.h"

using namespace std;

ClassImp(KRawBolometerRecord);

KRawBolometerRecord::KRawBolometerRecord(void)
{
  //standard constructor
  
	InitializeMembers();

}

KRawBolometerRecord::KRawBolometerRecord(const KRawBolometerRecord &aRec)
: KBolometerRecord(aRec)
{
  //copy constructor 
  
	CopyLocalMembers(aRec);

}

KRawBolometerRecord& KRawBolometerRecord::operator=(const KRawBolometerRecord &aRec)
{
	if(&aRec == this) return *this;

	this->KBolometerRecord::operator=(aRec);

	CopyLocalMembers(aRec);

	return *this;
}

void KRawBolometerRecord::CopyLocalMembers(const KRawBolometerRecord& /*aRec*/)
{
  //copies local data members
  
	fSambaRecord = 0;  //will need to set these another way
	fPulseRecords.Clear();
}


KRawBolometerRecord::~KRawBolometerRecord(void)
{
  //destructor 
  
	//Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KRawBolometerRecord::Clear(Option_t *opt)
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

void KRawBolometerRecord::InitializeMembers(void)
{
  //init local data members
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  fSambaRecord = 0;
	fPulseRecords.Clear();

}


Bool_t KRawBolometerRecord::IsSame(const KRawBolometerRecord &aRec, Bool_t bPrint) const
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

	cout.precision(8);

	return bIsEqual;
}



KRawBoloPulseRecord* KRawBolometerRecord::GetPulseRecord(Int_t channel, Int_t aType) const
{
  //this is a private method
  //
  //returns a pointer to a KRawBoloPulseRecord. channel is the channel number (1, 2, ...) and
  //a type is either heat, collectrode, veto or guard. The integers associated 
  //with those types do not need to be known. Instead call the static functions
  //KBoloPulseRecord::GetHeatType()
  //KBoloPulseRecord::GetVetoType()
  //KBoloPulseRecord::GetCollectrodeType()
  //KBoloPulseRecord::GetGuardType()
  //
  //However, its much easier to use the methods GetHeatPulse, GetCollectrodePulse, etc...
  //that are found in this class
  //
  KRawBoloPulseRecord* pulse = 0;
  for(Int_t k = 0; k < GetNumPulseRecords(); k++){
    pulse = GetPulseRecord(k);
    if( (pulse->GetPulseType() == aType) && (pulse->GetChannelNumber() == channel))
      return pulse;
  }
  return 0;  //if we get here, then we didn't find the proper pulse
}

KRawBoloPulseRecord* KRawBolometerRecord::GetHeatPulse(Int_t channel) const
{
  //returns a pointer to a Heat pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  
  return GetPulseRecord(channel, KBoloPulseRecord::GetHeatType());
  
}

KRawBoloPulseRecord* KRawBolometerRecord::GetCollectrodePulse(Int_t channel) const
{
  //returns a pointer to a Collectrode pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  
  return GetPulseRecord(channel, KBoloPulseRecord::GetCollectrodeType());
  
    
}

KRawBoloPulseRecord* KRawBolometerRecord::GetVetoPulse(Int_t channel) const
{
  //returns a pointer to a Veto pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  return GetPulseRecord(channel, KBoloPulseRecord::GetVetoType());
  
}

KRawBoloPulseRecord* KRawBolometerRecord::GetGuardPulse(Int_t channel) const
{
  //returns a pointer to a Guard pulse record that is associated with this bolometer record.
  //
  //Note that aChannel should be 1 or 2.
  //
  return GetPulseRecord(channel, KBoloPulseRecord::GetGuardType());
  
}


Double32_t KRawBolometerRecord::GetAmplitude(Int_t aChannel, Int_t aType) const
{
  //returns the calibrated pulse energy (in keV) on aChannel of pulse type aType, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2. To determine the correct
  //value for aType, use the static fuctions
  //
  //KBoloPulseRecord::GetHeatType()
  //KBoloPulseRecord::GetCollectrodeType()
  //KBoloPulseRecord::GetVetoType()
  //KBoloPulseRecord::GetGuardType()
  //
  //Or, you can just use the methods GetAmplitudeHeat(aChannel), GetAmplitudeCollectrode(aChannel),
  //GetAmplitudeVeto(aChannel) and GetAmplitudeGuard(aChannel)
  //
  
  
  KRawBoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetAmplitude();
  else return -9999.;
}

Double32_t KRawBolometerRecord::GetAmplitudeCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a collecltrode channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitude(aChannel, KBoloPulseRecord::GetCollectrodeType());
}

Double32_t KRawBolometerRecord::GetAmplitudeVeto(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a veto channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitude(aChannel, KBoloPulseRecord::GetVetoType());
}

Double32_t KRawBolometerRecord::GetAmplitudeGuard(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a guard channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitude(aChannel, KBoloPulseRecord::GetGuardType());
}

Double32_t KRawBolometerRecord::GetAmplitudeHeat(Int_t aChannel) const
{
  //returns the calibrated pulse energy (in keV) on a heat channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitude(aChannel, KBoloPulseRecord::GetHeatType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaseline(Int_t aChannel, Int_t aType) const
{
  KRawBoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetAmplitudeBaseline();
  else return -9999.;
}


Double32_t KRawBolometerRecord::GetAmplitudeBaselineCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a collecltrode channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaseline(aChannel, KBoloPulseRecord::GetCollectrodeType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineVeto(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a veto channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaseline(aChannel, KBoloPulseRecord::GetVetoType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineGuard(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a guard channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaseline(aChannel, KBoloPulseRecord::GetGuardType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineHeat(Int_t aChannel) const
{
  //returns the calibrated pulse energy along the baseline (in keV) on a heat channel, as
  //calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaseline(aChannel, KBoloPulseRecord::GetHeatType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineNoise(Int_t aChannel, Int_t aType) const
{
  KRawBoloPulseRecord* pulse = GetPulseRecord(aChannel,aType);
  if(pulse)
    return pulse->GetAmplitudeBaselineNoise();
  else return -9999.;
}


Double32_t KRawBolometerRecord::GetAmplitudeBaselineNoiseCollectrode(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a collecltrode channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaselineNoise(aChannel, KBoloPulseRecord::GetCollectrodeType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineNoiseVeto(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a veto channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaselineNoise(aChannel, KBoloPulseRecord::GetVetoType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineNoiseGuard(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a guard channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaselineNoise(aChannel, KBoloPulseRecord::GetGuardType());
}

Double32_t KRawBolometerRecord::GetAmplitudeBaselineNoiseHeat(Int_t aChannel) const
{
  //returns the calibrated pulse RMS noise (in keV) along the baseline
  //on a heat channel, as calculated by the ERA processor.
  //
  //Note that aChannel should be 1 or 2.
  //

  return GetAmplitudeBaselineNoise(aChannel, KBoloPulseRecord::GetHeatType());
}


void KRawBolometerRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes

	KBolometerRecord::Compact();

	fPulseRecords.Compress();
}

void KRawBolometerRecord::AddPulseRecord(KRawBoloPulseRecord* aPulseRecord)
{
  //add a pulse record to this object
  
	//fPulseRecords.Add((TObject *)aPulseRecord);
	fPulseRecords.Add((TObject *)aPulseRecord);
}

KRawBoloPulseRecord* KRawBolometerRecord::GetPulseRecord(Int_t i) const
{
  //this returns the i^th pulse record for this particular bolometer record.
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
	return (KRawBoloPulseRecord*) fPulseRecords.At(i);
}

