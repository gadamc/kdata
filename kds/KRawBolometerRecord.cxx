//_____________________________________________
//
// KRawBolometerRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The Raw-tier class for Single Bolo Sub Records. 
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
  fPulseRecords.Delete();
}


KRawBolometerRecord::~KRawBolometerRecord(void)
{
  //destructor 
  
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
  fPulseRecords.Delete();

}


Bool_t KRawBolometerRecord::IsSame(const KRawBolometerRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.
  
  Bool_t bIsEqual = true; //assume its true, then test for differences

  if(!this->KBolometerRecord::IsSame(aRec,bPrint)){
    bIsEqual = false;
    if(!bPrint)
      return false;  //if we're not printing out, just return false at first failure
    //the operator== method uses this functionality.
  }

  return bIsEqual;
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

