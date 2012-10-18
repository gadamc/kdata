//______________________________________________________________________
//
// KAmpBoloPulseRecord.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Holds the Raw-level pulse information, including the pulse trace
// This class is based on KBoloPulseRecord (which stores the channel
// number and type). 
//


#include "KAmpBoloPulseRecord.h"
#include "KRawSambaRecord.h"
#include "KPulseAnalysisRecord.h"
#include "TH1.h"
#include "TGraph.h"
#include <iostream>
#include <typeinfo>
#include <cstring>

using namespace std;

ClassImp(KAmpBoloPulseRecord);

KAmpBoloPulseRecord::KAmpBoloPulseRecord(void)
{
  //standard constructor

  InitializeMembers();
}


KAmpBoloPulseRecord::KAmpBoloPulseRecord(const KAmpBoloPulseRecord &aRec)
  : KBoloPulseRecord(aRec)
{
  //copy constructor 

  CopyLocalMembers(aRec);

}

KAmpBoloPulseRecord& KAmpBoloPulseRecord::operator=(const KAmpBoloPulseRecord &aRec)
{
  if(&aRec == this) return *this;

  this->KBoloPulseRecord::operator=(aRec);

  CopyLocalMembers(aRec);

  return *this;
}

void KAmpBoloPulseRecord::CopyLocalMembers(const KAmpBoloPulseRecord &/*aRec*/)
{
  //used in the assignment operator method, this copies over the local 
  //data members. It also set sets fBolometerRecord (the TRef pointer) to zero.

  fPulseAnaRecords.Delete();

}


KAmpBoloPulseRecord::~KAmpBoloPulseRecord(void)
{
  //destructor

}

void KAmpBoloPulseRecord::Clear(Option_t* /* opt */)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KAmpBoloPulseRecord::InitializeMembers(void)
{
  //init local members.

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

  fPulseAnaRecords.Delete();
}

Bool_t KAmpBoloPulseRecord::IsSame(const KAmpBoloPulseRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.

  Bool_t bIsEqual = true; //assume its true, then test for differences

  //call the base class's IsSame methods
  if(!this->KBoloPulseRecord::IsSame(aRec,bPrint)){
    bIsEqual = false;
    if(!bPrint)
      return false;  //if we're not printing out, just return false at first failure
                     //the operator== method uses this functionality.
  }

  return bIsEqual;
}

void KAmpBoloPulseRecord::AddPulseAnalysisRecord(KPulseAnalysisRecord* aPulseRecord)
{
  //add a pulse analysis record to this object

  //fPulseAnaRecords.Add((TObject *)aPulseRecord);
  fPulseAnaRecords.Add((TObject *)aPulseRecord);
}

KPulseAnalysisRecord* KAmpBoloPulseRecord::GetPulseAnalysisRecord(Int_t i) const
{
  //this returns the i^th pulse analysis record for this particular bolometer record.
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
  return (KPulseAnalysisRecord*) fPulseAnaRecords.At(i);
}

KPulseAnalysisRecord* KAmpBoloPulseRecord::GetPulseAnalysisRecord(const char* name) const
{
  //this returns the pulse analysis record with a particular name
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
  for (int i = 0; i < GetNumPulseAnalysisRecords(); i++){
    KPulseAnalysisRecord* r = GetPulseAnalysisRecord(i);
    if (strcmp(r->GetName(), name) == 0 )
      return r;
  }
  return 0;

}


void KAmpBoloPulseRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes

  KBoloPulseRecord::Compact();
  fPulseAnaRecords.Compress();
}
