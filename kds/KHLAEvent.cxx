//_____________________________________________
//
// KHLAEvent.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for High Level Analysis. One typically
// begins all analysis by getting a pointer to this object from 
// the KDataReader object. 
// 
// root[0]: KDataReader f("Kds_Run12_v3.0.root");
// root[1]: KHLAEvent *e = f.GetEvent();
// 
// By calling KDataReader::GetEntry(Int_t i) within a for-loop
// or KDataReader::GetNextEntry, one can loop through the KDS data
// file and examine each event separately. Whenever GetEntry or GetNextEntry
// are called, the values stored in the KHLAEvent object are updated with
// the values for that entry. From this KHLAEvent object
// one can obtain pointers to they Muon and Bolometer System Records
// GetBoloSystemRecord(void), GetMuonVetoSystemRecord()
// and to each of the event's Sub Records. (GetBolo(Int_t i), GetMuonModule(Int_t i)) 
//

#include "KHLAEvent.h"
#include "KRawEvent.h"

#include "TProcessID.h"

//sub record includes
#include "KHLASambaRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLABoloPulseRecord.h"
#include "KHLAMuonModuleRecord.h"
#include <iostream>

using namespace std;

ClassImp(KHLAEvent);

KHLAEvent::KHLAEvent(void)
{
	//Default Constructor. This allocates memory for the KClonesArrays
	//and this might be a bug/memory leak. According to the ROOT documentation
	//no memory should be allocated by the default constructor. This needs
	//to be checked.
	//
	//If you are creating your own KHLAEvent object for use outside 
	//of ROOT I/O, it is strongly encouraged that you use the KEventFactory
	//to make the object for you. It is intended that if the KHLAEvent
	//needs to be built in a more complex way, by always asking the KEventFactory
	//to build an event object for you, you are guaranteed to get a fully
	//created object (assuming that the KEventFactory is properly coded. :)
	
	//HLAEvent members that are on the STACK
	InitializeMembers();
	
	//KClonesArray Initializations must be done separately.
	//as well as any other members that are created on the heap/free store
	
	fSamba = 0;
	fBolo = 0;
	fBoloPulse = 0;
	fMuonModule = 0;
	
	CreateArrays();
}

KHLAEvent::KHLAEvent(const KHLAEvent &anEvent)
: KEvent(anEvent)
{
	//Copy Constructor. The TRefs will point to newly created objects
	//and not to the data members of the KHLAEvent object you passed in.
	
	CopyLocalMembers(anEvent);
	
	//MUST copy all TRef's appropriately to the new file. Otherwise
	//they will point to records in a different File! This is 
	//why I use the copyClonesArray method here instead of
	//using KClonesArray's copy constructors. 
	
	fSamba = 0;
	fBolo = 0;
	fBoloPulse = 0;
	fMuonModule = 0;
	CreateArrays();

	CopyClonesArrays(anEvent);
}

KHLAEvent::~KHLAEvent(void) 
{
	
	Clear("C");
	
	//Delete KClonesArrays
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


KHLAEvent& KHLAEvent::operator=(const KEvent &anEvent)
{
  //assignment operator
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "hla = base" << endl;
#endif
	
	//assigns the data member values of an KEvent object into the KHLAEvent. 
	if(&anEvent == this) return *this;

	
	try {
		const KRawEvent &rhs = dynamic_cast<const KRawEvent&>(anEvent);
		this->operator=(rhs);
		
	}
	catch(bad_cast) {
		
		try{
			const KHLAEvent &rhs = dynamic_cast<const KHLAEvent&>(anEvent);
			this->operator=(rhs);
		}
		
		catch(bad_cast) {
			this->KEvent::operator=(anEvent);
		}
	}
	

	return *this;
}

KHLAEvent& KHLAEvent::operator=(const KRawEvent &anEvent)
{
	//assigns the data member values of an KRawEvent into this KHLAEvent.
	//calls this->CopyFromRawEvent
	
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "hla = raw" << endl;
#endif
	try {
		const KEvent &rhs = dynamic_cast<const KEvent&>(anEvent);
		if(&rhs == this) return *this;
		
		this->KEvent::operator=(rhs);
		CopyFromRawEvent(anEvent);
	}
	catch (bad_cast) {
		//do nothing... although, maybe should do something
		//because it should be totally strange if this doesn't throw.
	}
	
	return *this;
}


KHLAEvent& KHLAEvent::operator=(const KHLAEvent &anEvent)
{
	//sets anEvent to this. 
	
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "hla = hla" << endl;
#endif
	if(&anEvent == this) return *this;
	
	this->KEvent::operator=(anEvent);
	
	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);

	return *this;
}

