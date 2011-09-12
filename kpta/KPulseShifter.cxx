//______________________________________________________________________
//
// KPulseShifter.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// KPulseShifter class can be used to cyclically shift a pulse to left (negative shift value) or to the right (positive shift value)

#include "KPulseShifter.h"
#include <cmath>
#include <iostream>
#include <string.h>

using namespace std;
//ClassImp(KPulseShifter);

KPulseShifter::KPulseShifter(void)
{
  SetName("KPulseShifter");
  InitializeMembers();
}

KPulseShifter::KPulseShifter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
: KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KPulseShifter"); 
  InitializeMembers();
}

KPulseShifter::~KPulseShifter(void)
{
}


bool KPulseShifter::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. nothing done. " << endl;
    return false;
  }
   
  for(unsigned int i = 0 ; i < fOutputSize; i++)
    if((i - fPulseShift)>=0)
			*(fOutputPulse+i) = *(fInputPulse+(i-fPulseShift)%fOutputSize);
		else
			*(fOutputPulse+i) = *(fInputPulse+(i-fPulseShift)%fOutputSize+fOutputSize);
  
  return true;
}



void KPulseShifter::InitializeMembers(void)
{
  fPulseShift = 0;
   
}


