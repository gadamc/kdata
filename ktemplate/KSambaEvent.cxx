//______________________________________________________________________
//
// KSambaEvent.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaEvent.h"

ClassImp(KSambaEvent);


KSambaEvent::KSambaEvent(void)
{
  SetRecordWord("* Evenement ");
  InitializeMembers();
}

KSambaEvent::~KSambaEvent(void)
{

}

void KSambaEvent::InitializeMembers(void)
{
  
}