UInt_t KHLAEvent::GetLargestUniqueIDNumber(void)
{
  //returns the largest unique ID number for TRefs in this
  //event. This is used for adding new subrecords to events, which
  //is needed in order to "merge" the data. 
  //
  // Note to developers: the structure of KData should be rethought
  // and this method, along with the AddSubRecord method, should probably
  // be put into a separate EventBuilder class, which could be used
  // by the KEventFactory. Or, perhaps just add it to the factory. 
  //
	
	UInt_t aUniqueIDNumber = 0;
	
	//hmm... can a unique ID number be equal to zero???? 
	Bool_t bIsInitialized = false;
	
	for(Int_t i = 0; i < GetNumSambas(); i++){
		if(!bIsInitialized && i == 0){
			aUniqueIDNumber = GetSamba(i)->GetUniqueID();
			bIsInitialized = true;
		}
		else if(GetSamba(i)->GetUniqueID() > aUniqueIDNumber)
			aUniqueIDNumber = GetSamba(i)->GetUniqueID();
	}
	
	for(Int_t i = 0; i < GetNumBolos(); i++){
		if(!bIsInitialized && i == 0){
			aUniqueIDNumber = GetBolo(i)->GetUniqueID();
			bIsInitialized = true;
		}
		else if(GetBolo(i)->GetUniqueID() > aUniqueIDNumber)
			aUniqueIDNumber = GetBolo(i)->GetUniqueID();
	}
	
	for(Int_t i = 0; i < GetNumBoloPulses(); i++){
		if(!bIsInitialized && i == 0){
			aUniqueIDNumber = GetBoloPulse(i)->GetUniqueID();
			bIsInitialized = true;
		}
		else if(GetBoloPulse(i)->GetUniqueID() > aUniqueIDNumber)
			aUniqueIDNumber = GetBoloPulse(i)->GetUniqueID();
	}
	
	for(Int_t i = 0; i < GetNumMuonModules(); i++){
		if(!bIsInitialized && i == 0){
			aUniqueIDNumber = GetMuonModule(i)->GetUniqueID();
			bIsInitialized = true;
		}
		else if(GetMuonModule(i)->GetUniqueID() > aUniqueIDNumber)
			aUniqueIDNumber = GetMuonModule(i)->GetUniqueID();
	}

	return aUniqueIDNumber;
}

Int_t KHLAEvent::AddSubRecords(const KHLAEvent &anEvent, Bool_t skimNoise)
{
  //Adds the sub records of 'anEvent' to *this. If skimNoise == true,
  //then the bolometer records (and their associated samba and bolopulse records) 
  //for bolometer records that have an EventFlag == 0 will not be added to *this.
  //returns the number of sub records added to *this.
  //
	
  Int_t numRecord = 0;
	
  for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
    KHLABolometerRecord *bolo0 = anEvent.GetBolo(i);
    if(  (skimNoise==true) ? (bolo0->GetEventFlag() > 0) : true ) { //if skimNoise is true
                                                                    //then only include this bolometer if its not noise
      if(AddBoloSubRecord(*bolo0)) numRecord++;
      else return -1;
    }
  }
	
  for(Int_t i = 0; i < anEvent.GetNumMuonModules(); i++){    
    if(AddMuonModuleSubRecord(*anEvent.GetMuonModule(i)))  numRecord++;
    else return -1;
	}
	
	return numRecord;
}


