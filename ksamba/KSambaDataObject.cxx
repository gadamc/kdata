//______________________________________________________________________
//
// KSambaDataObject.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaDataObject.h"

ClassImp(KSambaDataObject);

KSambaDataObject::KSambaDataObject(void)
{

  InitializeMembers();
}

KSambaDataObject::~KSambaDataObject(void)
{

}

void KSambaDataObject::InitializeMembers(void)
{
  fRecordWord = "";
}
