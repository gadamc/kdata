//_____________________________________________
//
// KSubRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Sub Records. 
//

#include "KSubRecord.h"
#include <iostream>

using namespace std;

ClassImp(KSubRecord);

KSubRecord::KSubRecord(void)
{

  InitializeMembers();

}

KSubRecord::~KSubRecord(void)
{
	//Does calling clear at destruction take too much computing time?
  Clear("C");
}

void KSubRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
	//members. Its necessary for this Clear method to exist
	//in the case that instances of this object are stored
	//inside of a TClonesArray
	TObject::Clear(opt);
	
	//Clear and delete local objects here. 
	
	//Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();
}

void KSubRecord::InitializeMembers(void)
{
  
}

