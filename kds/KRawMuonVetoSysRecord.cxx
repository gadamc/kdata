//_____________________________________________
//
// KRawMuonVetoSysRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is the Raw Level Muon Veto System Record. It contains information
// about the Bolometer System that we store ONLY in the Raw Level Files.
// Most likely, at this point, this class is empty and you should
// refer to the base-class, KBoloSystemRecord, for more information
// about what is stored in this class. 
//
//


#include "KRawMuonVetoSysRecord.h"
#include <iostream>
using namespace std;

ClassImp(KRawMuonVetoSysRecord);

KRawMuonVetoSysRecord::KRawMuonVetoSysRecord(void)
{

  InitializeMembers();
}

KRawMuonVetoSysRecord::~KRawMuonVetoSysRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

KRawMuonVetoSysRecord::KRawMuonVetoSysRecord(const KRawMuonVetoSysRecord &aRec)
: KMuonVetoSystemRecord(aRec)
{
  CopyLocalMembers(aRec);
}

KRawMuonVetoSysRecord& KRawMuonVetoSysRecord::operator=(const KRawMuonVetoSysRecord &aRec)
{
  if(&aRec == this) return *this;
  
  this->KMuonVetoSystemRecord::operator=(aRec);
  
  CopyLocalMembers(aRec);
  
  return *this;
}

void KRawMuonVetoSysRecord::CopyLocalMembers(const KRawMuonVetoSysRecord &/*aRec*/)
{
  //nothing to do! No local members yet - see the base class. :)
}


void KRawMuonVetoSysRecord::Clear(Option_t *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
  KMuonVetoSystemRecord::Clear(opt);
  
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KRawMuonVetoSysRecord::InitializeMembers(void)
{
  //
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

Bool_t KRawMuonVetoSysRecord::IsSame(const KRawMuonVetoSysRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.
  
  Bool_t bIsEqual = true; //assume its true, then test for differences
  
  //call the base class's IsSame methods
  if(!this->KMuonVetoSystemRecord::IsSame(aRec,bPrint)){
    bIsEqual = false;
    if(!bPrint)
      return false;  //if we're not printing out, just return false at first failure
    //the operator== method uses this functionality.
  }

  
  return bIsEqual;

}

void KRawMuonVetoSysRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes
  
  KMuonVetoSystemRecord::Compact();
  
}
