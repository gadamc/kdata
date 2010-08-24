//______________________________________________________________________

// KPsaProcessor.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPsaProcessor.h"

//ClassImp(KPsaProcessor);

KPsaProcessor::KPsaProcessor(void)
{

  InitializeMembers();
}

KPsaProcessor::~KPsaProcessor(void)
{
  
}

void KPsaProcessor::InitializeMembers(void)
{
  fProcessorName = "";
}
