//_____________________________________________
//
// KRawEvent.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de>
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the main Event class for Raw level analysis. One typically
// begins all analysis by getting a pointer to this object from 
// the KDataReader object. By calling KDataReader::GetEntry(Int_t i)
// or KDataReader::GetNextEntry, one can loop through the KDS data
// file and examine each event separately. From this object
// one can obtain pointers to they Muon and Bolometer System Records
// GetBoloSystemRecord(void), GetMuonVetoSystemRecord()
// and to each of the event's Sub Records. The raw level data file
// should contain the bolometer pulse traces for each event.
//
// To allocate memory and get a pointer to an object of this type from the KEventFactory, 
// use the global varaibles
// const Char_t kRawEventName[] = "Raw";
// const Char_t kRawClassName[] = "KRawEvent";
//
// to call
//
// KRawEvent *rawEvent = KEventFactory::NewEvent("Raw");
// KRawEvent *rawEvent = KEventFactory::NewEvent("KRawEvent");
//  
// or, simply use
//
// KRawEvent *rawEvent = KEventFactory::NewRawEvent();
//

#include "KRawEvent.h"
#include "KHLAEvent.h"
#include "TProcessID.h"
#include "TClonesArray.h"

//sub record includes
#include "KRawSambaRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KRawMuonModuleRecord.h"

#include <typeinfo>
#include <iostream>
#include <exception>

using namespace std;

ClassImp(KRawEvent);

