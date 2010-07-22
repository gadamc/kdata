//_____________________________________________
//
// EdwHLAEvent.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for High Level Analysis. One typically
// begins all analysis by getting a pointer to this object from 
// the EdwDSReader object. By calling EdwDSReader::GetEntry(Int_t i)
// or EdwDSReader::GetNextEntry, one can loop through the EdwDS data
// file and examine each event separately. From this HLAEvent object
// one can obtain pointers to they Muon and Bolometer System Records
// GetBoloSystemRecord(void), GetMuonVetoSystemRecord()
// and to each of the event's Sub Records. 

#include "EdwHLAEvent.h"
#include "EdwRawEvent.h"

#include "TProcessID.h"

//system Record Includes
//#include "EdwHLABoloSysRecord.h"
//#include "EdwHLAMuonVetoSysRecord.h"

//sub record includes
#include "TClonesArray.h"
#include "EdwHLASambaSubRecord.h"
#include "EdwHLASingleBoloSubRecord.h"
#include "EdwHLABoloPulseSubRecord.h"
#include "EdwHLAMuonModuleSubRecord.h"
#include <iostream>

using namespace std;

ClassImp(EdwHLAEvent);

EdwHLAEvent::EdwHLAEvent(void)
{
	//Default Constructor. This allocates memory for the TClonesArrays
	//and this might be a bug/memory leak. According to the ROOT documentation
	//no memory should be allocated by the default constructor. This needs
	//to be checked.
	//
	//If you are creating your own EdwHLAEvent object for use outside 
	//of ROOT I/O, it is strongly encouraged that you use the EdwEventFactory
	//to make the object for you. It is intended that if the EdwHLAEvent
	//needs to be built in a more complex way, by always asking the EdwEventFactory
	//to build an event object for you, you are guaranteed to get a fully
	//created object (assuming that the EdwEventFactory is properly coded. :)
	
	//HLAEvent members that are on the STACK
	InitializeMembers();
	
	//TClonesArray Initializations must be done separately.
	//as well as any other members that are created on the heap/free store
	fNumSamba = 0;
	fNumBolo = 0;
	fNumBoloPulse = 0;
	fNumMuonModule = 0;
	
	fSamba = 0;
	fBolo = 0;
	fBoloPulse = 0;
	fMuonModule = 0;
	
	CreateArrays();
}

EdwHLAEvent::EdwHLAEvent(const EdwHLAEvent &anEvent)
: EdwEventBase(anEvent)
{
	//Copy Constructor. The TRefs will point to newly created objects
	//and not to the data members of the EdwHLAEvent object you passed in.
	
	CopyLocalMembers(anEvent);
	
	//MUST copy all TRef's appropriately to the new file. Otherwise
	//they will point to records in a different File! This is 
	//why I use the copyClonesArray method here instead of
	//using TClonesArray's copy constructors. 
	fNumSamba = 0;
	fNumBolo = 0;
	fNumBoloPulse = 0;
	fNumMuonModule = 0;
	
	fSamba = 0;
	fBolo = 0;
	fBoloPulse = 0;
	fMuonModule = 0;
	CreateArrays();

	CopyClonesArrays(anEvent);
}

EdwHLAEvent::~EdwHLAEvent(void) 
{
	
	Clear("C");
	
	//Delete TClonesArrays
	if(fSamba) delete fSamba;
	fSamba = 0;
	
	if(fBolo) delete fBolo;
	fBolo = 0;
	
	if(fBoloPulse) delete fBoloPulse;
	fBoloPulse = 0;
	
	if(fMuonModule) delete fMuonModule;
	fMuonModule = 0;
	
	if(fSamba != 0) cout << "WTF!" <<endl;
	
	
}

EdwHLAEvent& EdwHLAEvent::operator=(const EdwEventBase &anEvent)
{
	//assigns the data member values of an EdwEventBase object into the EdwHLAEvent. 
	
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "hla = base" << endl;
#endif
	if(&anEvent == this) return *this;
	this->EdwEventBase::operator=(anEvent);

	return *this;
}