Bool_t KHLAEvent::AddMuonModuleSubRecord(const KHLAMuonModuleRecord &inMuonModule)
{
  //Add inMuonModule to this KHLAEvent.
  
  //don't need to deal with object count here because we're not using
  //TRefs with the muon module data. 

  KHLAMuonModuleRecord *muonmodule = AddMuonModule();
  
  if(muonmodule != 0){
    *muonmodule = inMuonModule;
    return true;
  }
  else return false; //only return false if AddMuonModule() doesn't work
  
}


Bool_t KHLAEvent::AddBoloSubRecord(const KHLABolometerRecord &inBolo)
{
  //Add inBolo to this KHLAEvent and also add the Samba record that it points
  //to and the Pulse Records. If no Samba record is associated with inBolo (via GetSambaRecord),
  //this method returns false. If an invalid pointer to a Pulse Record is found, 
  //this method returns false. This ensures that the event is built properly. 
  
  Int_t ObjectNumber = TProcessID::GetObjectCount(); //save this number for later.
	
	//but first, we have to see what unique ID numbers exist within this particular event
	//and start the unique ID iterations from that point! otherwise, we'll get objects in the
	//event with the same unique ID number and the TRefs won't work. 
	UInt_t aNumber = GetLargestUniqueIDNumber();
	if(aNumber != 0) TProcessID::SetObjectCount(aNumber);
	
  KHLABolometerRecord *newBolo = AddBolo();
  
  if(newBolo != 0)
    *newBolo = inBolo;  //copies everything but the TRef data.
  else {
    cerr << "KHLAEvent::AddBoloSubRecord Invalid new KHLABolometerRecord Pointer" << endl;
    TProcessID::SetObjectCount(ObjectNumber);
    return false;
  }
  
  
  //Add the samba record for this bolometer - if the samba record is already in this event
  //then don't add a duplicate.
  KHLASambaRecord *inSamba = inBolo.GetSambaRecord();
  
  if(inSamba != 0) {
    
    //first, have to check to see if there already exists a samba sub record that is exactly like this one.
    Bool_t bAddSamba = true;
    
    for(Int_t n = 0; n < GetNumSambas(); n++){
      KHLASambaRecord *samba = GetSamba(n);
      
      if(*samba == *inSamba){  
        //we found a match for the samba record already in our event! 
        //Just set the samba record without creating a new Samba Record
        newBolo->SetSambaRecord(samba);
        bAddSamba = false;
        n = GetNumSambas(); //exit the loop
      }
    }
    
    //if we didn't find an already existing samba record that matches, then we
    //add a new samba record to this event and copy the samba 
    //record from inBolo.
    if(bAddSamba==true){
      //cout << "Add new Samba" << endl;
      KHLASambaRecord *samba = AddSamba();

      *samba = *inSamba; //copy the samba record data
      newBolo->SetSambaRecord(samba); //set the pointer
    }
    
  }
  else {
    cerr << "KHLAEvent::AddBoloSubRecord Invalid KHLASambaRecord Pointer" << endl;
    TProcessID::SetObjectCount(ObjectNumber);
    return false;
  }
  //done adding the samba record.
    
  
  //now add all of the pulse records  
  for(Int_t j = 0; j < inBolo.GetNumPulseRecords(); j++){
    KHLABoloPulseRecord *pulse0 = inBolo.GetPulseRecord(j);
    if(pulse0 != 0){
      KHLABoloPulseRecord *pulse = AddBoloPulse();

      if(pulse != 0){
        *pulse = *pulse0;
        newBolo->AddPulseRecord(pulse);
        pulse->SetBolometerRecord(newBolo);
      }
      else {
        cerr << "KHLAEvent::AddSubRecords AddBoloPulse returned an invalid KHLABoloPulseRecord Pointer" << endl;
        TProcessID::SetObjectCount(ObjectNumber);
        return false;
      }
    }
    
    else {
      cerr << "KHLAEvent::AddSubRecords Invalid KHLABoloPulseRecord Pointer" << endl;
      TProcessID::SetObjectCount(ObjectNumber);
      return false;
    }
    
    
  }

  //Restore Object count                                                                                                     
	//To save space in the table keeping track of all referenced objects 
	//and computation time,
	//we assume that our events DO NOT address each other. We reset the                                                        
	//object count to what it was at the beginning of the event.                                                               
	TProcessID::SetObjectCount(ObjectNumber);
  
  return true;
}


