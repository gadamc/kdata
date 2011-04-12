//______________________________________________________________________
//
// KSambaHeader.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaHeader.h"

ClassImp(KSambaHeader);

KSambaHeader::KSambaHeader(void)
{
  SetRecordWord("* Archive SAMBA");
  InitializeMembers();
}

KSambaHeader::~KSambaHeader(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KSambaHeader::Clear(Option_t* /* opt */)
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

void KSambaHeader::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
