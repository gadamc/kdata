//_____________________________________________
//
// EdwHLASubRecordBase.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// Base class for all HLA Sub Records. Currently empty, but may be useful
// in the future or if some analysis needs polymorphic behavior.
//

#include "EdwHLASubRecordBase.h"

ClassImp(EdwHLASubRecordBase);

EdwHLASubRecordBase::EdwHLASubRecordBase(void)
{

  InitializeMembers();
}

EdwHLASubRecordBase::~EdwHLASubRecordBase(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void EdwHLASubRecordBase::Clear(Option_t* /* opt */)
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

void EdwHLASubRecordBase::InitializeMembers(void)
{
	//Init local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
