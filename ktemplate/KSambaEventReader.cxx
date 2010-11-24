//______________________________________________________________________
//
// KSambaEventReader.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//

#include "KSambaEventReader.h"
#include "KSambaEvent.h"

ClassImp(KSambaEventReader);


KSambaEventReader::KSambaEventReader(void)
{
  
  InitializeMembers();
}

KSambaEventReader::~KSambaEventReader(void)
{
  
}

void KSambaEventReader::InitializeMembers(void)
{
  
  
}


Bool_t KSambaEventReader::ReadEvent(ifstream& aSambaFile, KSambaEvent* aheader)
{
  return true;
}