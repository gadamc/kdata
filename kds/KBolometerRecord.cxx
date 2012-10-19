//_____________________________________________
//
// KBolometerRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// A base class for all SingleBolo Sub Records. The SingleBolo Sub records
// contain data from an individual bolometer. Currently, it only holds
// the bolometer detector name. All other data are found in the Raw and HLA
// bolometer record files. 
//

#include "KBolometerRecord.h"
#include <stdio.h>
#include <iostream>
#include <cstring> 

using namespace std;

ClassImp(KBolometerRecord);

KBolometerRecord::KBolometerRecord(void)
{
  //constructor 
  
  InitializeMembers();
  
  //we must take care of members on the heap outside of 
  //InitializeMembers...
  
}

KBolometerRecord::KBolometerRecord(const KBolometerRecord &aRec)
  : TObject()
{
  //copy constructor 
  
  CopyLocalMembers(aRec);
  
}

KBolometerRecord& KBolometerRecord::operator=(const KBolometerRecord &aRec)
{
  //assignment operator
  
  if(&aRec == this) return *this;
  
  CopyLocalMembers(aRec);
  
  
  return *this;
}

void KBolometerRecord::CopyLocalMembers(const KBolometerRecord &aRec)
{
  
  SetDetectorName(aRec.fDetectorName.c_str());
  SetMass(aRec.GetMass());
  
}

KBolometerRecord::~KBolometerRecord(void)
{
  //destructor 
  
  //Does calling clear at destruction take too much computing time?
  Clear("C");
  
  
}

void KBolometerRecord::Clear(Option_t * /*opt*/)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
  
  
  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
  
}

void KBolometerRecord::InitializeMembers(void)
{
  //init local members
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  fDetectorName.resize(0);
  fMass = 0;
  
}

void KBolometerRecord::SetDetectorName(const Char_t* aWord)
{
  fDetectorName = aWord;
}

const Char_t* KBolometerRecord::GetDetectorName(void) const 
{
  return fDetectorName.c_str();
}


Bool_t KBolometerRecord::IsSame(const KBolometerRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.
  
  Bool_t bIsEqual = true; //assume its true, then test for differences
  
  
  if(fDetectorName != aRec.fDetectorName){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBolometerRecord fDetectorName Not Equal. lhs: " 
      << fDetectorName << " != rhs " << aRec.fDetectorName << endl;   
    else
      return false;  
  }
  if(fMass != aRec.fMass){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBolometerRecord fMass Not Equal. lhs: " 
      << fMass << " != rhs " << aRec.fMass << endl;   
    else
      return false;  
  }
    
  return bIsEqual;
}


void KBolometerRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes
  
}

Bool_t KBolometerRecord::IsDetector(const char* name)
{
  //returns true if name is equal to the string fDetectorName.
  //This method is quite useful when using the TTree::Draw methods 
  //and accessing data via the class methods instead of the variable names
  //this is faster because no strings are created and destroyed (no 
  //unnecessary memory allocation).
  
  //Use it like this:
  //
  //t->Draw("fQvalue:fEnergyRecoil","fBolo.IsDetector(\"ID3\")")
  //
  //
  
  if(name == 0) return false;
  
  if(fDetectorName.compare(name) == 0)
    return true;
  
  else return false;

}



