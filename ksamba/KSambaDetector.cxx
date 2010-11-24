//______________________________________________________________________
//
// KSambaDetector.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaDetector.h"

ClassImp(KSambaDetector);

KSambaDetector::KSambaDetector(void)
{
  SetRecordWord("* Detecteur ");
  InitializeMembers();
}

KSambaDetector::~KSambaDetector(void)
{

}

void KSambaDetector::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