KRawEvent::KRawEvent(void)
{
	//Default constructor 
	InitializeMembers();
  
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

KRawEvent::KRawEvent(const KRawEvent &anEvent)
: KEvent(anEvent)
{
  //Copy Constructor. The TRefs will point to newly created objects
	//and not to the data members of the KRawEvent object you passed in.
	
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


KRawEvent::~KRawEvent(void) 
{
  //destructor 
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


KRawEvent& KRawEvent::operator=(const KEvent &anEvent)
{
  //assignment operator
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = base" << endl;
#endif
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

KRawEvent& KRawEvent::operator=(const KRawEvent &anEvent)
{
  //assignment operator
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = raw" << endl;
#endif
	if(&anEvent == this) return *this;

	//copy the base class info.
	this->KEvent::operator=(anEvent);

	CopyLocalMembers(anEvent);
	CopyClonesArrays(anEvent);
  
	return *this;	
}

KRawEvent& KRawEvent::operator=(const KHLAEvent &anEvent)
{
  //assignment operator - only assigns the common data found in the 
  //base class.
  
#ifdef _K_DEBUG_EVENT_ASSIGNOP
	cout << "raw = hla" << endl;
#endif
	try {
		const KEvent &rhs = dynamic_cast<const KEvent&>(anEvent);
		if(&rhs == this) return *this;
		//copy the base class info.
		this->KEvent::operator=(rhs);
    //CopyFromHlaEvent(anEvent);
	}
	catch (bad_cast) {
		cout << "THROW!" << endl;
		//what should i do? this should never throw!
		throw;  
	}
	
	return *this;
}


UInt_t KRawEvent::GetLargestUniqueIDNumber(void)
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

Int_t KRawEvent::AddSubRecords(const KRawEvent &anEvent)
{
  //Adds the sub records of 'anEvent' to *this. 
  //returns the number of sub records added to *this.
  //
  //hmmm... should this method be in the KEventFactory?  Or somewhere other
  //than the KRawEvent object?
	
  Int_t numRecord = 0;
	
	
	Int_t ObjectNumber = TProcessID::GetObjectCount(); //save this number for later.
	
	//but first, we have to see what unique ID numbers exist within this particular event
	//and start the unique ID iterations from that point! otherwise, we'll get objects in the
	//event with the same unique ID number and the TRefs won't work. 
	UInt_t aNumber = GetLargestUniqueIDNumber();
	if(aNumber != 0) TProcessID::SetObjectCount(aNumber);
	
  
	KRawSambaRecord mySamba;
	KRawSambaRecord mySambaOrig;
	//KRawBolometerRecord myBolo;
	
	for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
		KRawBolometerRecord *bolo0 = anEvent.GetBolo(i);
		
	   
    KRawBolometerRecord *bolo = AddBolo();
    numRecord++;
    //Int_t fSambaNum = bolo0->GetSambaRecordNum();
    KRawSambaRecord *samba0 = bolo0->GetSambaRecord();
    //samba0->print();
    mySambaOrig = *samba0;
    KRawSambaRecord *samba = 0;
    
    if(bolo != 0 && bolo0 != 0) 
      *bolo = *bolo0;  //copies everything but the TRef and TRefArray
    else
      cerr << "KRawEvent::AddSubRecords Invalid KRawBolometerRecord Pointer" << endl;
    
    if(bolo0->GetSambaRecord() != 0) {
      mySamba = mySambaOrig;
      //first, have to check to see if there already exists a samba sub record that is exactly like this one.
      Bool_t bAddSamba = true;
      for(Int_t n = 0; n < GetNumSambas(); n++){
        samba = GetSamba(n);
        
        if(*samba == mySamba){  //we found a match for a samba record already in our event! Just set the samba record without creating a new Samba Record
                                //cout << "Samba Match Found" << endl;
                                //this->myPrint();
                                //anEvent->myPrint();
          bolo->SetSambaRecord(samba);
          bAddSamba = false;
          n = GetNumSambas();
        }
        
      }
      if(bAddSamba==true){
        //cout << "Add new Samba" << endl;
        samba = AddSamba();
        numRecord++;
        *samba = mySamba;
        bolo->SetSambaRecord(samba);
      }
      
    }
    else
      cerr << "KRawEvent::AddSubRecords Invalid KRawSambaRecord Pointer" << endl;
    
    
    UInt_t numPulseRecords = bolo0->GetNumPulseRecords();
    
    for(UInt_t j = 0; j < numPulseRecords; j++){
      KRawBoloPulseRecord *pulse0 = bolo0->GetPulseRecord(j);
      if(pulse0 != 0){
        KRawBoloPulseRecord *pulse = AddBoloPulse();
        numRecord++;
        if(pulse != 0){
          *pulse = *pulse0;
          bolo->AddPulseRecord(pulse);
          pulse->SetBolometerRecord(bolo);
        }
        else {
          cerr << "KRawEvent::AddSubRecords AddBoloPulse returned an invalid KRawBoloPulseRecord Pointer" << endl;
        }
      }
      
      else {
        cerr << "KRawEvent::AddSubRecords Invalid KRawBoloPulseRecord Pointer" << endl;
      }
      
      
    } 
	}
	
  
	for(Int_t i = 0; i < anEvent.GetNumMuonModules(); i++){
		KRawMuonModuleRecord *s = AddMuonModule();
		KRawMuonModuleRecord *sO = anEvent.GetMuonModule(i);
		numRecord++;
		if(s != 0 && sO != 0) 
			*s = *sO;
		else
			cerr << "KRawEvent::operator= Invalid MuonModule Pointer" << endl;
	}
	
	//Restore Object count                                                                                                     
	//To save space in the table keeping track of all referenced objects 
	//and computation time,
	//we assume that our events DO NOT address each other. We reset the                                                        
	//object count to what it was at the beginning of the event.                                                               
	TProcessID::SetObjectCount(ObjectNumber);
  
	return numRecord;
}




void KRawEvent::CopyClonesArrays(const KRawEvent &anEvent)
{
    
  ClearArrays("C");
  AddSubRecords(anEvent);
}


void KRawEvent::CopyLocalMembers(const KRawEvent &anEvent)
{
  //copies local data members
  
  //fDataCleaningWord = anEvent.fDataCleaningWord;

	fMuonSystem = anEvent.fMuonSystem;
	fNumSamba = anEvent.fNumSamba;
	fNumBolo = anEvent.fNumBolo;
	fNumBoloPulse = anEvent.fNumBoloPulse;
	fNumMuonModule = anEvent.fNumMuonModule;

}



void KRawEvent::ClearArrays(Option_t *anOption)
{
	DeleteArray(anOption, fSamba,fNumSamba);  //have to call delete on Samba array because the KSambaRecord class contains a string
	DeleteArray(anOption, fBolo,fNumBolo); // also contains a string, like KSambaRecord
	ClearArray(anOption, fBoloPulse,fNumBoloPulse);
	ClearArray(anOption, fMuonModule,fNumMuonModule);
}

void KRawEvent::Clear(Option_t *opt)
{
  ClearArrays(opt);
	//delete any memory allocated by the KEvent class here that
	//needs to be deleted for each event -- but not the TClonesArrays. Clear should be called 
	//after every event so that the TClonesArrays are cleared. 
	KEvent::Clear(opt);
	
	//Call the System Record clears in case they need to clean up
	//or to reset initial values of member variables

	fMuonSystem.Clear(opt);
	
	//reset member variables to zero
	InitializeMembers();
}

void KRawEvent::InitializeMembers(void)
{
	
}

void KRawEvent::CreateArrays(void)
{
  
	//Allocates memory for the TClonesArrays if they haven't already
	//been allocated.
	
	if(!fSamba)
		fSamba = new TClonesArray("KRawSambaRecord",2);
	
	if(!fBolo)
		fBolo = new TClonesArray("KRawBolometerRecord",5);
	
	if(!fBoloPulse)
		fBoloPulse = new TClonesArray("KRawBoloPulseRecord",20);
	
	if(!fMuonModule)
		fMuonModule = new TClonesArray("KRawMuonModuleRecord",5);
  
	
	//why doesn't this create a memory leak? CreateArrays is called
	//by the default constructor. Does ROOT realize that I already 
	//have these TClonesArrays. Or maybe a TClonesArray object doesn't
	//require that much memory, so I don't notice the leak.
  
}

template<class T> T* KRawEvent::AddSubRecord(TClonesArray *mArray, Int_t &mCount)
{
	TClonesArray &mArrayRef = *mArray;
	T* mNewSubRecord = new(mArrayRef[mCount++]) T;
	return mNewSubRecord;
}

void KRawEvent::DeleteArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		//we have to delete because our sub-records contain TString members! :(
		mArray->Delete( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}


void KRawEvent::ClearArray(Option_t *anOption, TClonesArray *mArray, Int_t &mCount)
{
	if(mArray) {
		mArray->Clear( (anOption && *anOption) ? anOption : "C" );
		mCount = 0;
	}
}


KRawSambaRecord* KRawEvent::AddSamba(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	return AddSubRecord<KRawSambaRecord>(fSamba,fNumSamba);
}

KRawBolometerRecord* KRawEvent::AddBolo(void)
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kBoloTriggerType);
	return AddSubRecord<KRawBolometerRecord>(fBolo,fNumBolo);
}


KRawBoloPulseRecord* KRawEvent::AddBoloPulse(void)
{
  //Use this event only when creating an event and you want to add
	//a new SubRecord.
  
  AddTriggerType(kBoloTriggerType);
  return AddSubRecord<KRawBoloPulseRecord>(fBoloPulse,fNumBoloPulse);
}


KRawMuonModuleRecord* KRawEvent::AddMuonModule()
{
	//Use this event only when creating an event and you want to add
	//a new SubRecord.
	
	AddTriggerType(kMuonVetoTriggerType);
	return AddSubRecord<KRawMuonModuleRecord>(fMuonModule,fNumMuonModule);
}

KRawSambaRecord *KRawEvent::GetSamba(Int_t i) const
{
  // Return the i'th Samba Sub Record for this event.
	
  KRawSambaRecord *ms = 0;
  if (i < fNumSamba && i >= 0) ms = (KRawSambaRecord *)fSamba->At(i);
  return ms;
}

KRawBolometerRecord *KRawEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.
	
  KRawBolometerRecord *ms = 0;
  if (i < fNumBolo && i >= 0) ms = (KRawBolometerRecord *)fBolo->At(i);
  return ms;
}

