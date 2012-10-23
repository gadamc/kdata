//_____________________________________________
//
// KBoloSystemRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This is base class for all Bolo System Records.
// It is currently empty since, given the Samba DAQ setup for Edewelss II and III
// all of the system information has to be packed into the bolometer data records.
// A change in DAQ will probably change this structure, and thus this class is kept
// in the data structure as a placeholder. 
// 


#include "KBoloSystemRecord.h"
#include <iostream>

ClassImp(KBoloSystemRecord);

KBoloSystemRecord::KBoloSystemRecord(void)
{
  //standard constructor
  
  InitializeMembers();
}

KBoloSystemRecord::~KBoloSystemRecord(void)
{
  //destructor
  
  //Does calling clear at destruction take too much computing time?
  Clear("C");
}

KBoloSystemRecord::KBoloSystemRecord(const KBoloSystemRecord &aRec)
{
  //copy constructor 
  
  CopyLocalMembers(aRec);
}

KBoloSystemRecord& KBoloSystemRecord::operator=(const KBoloSystemRecord &aRec)
{
  //assignment operator
  
  if(&aRec == this) return *this;
  
  CopyLocalMembers(aRec);
  return *this;
}

void KBoloSystemRecord::CopyLocalMembers(const KBoloSystemRecord &/*aRec*/)
{

  //fRunName = aRec.fRunName;

}

void KBoloSystemRecord::Clear(Option_t * /*opt*/)
{
  //Clear the base classes and then clear/delete any local
  //members. 

  //Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  
  //Clear and delete local objects here. 
  
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
}


void KBoloSystemRecord::InitializeMembers(void)
{
  //Initialize local members.
  
  //fRunName = "";
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

/*
Double_t KBoloSystemRecord::GetTimeSinceLastBoloSysEvent(void) const
{
  return GetTimeSinceLastSystemRecord();  //call the base class's TSLSystemRecord
}

void KBoloSystemRecord::SetTimeSinceLastBoloSysEvent(Double_t aTime)
{
  SetTimeSinceLastSystemRecord(aTime);  //call the base class's TSLSystemRecord
}
*/

Bool_t KBoloSystemRecord::IsSame(const KBoloSystemRecord & /*aRec*/, Bool_t /*bPrint*/) const
{
  Bool_t bIsEqual = true; //assume its true, then test for differences
  
  
  /*if(fRunName != aRec.fRunName){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloSystemRecord fDistanceEst Not Equal. lhs: " 
      << fRunName << " != rhs " << aRec.fRunName << endl; 
    else
      return false;  
  }*/

  
  return bIsEqual;
  
}

void KBoloSystemRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes
  
  
}