void KHLAEvent::CopyClonesArrays(const KHLAEvent &anEvent)
{
	//

	ClearArrays("C");
	
	AddSubRecords(anEvent);
	
}

void KHLAEvent::CopyLocalMembers(const KHLAEvent &anEvent)
{
	//fDataCleaningWord = anEvent.fDataCleaningWord;
	fBoloSystem = anEvent.fBoloSystem; 
	fMuonSystem = anEvent.fMuonSystem;
		
  fRunNumber = anEvent.fRunNumber;
  fRunStartTime = anEvent.fRunStartTime;
	fRunEndTime = anEvent.fRunEndTime;
  fGSEventNumber = anEvent.fGSEventNumber;	
  
}

void KHLAEvent::CopyFromRawEvent(const KRawEvent &/*anEvent*/)
{
	//Does nothing at the moment because the KRawEvent class is empty.
	//This might be a good place to calculate any High Level
	//event information based upon Raw Event information. 
	//For example, the pulse Shapes in the Raw Events would
	//be analyzed and the Q, Energy values would then be stored
	//into the HLA object.
}

void KHLAEvent::ClearArrays(Option_t *anOption)
{
	ClearArray(anOption, fSamba);  
	ClearArray(anOption, fBolo); 
	ClearArray(anOption, fBoloPulse);
	ClearArray(anOption, fMuonModule);
}

void KHLAEvent::Clear(Option_t *anOption)
{
	
	ClearArrays(anOption);
	//delete any memory allocated by the KEvent class here that
	//needs to be deleted for each event -- but not the KClonesArrays. Clear should be called 
	//after every event so that the KClonesArrays are cleared. 
	KEvent::Clear(anOption);
	
	//Call the System Record clears in case they need to clean up
	//or to reset initial values of member variables
	fBoloSystem.Clear(anOption);
	fMuonSystem.Clear(anOption);
	
	//reset member variables to zero
	InitializeMembers();
																 
}

void KHLAEvent::InitializeMembers(void)
{

  SetRunNumber(-99);
  SetRunStartTime(-99.0);
	SetRunEndTime(-99.0);
  SetGSEventNumber(-99);
	
}

void KHLAEvent::CreateArrays(void)
{
	//Allocates memory for the KClonesArrays if they haven't already
	//been allocated.
	
	if(!fSamba)
		fSamba = new KClonesArray(KHLASambaRecord::Class(),2);
	
	if(!fBolo)
		fBolo = new KClonesArray(KHLABolometerRecord::Class(),5);
	
	if(!fBoloPulse)
		fBoloPulse = new KClonesArray(KHLABoloPulseRecord::Class(),20);
	
	if(!fMuonModule)
		fMuonModule = new KClonesArray(KHLAMuonModuleRecord::Class(),5);

	
	//why doesn't this create a memory leak? CreateArrays is called
	//by the default constructor. Does ROOT realize that I already 
	//have these KClonesArrays. Or maybe a KClonesArray object doesn't
	//require that much memory, so I don't notice the leak.
}

template<class T> T* KHLAEvent::AddSubRecord(TClonesArray *mArray)
{
	return static_cast<T* >(static_cast<KClonesArray *>(mArray)->GetNewOrCleanedObject(mArray->GetEntriesFast() ) );
}

