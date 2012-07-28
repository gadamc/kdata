//______________________________________________________________________
//
// KNullProcessor.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2012 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// A KPtaProcessor object that makes no transformation to the signal.
//
// This class may be useful if a KAmpSite or KAmper object requires that you set 
// a PtaProcessor, but in your particular use case, you don't want it to do anything.
// This NULL processor, when RunProcess is called, sets the input Pulse pointer to the outputPulse pointer
// If the outputPulse Pointer is already set to another memory location, THEN THAT MEMORY IS DELETED.
// So, be a little bit careful when using this. 
// 

#include <iostream>
#include "KNullProcessor.h"

ClassImp(KNullProcessor);

KNullProcessor::KNullProcessor(void)
{
  SetName("KNullProcessor");
}


KNullProcessor::~KNullProcessor(void)
{
  fOutputPulse = 0;
  fOutputSize = 0; 
  //the KPtaProcessor base class takes care of the real cleanup. So, I just set the output pulse pointer to 0
}

void KNullProcessor::InitializeMembers(void)
{

}

bool KNullProcessor::RunProcess(void)
{
  return true;

}

void KNullProcessor::AllocateArrays(unsigned int size)
{
  if(fInputPulse) delete [] fInputPulse;
  fInputPulse = new double[ size ];
  fOutputPulse = fInputPulse;
  fInputSize = fOutputSize = size;
}