EdwHLAEvent& EdwHLAEvent::operator=(const EdwRawEvent &anEvent)
{
	//assigns the data member values of an EdwRawEvent into this EdwHLAEvent.
	//calls this->CopyFromRawEvent
	
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "hla = raw" << endl;
#endif
	try {
		const EdwEventBase &rhsBase = dynamic_cast<const EdwEventBase&>(anEvent);
		if(&rhsBase == this) return *this;
		
		this->EdwEventBase::operator=(rhsBase);
		CopyFromRawEvent(anEvent);
	}
	catch (bad_cast) {
		//do nothing... although, maybe should do something
		//because it should be totally strange if this doesn't throw.
	}
	
	return *this;
}


EdwHLAEvent& EdwHLAEvent::operator=(const EdwHLAEvent &anEvent)
{
	//sets anEvent to this. 
	
#ifdef _EDW_DEBUG_EVENT_ASSIGNOP
	cout << "hla = hla" << endl;
#endif
	if(&anEvent == this) return *this;
	
	this->EdwEventBase::operator=(anEvent);
	
	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);

	return *this;
}

void EdwHLAEvent::CopyClonesArrays(const EdwHLAEvent &anEvent)
{
	//
	//ClonesArray assignment doesn't appear to work in the following way
	//*fSamba = *anEvent.GetSambaSubRecords();
	//*fBolo = *anEvent.GetBoloSubRecords();
	//*fBoloPulse = *anEvent.GetBoloPulseSubRecords();
	//*fMuonModule = *anEvent.GetMuonModuleSubRecords();
	//
	//so, I just clear this object's array, and create as many objects as I need.
	//this might be inefficient, but I don't think so. besides,
	//we likely won't be creating many many instances of the Event class.
	//at least, we shouldn't do that. 
	//if your code is really slow and you think it has something
	//to do with copying HLAEvents, look here for possible fixes. 
	//
	//MUST copy all TRef's appropriately to the new file. Otherwise
	//they will point to records in a different File!

	
	ClearArray("C", fSamba,fNumSamba);
	ClearArray("C", fBolo,fNumBolo);
	ClearArray("C", fBoloPulse,fNumBoloPulse);
	ClearArray("C", fMuonModule,fNumMuonModule);
	
	Int_t ObjectNumber = TProcessID::GetObjectCount();
	/*
	for(Int_t i = 0; i < anEvent.GetNumSambas(); i++){
		EdwHLASambaSubRecord *s = AddSamba();
		EdwHLASambaSubRecord *sO = anEvent.GetSamba(i);
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cout << "EdwHLAEvent::operator= Invalid Samba Pointer" << endl;
	}
	*/
	//UNFORTUNATELY, WE CAN'T PUT THE COPYING OF THE TREF'D SAMBA
	//OBJECT INSIDE OF THE SINGLEBOLO ASSIGNMENT OPERATOR METHOD
	//BECAUSE EACH SAMBA RECORD MUST BE ADDED TO THE EVENT. 
	
	for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
		EdwHLASingleBoloSubRecord *bolo = AddBolo();
		EdwHLASingleBoloSubRecord *boloO = anEvent.GetBolo(i);
		EdwHLASambaSubRecord *sambaO = boloO->GetSambaRecord();
		EdwHLASambaSubRecord *samba = 0;
		
		if(bolo != 0 && boloO != 0) 
			*bolo = *boloO;
		else
			cout << "EdwHLAEvent::operator= Invalid SingleBolo Pointer" << endl;
		
		if(sambaO != 0) {
			samba = AddSamba();
			*samba = *sambaO;
			if(samba) bolo->SetSambaRecord(samba);
		}
		//else
			//cout << "EdwHLAEvent::operator= Invalid Samba Pointer" << endl;
			//no need to print out an error message here. it may be the desired
			//behavior that there is not an associated samba record. 
		
	}
	
	//EVENTUALLY, THE BOLO PULSES WILL HAVE TO BE ADDED
	//AND LINKED VIA THE TREF IN THE SAME WAY THAT THE SAMBA
	//RECORD IS DONE ABOVE, INSTEAD OF THIS CURRENT METHOD THAT
	//DOESN'T PRESERVE PROPER TREF LINKING TO THIS EVENT
	for(Int_t i = 0; i < anEvent.GetNumBoloPulses(); i++){
		EdwHLABoloPulseSubRecord *s = AddBoloPulse();
		EdwHLABoloPulseSubRecord *sO = anEvent.GetBoloPulse(i);
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cout << "EdwHLAEvent::operator= Invalid BoloPulse Pointer" << endl;
	}
	
	for(Int_t i = 0; i < anEvent.GetNumMuonModules(); i++){
		EdwHLAMuonModuleSubRecord *s = AddMuonModule();
		EdwHLAMuonModuleSubRecord *sO = anEvent.GetMuonModule(i);
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cout << "EdwHLAEvent::operator= Invalid MuonModule Pointer" << endl;
	}
	
	//Restore Object count                                                                                                     
	//To save space in the table keeping track of all referenced objects 
	//and computation time,
	//we assume that our events DO NOT address each other. We reset the                                                        
	//object count to what it was at the beginning of the event.                                                               
	TProcessID::SetObjectCount(ObjectNumber);
	
}