void KHLAEvent::DeleteArray(Option_t *anOption, TClonesArray *mArray)
{
	if(mArray) {
		//we have to delete because our sub-records contain TString members! :(
		mArray->Delete( (anOption && *anOption) ? anOption : "C" );
	}
}


void KHLAEvent::ClearArray(Option_t *anOption, TClonesArray *mArray)
{
	if(mArray) {
		static_cast<KClonesArray *>(mArray)->Clear( (anOption && *anOption) ? anOption : "C" );
	}
}


KHLASambaRecord* KHLAEvent::AddSamba(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	return AddSubRecord<KHLASambaRecord>(fSamba);
}

KHLABolometerRecord* KHLAEvent::AddBolo(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kBoloTriggerType);
	return AddSubRecord<KHLABolometerRecord>(fBolo);
}

KHLABoloPulseRecord* KHLAEvent::AddBoloPulse()
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	return AddSubRecord<KHLABoloPulseRecord>(fBoloPulse);
}

KHLAMuonModuleRecord* KHLAEvent::AddMuonModule()
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kMuonVetoTriggerType);
	return AddSubRecord<KHLAMuonModuleRecord>(fMuonModule);
}

KHLASambaRecord *KHLAEvent::GetSamba(Int_t i) const
{
  // Return the i'th Samba Sub Record for this event.
	
  return static_cast<KHLASambaRecord *>(fSamba->At(i));
}

KHLABolometerRecord *KHLAEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.
	
  return static_cast<KHLABolometerRecord *>(fBolo->At(i));
}

KHLABoloPulseRecord *KHLAEvent::GetBoloPulse(Int_t i) const
{
  // Return the i'th Bolometer Pulse Sub Record for this event.
	
  return static_cast<KHLABoloPulseRecord *>(fBoloPulse->At(i));
}

KHLAMuonModuleRecord *KHLAEvent::GetMuonModule(Int_t i) const
{
  // Return the i'th Muon Module Sub Record for this event.
	
  return static_cast<KHLAMuonModuleRecord *>(fMuonModule->At(i));
}