KRawBoloPulseRecord *KRawEvent::GetBoloPulse(Int_t i) const
{
  // Return the i'th Bolometer Pulse Sub Record for this event.
	
  KRawBoloPulseRecord *ms = 0;
  if (i < fNumBoloPulse && i >= 0) ms = (KRawBoloPulseRecord *)fBoloPulse->At(i);
  return ms;
}

KRawMuonModuleRecord *KRawEvent::GetMuonModule(Int_t i) const
{
  // Return the i'th Muon Module Sub Record for this event.
	
  KRawMuonModuleRecord *ms = 0;
  if (i < fNumMuonModule && i >= 0) ms = (KRawMuonModuleRecord *)fMuonModule->At(i);
  return ms;
}

Bool_t KRawEvent::IsSame(const KRawEvent &anEvent, Bool_t bPrint) const
{
  //Compares two KRawEvents and their member variables to test for equality.
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
	
	
  
	if(!fMuonSystem.IsSame(anEvent.fMuonSystem, bPrint)){
		if (bPrint) 
			cout << "KRawEvent fMuonSystem Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	if(fNumBolo == anEvent.fNumBolo){
		
		for(Int_t i = 0; i < fNumBolo; i++){
			KRawBolometerRecord *bolo = GetBolo(i);
			KRawBolometerRecord *boloOther = anEvent.GetBolo(i);
			if(bolo != 0 && boloOther != 0){
				if(!bolo->IsSame(*boloOther, bPrint)){
					if (bPrint) 
						cout << "KRawEvent KRawBolometerRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
				
				//Check the Samba Records for Each of the Bolometer Records.
				KRawSambaRecord *sam = bolo->GetSambaRecord();
				KRawSambaRecord *samOther = boloOther->GetSambaRecord();
				
				if(sam != 0 && samOther != 0){
					if(!sam->IsSame(*samOther, bPrint)){
						if(bPrint)
							cout << "KRawEvent KRawBolometerRecord number " << i << " Samba Records not equal" << endl;
						bIsEqual = false;
						if(!bPrint)
							return false;
					}
				}
				else {
					if(bPrint)
						cout << "KRawEvent KRawBolometerRecord number " << i << " Got NULL Pointers for Samba Records" << endl;
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
				
				//Now, must check the Pulse Records.
				
				if(bolo->GetNumPulseRecords() == boloOther->GetNumPulseRecords()){
					for(Int_t k = 0; k < bolo->GetNumPulseRecords(); k++){
						KRawBoloPulseRecord *pulse = bolo->GetPulseRecord(k);
						KRawBoloPulseRecord *pulseOther = boloOther->GetPulseRecord(k);
						
						if(pulse != 0 && pulseOther != 0){
							if(!pulse->IsSame(*pulseOther, bPrint)){
								if(bPrint)
									cout << "KRawEvent KRawBolometerRecord number " << i << " Pulse Record " << k << " not equal" << endl;
								bIsEqual = false;
								if(!bPrint)
									return false;
							}
						}
						else {
							if(bPrint)
								cout << "KRawEvent KRawBolometerRecord number " << i << " Got NULL Pointers for Pulse Record" << k << endl;
							bIsEqual = false;
							if(!bPrint)
								return false;
						}
					}
				}
				else {
					if(bPrint)
						cout << "KRawEvent KRawBolometerRecord number " << i << " Number of Pulse Records Not Equal" << endl;	
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
        
				//okay!
			}
			else {
				if(bPrint)
					cout << "KRawEvent KRawBolometerRecord number " << i << " Got NULL Pointers for Bolometer Record" << endl;
				bIsEqual = false;
				if(!bPrint)
					return false;
			}
      
		}
		
	}
	else {
		if (bPrint) 
			cout << "KRawEvent fNumBolo Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
	
	
	
	if(fNumMuonModule == anEvent.fNumMuonModule){
		for(Int_t i = 0; i < fNumMuonModule; i++){
			KRawMuonModuleRecord *s = GetMuonModule(i);
			KRawMuonModuleRecord *sOther = anEvent.GetMuonModule(i);
			if(s != 0 && sOther != 0){
				if(!s->IsSame(*sOther, bPrint)){
					if (bPrint) 
						cout << "KRawEvent KRawBoloPulseRecord number " << i << " Not Equal" << endl;		
					bIsEqual = false;
					if(!bPrint)
						return false;
				}
			}
		}
	}
	else {
		if (bPrint) 
			cout << "KRawEvent fNumMuonModule Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;
	}
  
	return bIsEqual;
  
}


void KRawEvent::Compact(void)
{
	//Make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KEvent::Compact();
  
  fMuonSystem.Compact();
	
	for(Int_t i = 0; i < GetNumSambas(); i++){
		KRawSambaRecord* samba = GetSamba(i);
		samba->Compact();
	}
	for(Int_t i = 0; i < GetNumBolos(); i++){
		KRawBolometerRecord* bolo = GetBolo(i);
		bolo->Compact();
	}
	for(Int_t i = 0; i < GetNumBoloPulses(); i++){
		KRawBoloPulseRecord* bp = GetBoloPulse(i);
		bp->Compact();
	}
	for(Int_t i = 0; i < GetNumMuonModules(); i++){
		KRawMuonModuleRecord* module = GetMuonModule(i);
		module->Compact();
	}

  
}

	