void EdwHLAEvent::CopyLocalMembers(const EdwHLAEvent &anEvent)
{
	fDataCleaningWord = anEvent.fDataCleaningWord;
	fBoloSystem = anEvent.fBoloSystem; 
	fMuonSystem = anEvent.fMuonSystem;
	fNumSamba = anEvent.fNumSamba;
	fNumBolo = anEvent.fNumBolo;
	fNumBoloPulse = anEvent.fNumBoloPulse;
	fNumMuonModule = anEvent.fNumMuonModule;
	
}

void EdwHLAEvent::CopyFromRawEvent(const EdwRawEvent &/*anEvent*/)
{
	//Does nothing at the moment because the EdwRawEvent class is empty.
	//This might be a good place to calculate any High Level
	//event information based upon Raw Event information. 
	//For example, the pulse Shapes in the Raw Events would
	//be analyzed and the Q, Energy values would then be stored
	//into the HLA object.
}


void EdwHLAEvent::Clear(Option_t *anOption)
{
	ClearArray(anOption, fSamba,fNumSamba);
	ClearArray(anOption, fBolo,fNumBolo);
	ClearArray(anOption, fBoloPulse,fNumBoloPulse);
	ClearArray(anOption, fMuonModule,fNumMuonModule);

	//delete any memory allocated by the EdwEvent class here that
	//needs to be deleted for each event -- but not the TClonesArrays. Clear should be called 
	//after every event so that the TClonesArrays are cleared. 
	EdwEventBase::Clear(anOption);
	
	//Call the System Record clears in case they need to clean up
	//or to reset initial values of member variables
	fBoloSystem.Clear(anOption);
	fMuonSystem.Clear(anOption);
	
	//reset member variables to zero
	InitializeMembers();
																 
}

void EdwHLAEvent::InitializeMembers(void)
{
	fDataCleaningWord.Clear();
	SetDataType(kHLADataType);
	
}

void EdwHLAEvent::CreateArrays(void)
{
	//Allocates memory for the TClonesArrays if they haven't already
	//been allocated.
	
	if(!fSamba)
		fSamba = new TClonesArray("EdwHLASambaSubRecord",5);
	
	if(!fBolo)
		fBolo = new TClonesArray("EdwHLASingleBoloSubRecord",5);
	
	if(!fBoloPulse)
		fBoloPulse = new TClonesArray("EdwHLABoloPulseSubRecord",10);
	
	if(!fMuonModule)
		fMuonModule = new TClonesArray("EdwHLAMuonModuleSubRecord",5);

	
	//why doesn't this create a memory leak? CreateArrays is called
	//by the default constructor. Does ROOT realize that I already 
	//have these TClonesArrays. Or maybe a TClonesArray object doesn't
	//require that much memory, so I don't notice the leak.
}

