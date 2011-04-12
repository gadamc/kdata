//______________________________________________________________________
//
// KSambaHeaderReader.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaHeaderReader.h"

ClassImp(KSambaHeaderReader);


KSambaHeaderReader::KSambaHeaderReader(void)
{
  
  InitializeMembers();
}

/*KSambaHeaderReader::KSambaHeaderReader(ifstream& aSambaFile, KSambaHeader* aheader = 0)
{
  InitializeMembers();
  ReadHeader(aSambaFile, aheader);
}
*/


KSambaHeaderReader::~KSambaHeaderReader(void)
{
  
}

void KSambaHeaderReader::InitializeMembers(void)
{
  
  
}

Bool_t KSambaHeaderReader::ReadHeader(ifstream& aSambaFile, KSambaHeader* aheader)
{
  return true;
}