Bool_t KHLAEvent::IsSame(const KHLAEvent &anEvent, Bool_t bPrint) const
{
	//Compares two KHLAEvents and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KEvent::IsSame(anEvent,bPrint)){
		if (bPrint) 
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	
	if(!fBoloSystem.IsSame(anEvent.fBoloSystem, bPrint)){
		if (bPrint) 
			cout << "KHLAEvent fBoloSystem Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	 
	if(!fMuonSystem.IsSame(anEvent.fMuonSystem, bPrint)){
		if (bPrint) 
			cout << "KHLAEvent fMuonSystem Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	if(GetNumBolos() == anEvent.GetNumBolos()){
		
		for(Int_t i = 0; i < GetNumBolos(); i++){
			KHLABolometerRecord *bolo = GetBolo(i);
			KHLABolometerRecord *boloOther = anEvent.GetBolo(i);
			if(bolo != 0 && boloOther != 0){
				if(!bolo->IsSame(*boloOther, bPrint)){
					if (bPrint) 
						cout << "KHLAEvent KHLABolometerRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
				
				//Check the Samba Records for Each of the Bolometer Records.
				KHLASambaRecord *sam = bolo->GetSambaRecord();
				KHLASambaRecord *samOther = boloOther->GetSambaRecord();
				
				if(sam != 0 && samOther != 0){
					if(!sam->IsSame(*samOther, bPrint)){
						if(bPrint)
							cout << "KHLAEvent KHLABolometerRecord number " << i << " Samba Records not equal" << endl;
						bIsEqual = false;
						if(!bPrint)
							return false;
					}
				}
				else {
					if(bPrint)
						cout << "KHLAEvent KHLABolometerRecord number " << i << " Got NULL Pointers for Samba Records" << endl;
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
				
				//Now, must check the Pulse Records.
				
				if(bolo->GetNumPulseRecords() == boloOther->GetNumPulseRecords()){
					for(Int_t k = 0; k < bolo->GetNumPulseRecords(); k++){
						KHLABoloPulseRecord *pulse = bolo->GetPulseRecord(k);
						KHLABoloPulseRecord *pulseOther = boloOther->GetPulseRecord(k);
						
						if(pulse != 0 && pulseOther != 0){
							if(!pulse->IsSame(*pulseOther, bPrint)){
								if(bPrint)
									cout << "KHLAEvent KHLABolometerRecord number " << i << " Pulse Record " << k << " not equal" << endl;
								bIsEqual = false;
								if(!bPrint)
									return false;
							}
						}
						else {
							if(bPrint)
								cout << "KHLAEvent KHLABolometerRecord number " << i << " Got NULL Pointers for Pulse Record" << k << endl;
							bIsEqual = false;
							if(!bPrint)
								return false;
						}
					}
				}
				else {
					if(bPrint)
						cout << "KHLAEvent KHLABolometerRecord number " << i << " Number of Pulse Records Not Equal" << endl;	
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			
				//okay!
			}
			else {
				if(bPrint)
					cout << "KHLAEvent KHLABolometerRecord number " << i << " Got NULL Pointers for Bolometer Record" << endl;
				bIsEqual = false;
				if(!bPrint)
					return false;
			}

		}
		
	}
	else {
		if (bPrint) 
			cout << "KHLAEvent number of bolometer records Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	
	if(GetNumMuonModules() == anEvent.GetNumMuonModules()){
		for(Int_t i = 0; i < GetNumMuonModules(); i++){
			KHLAMuonModuleRecord *s = GetMuonModule(i);
			KHLAMuonModuleRecord *sOther = anEvent.GetMuonModule(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "KHLAEvent KHLABoloPulseRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "KHLAEvent number of MuonModule Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
		
  if(fRunNumber != anEvent.fRunNumber){
		if (bPrint) 
			cout << "KHLAEvent fRunNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fRunStartTime != anEvent.fRunStartTime){
		if (bPrint) 
			cout << "KHLAEvent fRunStartTime Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
	
	if(fRunEndTime != anEvent.fRunEndTime){
		if (bPrint) 
			cout << "KHLAEvent fRunEndTime Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}

  
	if(fGSEventNumber != anEvent.fGSEventNumber){
		if (bPrint) 
			cout << "KHLAEvent fGSEventNumber Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  
	}
  
  
	return bIsEqual;
}

void KHLAEvent::Compact(void)
{
	//Make the event class as small as possible. This calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KEvent::Compact();
	
	fBoloSystem.Compact();
	fMuonSystem.Compact();
	
  
  fSamba->Compress();
  fBolo->Compress();
  fBoloPulse->Compress();
  fMuonModule->Compress();
  
	for(Int_t i = 0; i < GetNumSambas(); i++){
		KHLASambaRecord* samba = GetSamba(i);
		samba->Compact();
	}
	for(Int_t i = 0; i < GetNumBolos(); i++){
		KHLABolometerRecord* bolo = GetBolo(i);
		bolo->Compact();
	}
	for(Int_t i = 0; i < GetNumBoloPulses(); i++){
		KHLABoloPulseRecord* bp = GetBoloPulse(i);
		bp->Compact();
	}
	for(Int_t i = 0; i < GetNumMuonModules(); i++){
		KHLAMuonModuleRecord* module = GetMuonModule(i);
		module->Compact();
	}
}

//Easy Access / Event-Level Calculation Methods 

Double_t KHLAEvent::GetSumBoloEnergyRecoil(void) const
{
	//Returns the sum of the recoil energies for all
	//bolometer sub-records in the event.
	
	Double_t energy = 0;
	for(Int_t i = 0; i < GetNumBolos(); i++)
		energy += GetBolo(i)->GetEnergyRecoil();
	
	return energy;
}


Double_t KHLAEvent::GetSumBoloEnergyIon(void) const
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