void EdwHLAEvent::ClearArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		mArray->Clear( (anOption && *anOption) ? anOption : "C" );
		//we have to delete because our sub-records contain TString members! :(
		//mArray->Delete( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}

template<class T> T* EdwHLAEvent::AddSubRecord(TClonesArray *mArray, Int_t &mCount)
{
	TClonesArray &mArrayRef = *mArray;
	T* mNewSubRecord = new(mArrayRef[mCount++]) T;
	return mNewSubRecord;
}

EdwHLASambaSubRecord* EdwHLAEvent::AddSamba(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	return AddSubRecord<EdwHLASambaSubRecord>(fSamba,fNumSamba);
}

EdwHLASingleBoloSubRecord* EdwHLAEvent::AddBolo(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kBoloTriggerType);
	return AddSubRecord<EdwHLASingleBoloSubRecord>(fBolo,fNumBolo);
}

EdwHLABoloPulseSubRecord* EdwHLAEvent::AddBoloPulse()
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	return AddSubRecord<EdwHLABoloPulseSubRecord>(fBoloPulse,fNumBoloPulse);
}

EdwHLAMuonModuleSubRecord* EdwHLAEvent::AddMuonModule()
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kMuonVetoTriggerType);
	return AddSubRecord<EdwHLAMuonModuleSubRecord>(fMuonModule,fNumMuonModule);
}

EdwHLASambaSubRecord *EdwHLAEvent::GetSamba(Int_t i) const
{
  // Return the i'th Samba Sub Record for this event.
	
  EdwHLASambaSubRecord *ms = 0;
  if (i < fNumSamba) ms = (EdwHLASambaSubRecord *)fSamba->At(i);
  return ms;
}

EdwHLASingleBoloSubRecord *EdwHLAEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.
	
  EdwHLASingleBoloSubRecord *ms = 0;
  if (i < fNumBolo) ms = (EdwHLASingleBoloSubRecord *)fBolo->At(i);
  return ms;
}

EdwHLABoloPulseSubRecord *EdwHLAEvent::GetBoloPulse(Int_t i) const
{
  // Return the i'th Bolometer Pulse Sub Record for this event.
	
  EdwHLABoloPulseSubRecord *ms = 0;
  if (i < fNumBoloPulse) ms = (EdwHLABoloPulseSubRecord *)fBoloPulse->At(i);
  return ms;
}

EdwHLAMuonModuleSubRecord *EdwHLAEvent::GetMuonModule(Int_t i) const
{
  // Return the i'th Muon Module Sub Record for this event.
	
  EdwHLAMuonModuleSubRecord *ms = 0;
  if (i < fNumMuonModule) ms = (EdwHLAMuonModuleSubRecord *)fMuonModule->At(i);
  return ms;
}

