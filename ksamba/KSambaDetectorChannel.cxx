//______________________________________________________________________
//
// KSambaDetectorChannel.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaDetectorChannel.h"

ClassImp(KSambaDetectorChannel);

KSambaDetectorChannel::KSambaDetectorChannel(void)
{
  SetRecordWord("* Voie \"");
  InitializeMembers();
}

KSambaDetectorChannel::~KSambaDetectorChannel(void)
{
  
}

void KSambaDetectorChannel::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
