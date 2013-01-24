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
//

#include "KRawEvent.h"
#include "TProcessID.h"
//#include "KClonesArray.h"

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


  fSamba = 0;
  fBolo = 0;
  fBoloPulse = 0;
  fMuonModule = 0;


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
  //using KClonesArray's copy constructors. 

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

  //Delete KClonesArrays
  if(fSamba) delete fSamba;
  fSamba = 0;

  if(fBolo) delete fBolo;
  fBolo = 0;

  if(fBoloPulse) delete fBoloPulse;
  fBoloPulse = 0;

  if(fMuonModule) delete fMuonModule;
  fMuonModule = 0;


}


KEvent& KRawEvent::operator=(const KEvent &anEvent)
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
    //just copy the base class information
    this->KEvent::operator=(anEvent);
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


UInt_t KRawEvent::GetLargestUniqueIDNumber(void)
{
  //returns the largest unique ID number for TRefs in this
  //event. This is used for adding new subrecords to events, which
  //is needed in order to "merge" the data. 
  //
  // Note to developers: the location of some code in kdata could be rethought
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

Int_t KRawEvent::AddSubRecords(const KEvent &rhs)
{
  //Adds the sub records of 'anEvent' to *this. 
  //returns the number of sub records added to *this.
  //
  //hmmm... should this method be in the KEventFactory?  Or somewhere other
  //than the KRawEvent object?

  try {
    const KRawEvent &anEvent = dynamic_cast<const KRawEvent &> (rhs);

    Int_t numRecord = 0;

    for(Int_t i = 0; i < anEvent.GetNumBolos(); i++){
      if(AddBoloSubRecord(*anEvent.GetBolo(i))) numRecord++;
      else return -1;
    }

    for(Int_t i = 0; i < anEvent.GetNumMuonModules(); i++){
      if(AddMuonModuleSubRecord(*anEvent.GetMuonModule(i))) numRecord++;
      else return -1; 
    }

    return numRecord;

  }
  catch(bad_cast) {
    cerr << "rhs not KRawEvent type." << endl;
    return -1;
  }


}

Bool_t KRawEvent::AddBoloSubRecord(const KBolometerRecord &rhs)
{
  //Add rhs to this KRawEvent. This also automatically adds the Samba record that it points
  //to and the Pulse Records. If no Samba record is associated with rhs (via GetSambaRecord),
  //this method returns false. If an invalid pointer to a Pulse Record is found, 
  //this method returns false. This ensures that the event is built properly. 


  try {
    const KRawBolometerRecord &inbolo = dynamic_cast<const KRawBolometerRecord &> (rhs);

    Int_t ObjectNumber = TProcessID::GetObjectCount(); //save this number for later.

    //but first, we have to see what unique ID numbers exist within this particular event
    //and start the unique ID iterations from that point! otherwise, we'll get objects in the
    //event with the same unique ID number and the TRefs won't work. 
    UInt_t aNumber = GetLargestUniqueIDNumber();
    if(aNumber != 0) TProcessID::SetObjectCount(aNumber);

    KRawBolometerRecord *newbolo = AddBolo();
    if (newbolo != 0)
      *newbolo = inbolo;
    else {
      cerr << "KRawEvent::AddBoloSubRecord Invalid new KRawBolometerRecord Pointer" << endl;
      TProcessID::SetObjectCount(ObjectNumber);
      return false;
    }

    KRawSambaRecord *inSamba = inbolo.GetSambaRecord();

    if(inSamba != 0){
      //first, have to check to see if there already exists a samba sub record that is exactly like this one.
      Bool_t bAddSamba = true;

      for(Int_t n = 0; n < GetNumSambas(); n++){
        KRawSambaRecord *samba = static_cast<KRawSambaRecord *>(GetSamba(n));

        if(*samba == *inSamba){
          //found the match. this samba is already in our event
          newbolo->SetSambaRecord(samba);
          bAddSamba = false;
          break;
        }
      }

      //if we didn't find an already existing samba record, then we make 
      //a new one and copy the contents
      if(bAddSamba){
        KRawSambaRecord *samba = AddSamba();
        *samba = *inSamba;
        newbolo->SetSambaRecord(samba);
      }
    }
    else {
      cerr << "KRawEvent::AddBoloSubRecord Invalid KRawSambaRecord Pointer" << endl;
      TProcessID::SetObjectCount(ObjectNumber);
      return false;
    }

    //now just add all of the pulse records

    for(Int_t j = 0; j < inbolo.GetNumPulseRecords(); j++){
      KRawBoloPulseRecord *inpulse = inbolo.GetPulseRecord(j);
      if(inpulse != 0){

        KRawBoloPulseRecord *newpulse = AddBoloPulse();

        if(newpulse != 0){
          *newpulse = *inpulse;
          newbolo->AddPulseRecord(newpulse);
          newpulse->SetBolometerRecord(newbolo);
        }
        else {
          cerr << "KRawEvent::AddBoloSubRecord. AddBoloPulse returned a null KRawBoloPulseRecord Pointer" << endl;
          TProcessID::SetObjectCount(ObjectNumber);
          return false;
        }
      }
      else {
        cerr << "KRawEvent::GetPulseRecord return a null KRawBoloPulseRecord Pointer" << endl;
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
  catch(bad_cast) {
    cerr << "rhs not KRawBolometerRecord type." << endl;
    return false;
  }


}


Bool_t KRawEvent::AddMuonModuleSubRecord(const KMuonModuleRecord &rhs)
{
  //Add muon module to the is event.

  //don't need to deal with object count here because we're not using
  //TRefs with the muon module data. 

  try {
    const KRawMuonModuleRecord &inMuonModule = dynamic_cast<const KRawMuonModuleRecord &> (rhs);

    KRawMuonModuleRecord *muonmodule = AddMuonModule();

    if(muonmodule != 0){
      *muonmodule = inMuonModule;
      return true;
    }
    else return false; //only return false if AddMuonModule() doesn't work
  }
  catch(bad_cast) {
    cerr << "rhs not KRawMuonModuleRecord type." << endl;
    return false;
  }



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

}



void KRawEvent::ClearArrays(Option_t *anOption)
{
  ClearArray(anOption, fSamba);  
  ClearArray(anOption, fBolo); 
  ClearArray(anOption, fBoloPulse);
  ClearArray(anOption, fMuonModule);
}

void KRawEvent::Clear(Option_t *opt)
{
  ClearArrays(opt);
  //delete any memory allocated by the KEvent class here that
  //needs to be deleted for each event -- but not the KClonesArrays. Clear should be called 
  //after every event so that the KClonesArrays are cleared. 
  KEvent::Clear(opt);

  //Call the System Record clears in case they need to clean up
  //or to reset initial values of member variables

  fMuonSystem.Clear(opt);

}



void KRawEvent::CreateArrays(void)
{

  //Allocates memory for the KClonesArrays if they haven't already
  //been allocated.
  //This is not called in the constructor for ROOT I/O reasons. 

  if(!fSamba)
    fSamba = new TClonesArray(KRawSambaRecord::Class(),2);

  if(!fBolo)
    fBolo = new TClonesArray(KRawBolometerRecord::Class(),5);

  if(!fBoloPulse)
    fBoloPulse = new TClonesArray(KRawBoloPulseRecord::Class(),20);

  if(!fMuonModule)
    fMuonModule = new TClonesArray(KRawMuonModuleRecord::Class(),5);


}



void KRawEvent::ClearArray(Option_t *anOption, TClonesArray *mArray)
{
  if(mArray) {
    mArray->Clear( (anOption && *anOption) ? anOption : "C" );
  }
}


KRawSambaRecord* KRawEvent::AddSamba(void)
{
  //Use this event only when creating an event and you want to add
  //a new SubRecord.

  return AddSubRecord<KRawSambaRecord>(fSamba);
}

KRawBolometerRecord* KRawEvent::AddBolo(void)
{
  //Use this event only when creating an event and you want to add
  //a new SubRecord.

  AddTriggerType(kBoloTriggerType);
  return AddSubRecord<KRawBolometerRecord>(fBolo);
}


KRawBoloPulseRecord* KRawEvent::AddBoloPulse(void)
{
  //Use this event only when creating an event and you want to add
  //a new SubRecord.

  AddTriggerType(kBoloTriggerType);
  return AddSubRecord<KRawBoloPulseRecord>(fBoloPulse);
}


KRawMuonModuleRecord* KRawEvent::AddMuonModule()
{
  //Use this event only when creating an event and you want to add
  //a new SubRecord.

  AddTriggerType(kMuonVetoTriggerType);
  return AddSubRecord<KRawMuonModuleRecord>(fMuonModule);
}

KSambaRecord *KRawEvent::GetSamba(Int_t i) const
{
  // Return the i'th Samba Sub Record for this event.

  return static_cast<KSambaRecord *>(fSamba->At(i));
}

KBolometerRecord *KRawEvent::GetBolo(Int_t i) const
{
  // Return the i'th Bolometer Sub Record for this event.

  return static_cast<KBolometerRecord *>(fBolo->At(i));
}

KBoloPulseRecord *KRawEvent::GetBoloPulse(Int_t i) const
{
  // Return the i'th Bolometer Pulse Sub Record for this event.

  return static_cast<KBoloPulseRecord *>(fBoloPulse->At(i));
}

KBolometerRecord *KRawEvent::GetBolo(const char* name) const
{
  //Return the first found Bolometer Sub Record with detector name == name.
  //this searches in order from i = 0 to i = GetNumBolos()-1
  for (int i = 0; i < GetNumBolos(); i++){
    KBolometerRecord *b = GetBolo(i);
    if(b)
      if( strcmp(b->GetDetectorName(), name) == 0 ) return b;
  }
  return 0;
}

KBoloPulseRecord *KRawEvent::GetBoloPulse(const char* name) const
{
  //Return the first found Bolometer Pulse Record with channel name == name.
  //this searches in order from i = 0 to i = GetNumBoloPulses()-1
  for (int i = 0; i < GetNumBoloPulses(); i++){
    KBoloPulseRecord *b = GetBoloPulse(i);
    if(b)
      if( strcmp(b->GetChannelName(), name) == 0 ) return b;
  }
  return 0;
  
}


KMuonModuleRecord *KRawEvent::GetMuonModule(Int_t i) const
{
  // Return the i'th Muon Module Sub Record for this event.

  return static_cast<KMuonModuleRecord *>(fMuonModule->At(i));
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


  if(GetNumBolos() == anEvent.GetNumBolos()){

    for(Int_t i = 0; i < GetNumBolos(); i++){
      KRawBolometerRecord *bolo = static_cast<KRawBolometerRecord *>(GetBolo(i));
      KRawBolometerRecord *boloOther = static_cast<KRawBolometerRecord *>(anEvent.GetBolo(i));
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
      cout << "KRawEvent size of Bolo Array Not Equal" << endl;   
    bIsEqual = false;
    if(!bPrint)
      return false;
  }



  if(GetNumMuonModules() == anEvent.GetNumMuonModules()){
    for(Int_t i = 0; i < GetNumMuonModules(); i++){
      KRawMuonModuleRecord *s = static_cast<KRawMuonModuleRecord *>(GetMuonModule(i));
      KRawMuonModuleRecord *sOther = static_cast<KRawMuonModuleRecord *>(anEvent.GetMuonModule(i));
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
      cout << "KRawEvent size of muon module array Not Equal" << endl;    
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

  fSamba->Compress();
  fBolo->Compress();
  fBoloPulse->Compress();
  fMuonModule->Compress();

  for(Int_t i = 0; i < GetNumSambas(); i++){
    KRawSambaRecord* samba = static_cast<KRawSambaRecord *>(GetSamba(i));
    samba->Compact();
  }
  for(Int_t i = 0; i < GetNumBolos(); i++){
    KRawBolometerRecord* bolo = static_cast<KRawBolometerRecord *>(GetBolo(i));
    bolo->Compact();
  }
  for(Int_t i = 0; i < GetNumBoloPulses(); i++){
    KRawBoloPulseRecord* bp = static_cast<KRawBoloPulseRecord *>(GetBoloPulse(i));
    bp->Compact();
  }
  for(Int_t i = 0; i < GetNumMuonModules(); i++){
    KRawMuonModuleRecord* module = static_cast<KRawMuonModuleRecord *>(GetMuonModule(i));
    module->Compact();
  }


}