Bool_t EdwHLAEvent::IsSame(const EdwHLAEvent &anEvent, Bool_t bPrint) const
{
	//Compares two EdwHLAEvents and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->EdwEventBase::IsSame(anEvent,bPrint)){
		if (bPrint) 
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fDataCleaningWord != anEvent.fDataCleaningWord){
		if (bPrint) 
			cout << "EdwHLAEvent fDataCleaningWord Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(!fBoloSystem.IsSame(anEvent.fBoloSystem, bPrint)){
		if (bPrint) 
			cout << "EdwHLAEvent fBoloSystem Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	 
	if(!fMuonSystem.IsSame(anEvent.fMuonSystem, bPrint)){
		if (bPrint) 
			cout << "EdwHLAEvent fMuonSystem Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	if(fNumSamba == anEvent.fNumSamba){
		for(Int_t i = 0; i < fNumSamba; i++){
			EdwHLASambaSubRecord *s = GetSamba(i);
			EdwHLASambaSubRecord *sOther = anEvent.GetSamba(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "EdwHLAEvent EdwHLASambaSubRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "EdwHLAEvent fNumSamba Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	if(fNumBolo == anEvent.fNumBolo){
		for(Int_t i = 0; i < fNumBolo; i++){
			EdwHLASingleBoloSubRecord *s = GetBolo(i);
			EdwHLASingleBoloSubRecord *sOther = anEvent.GetBolo(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "EdwHLAEvent EdwHLASingleBoloSubRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "EdwHLAEvent fNumBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	if(fNumBoloPulse == anEvent.fNumBoloPulse){
		for(Int_t i = 0; i < fNumBoloPulse; i++){
			EdwHLABoloPulseSubRecord *s = GetBoloPulse(i);
			EdwHLABoloPulseSubRecord *sOther = anEvent.GetBoloPulse(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "EdwHLAEvent EdwHLABoloPulseSubRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "EdwHLAEvent fNumBoloPulse Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	if(fNumMuonModule == anEvent.fNumMuonModule){
		for(Int_t i = 0; i < fNumMuonModule; i++){
			EdwHLAMuonModuleSubRecord *s = GetMuonModule(i);
			EdwHLAMuonModuleSubRecord *sOther = anEvent.GetMuonModule(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "EdwHLAEvent EdwHLABoloPulseSubRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "EdwHLAEvent fNumMuonModule Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
		
	return bIsEqual;
}

void EdwHLAEvent::myPrint()
{
	cout << "fNumSamba : fNumBolo : fNumBoloPulse : fNumMuonModule"<<endl;
	cout << fNumSamba  <<" : " << fNumBolo <<" : " << fNumBoloPulse <<" : " << fNumMuonModule << endl;
	//cout <<  "fRunNumber : fStamp : fTriggerType : fDataType : fDetectorStatusWord : fBlindnessWord : fGSEventNumber" << endl;
	//cout << fRunNumber  <<" : " << fStamp <<" : " << fTriggerType <<" : " << fDataType <<" : " <<fDetectorStatusWord <<" : " << fBlindnessWord <<" : " << fGSEventNumber  << endl;

}

void EdwHLAEvent::Compact(void)
{
	//Make the event class as small as possible. This calls 'Compact' for all member
	//variables that are EdwDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	EdwEventBase::Compact();
	
	fBoloSystem.Compact();
	fMuonSystem.Compact();
	
	for(Int_t i = 0; i < GetNumSambas(); i++){
		EdwHLASambaSubRecord* samba = GetSamba(i);
		samba->Compact();
	}
	for(Int_t i = 0; i < GetNumBolos(); i++){
		EdwHLASingleBoloSubRecord* bolo = GetBolo(i);
		bolo->Compact();
	}
	for(Int_t i = 0; i < GetNumBoloPulses(); i++){
		EdwHLABoloPulseSubRecord* bp = GetBoloPulse(i);
		bp->Compact();
	}
	for(Int_t i = 0; i < GetNumMuonModules(); i++){
		EdwHLAMuonModuleSubRecord* module = GetMuonModule(i);
		module->Compact();
	}
}

//Easy Access / Event-Level Calculation Methods 

Double_t EdwHLAEvent::GetSumBoloEnergyRecoil(void)
{
	//Returns the sum of the recoil energies for all
	//bolometer sub-records in the event.
	
	Double_t energy = 0;
	for(Int_t i = 0; i < GetNumBolos(); i++)
		energy += GetBolo(i)->GetEnergyRecoil();
	
	return energy;
}

Double_t EdwHLAEvent::GetSumBoloEnergyHeat(void)
{
	//Returns the sum of the energy on the heat channel for all
	//bolometer sub-records in the event.
	
	Double_t energy = 0;
	for(Int_t i = 0; i < GetNumBolos(); i++)
		energy += GetBolo(i)->GetEnergyHeat();
	
	return energy;
}

Double_t EdwHLAEvent::GetSumBoloEnergyIon(void)
{
	//Returns the sum of the fEnergyIon for all
	//bolometer sub-records in the event. Note that in the ERA
	//processors, the calculation of fEnergyIon depends upon
	//the category of the event.
	
	Double_t energy = 0;
	for(Int_t i = 0; i < GetNumBolos(); i++)
		energy += GetBolo(i)->GetEnergyIon();
	
	return energy;
}

