//______________________________________________________________________
//
// KRawBoloPulseRecord.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Holds the Raw-level pulse information, including the pulse trace
// This class is based on KBoloPulseRecord (which stores the channel
// number and type). 
//


#include "KRawBoloPulseRecord.h"

ClassImp(KRawBoloPulseRecord);

KRawBoloPulseRecord::KRawBoloPulseRecord(void)
{
  //standard constructor
  
  InitializeMembers();
}

KRawBoloPulseRecord::~KRawBoloPulseRecord(void)
{
  //destructor
  
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KRawBoloPulseRecord::Clear(Option_t* /* opt */)
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

void KRawBoloPulseRecord::InitializeMembers(void)
{
  //init local members.
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
