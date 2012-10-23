//_____________________________________________
//
// KRawSambaRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Most of the Samba Sub Record data are in the base class KSambaRecord.
// In fact, there's nothing in this class at the moment. It may not make 
// sense in the end to have a "high-level" samba record
// since it only contains raw data. But then again, if there are 
// any raw-data 'corrections' that need to be made, this would be 
// the place for those corrected data to exist.
// 


#include "KRawSambaRecord.h"
#include <iostream>
using namespace std;

ClassImp(KRawSambaRecord);

KRawSambaRecord::KRawSambaRecord(void)
{

  InitializeMembers();
}

KRawSambaRecord::KRawSambaRecord(const KRawSambaRecord &aRec)
: KSambaRecord(aRec)
{
  CopyLocalMembers(aRec);
  
}

KRawSambaRecord& KRawSambaRecord::operator=(const KRawSambaRecord &aRec)
{
  if(&aRec == this) return *this;
  
  this->KSambaRecord::operator=(aRec);
  
  CopyLocalMembers(aRec);
  
  return *this;
}

void KRawSambaRecord::CopyLocalMembers(const KRawSambaRecord &aRec)
{
  //nothing to do
  fSambaDelay = aRec.fSambaDelay;
  fTriggerBit1 = aRec.fTriggerBit1;
  fTriggerBit2 = aRec.fTriggerBit2;
  fDataSource = aRec.fDataSource;
  fDataBufferLength = aRec.fDataBufferLength;
  fStreamMode = aRec.fStreamMode;
}

KRawSambaRecord::~KRawSambaRecord(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KRawSambaRecord::Clear(Option_t  *opt)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
  KSambaRecord::Clear(opt);
  
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KRawSambaRecord::InitializeMembers(void)
{
  //Init local members
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  fSambaDelay = -1;
  fTriggerBit1 = 0;
  fTriggerBit2 = 0;
  fDataSource.resize(0);
  fDataBufferLength = -1; 
  fStreamMode = -1;
}

Bool_t KRawSambaRecord::IsSame(const KRawSambaRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.
  
  Bool_t bIsEqual = true; //assume its true, then test for differences
  
  //call the base class's IsSame methods
  if(!this->KSambaRecord::IsSame(aRec,bPrint)){
    bIsEqual = false;
    if(!bPrint)
      return false;  //if we're not printing out, just return false at first failure
    //the operator== method uses this functionality.
  }
  
  if(fSambaDelay != aRec.fSambaDelay){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fSambaDelay Not Equal. lhs: " 
      << fSambaDelay << " != rhs " << aRec.fSambaDelay << endl;   
    else
      return false;  
  }

  if(fTriggerBit1 != aRec.fTriggerBit1){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fTriggerBit1 Not Equal. lhs: " 
      << fTriggerBit1 << " != rhs " << aRec.fTriggerBit1 << endl;   
    else
      return false;  
  }
  
  if(fTriggerBit2 != aRec.fTriggerBit2){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fTriggerBit2 Not Equal. lhs: " 
      << fTriggerBit2 << " != rhs " << aRec.fTriggerBit2 << endl;   
    else
      return false;  
  }
  
  if(fDataSource != aRec.fDataSource){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fDataSource Not Equal. lhs: " 
      << fDataSource << " != rhs " << aRec.fDataSource << endl;   
    else
      return false;  
  }
  if(fDataBufferLength != aRec.fDataBufferLength){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fDataBufferLength Not Equal. lhs: " 
      << fDataBufferLength << " != rhs " << aRec.fDataBufferLength << endl;   
    else
      return false;  
  }
  if(fStreamMode != aRec.fStreamMode){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawSambaRecord fStreamMode Not Equal. lhs: " 
      << fStreamMode << " != rhs " << aRec.fStreamMode << endl;   
    else
      return false;  
  }
  
  
  return bIsEqual;
}


void KRawSambaRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes
  
  KSambaRecord::Compact();
  
}

