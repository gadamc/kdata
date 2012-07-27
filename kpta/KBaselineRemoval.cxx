//______________________________________________________________________
//
// KBaselineRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Substracts the baseline level from a pulse.
// Use SetBaselineStart and SetBaselineStop to set the region of the pulse
// that is used to calculate the baseline. The default values are 0% and 40%
// 

#include <iostream>
#include "KBaselineRemoval.h"

ClassImp(KBaselineRemoval);

KBaselineRemoval::KBaselineRemoval(void)
{
  SetName("KBaselineRemoval");
  InitializeMembers();
}

KBaselineRemoval::KBaselineRemoval(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KBaselineRemoval"); 
  InitializeMembers();
}

KBaselineRemoval::~KBaselineRemoval(void)
{

}

void KBaselineRemoval::InitializeMembers(void)
{
  //initialize members. The baseline start and stop values are initially
  // set to 0.0 and 0.40, respectively.

  fBaselineStart = 0.0;
  fBaselineStop = 0.40;
}

bool KBaselineRemoval::RunProcess(void)
{
  //cout << "Run Process: " << GetName() << endl;
  if(CalculateAverage())
    return Subtract(fBaseline);
  else {
    //fOutputPulse = fInputPulse;
    return false;
  }

}

bool KBaselineRemoval::CalculateAverage(void)
{

  fBaseline = 0;

  if(fInputSize < 1) 
    {std::cerr << "KBaselineRemoval returning false. fInputSize: " << fInputSize << std::endl; return false;}
  if(fBaselineStart < 0 || fBaselineStop < 0) 
    {std::cerr << "KBaselineRemoval returning false. fBaselineStart/fBaselineStop: " << fBaselineStart << "/" <<  fBaselineStop << std::endl; return false;}
  if(fBaselineStart > 1.0 || fBaselineStop > 1.0) 
    {std::cerr << "KBaselineRemoval returning false. fBaselineStart/fBaselineStop: " << fBaselineStart << "/" <<  fBaselineStop << std::endl; return false;}
  if(fBaselineStart >= fBaselineStop)
    {std::cerr << "KBaselineRemoval returning false. fBaselineStart/fBaselineStop: " << fBaselineStart << "/" <<  fBaselineStop << std::endl; return false;}
 
  double stop = fBaselineStop*fInputSize;
  unsigned int start = fBaselineStart*fInputSize;
  unsigned int i = start;
  for( ; i < stop; i++){
    fBaseline += *(fInputPulse+i);
  }
  
  fBaseline = fBaseline/double(i-start);
  return true;
  
  return true;
}

void KBaselineRemoval::Subtract(double aVal, unsigned int i)
{
  *(fOutputPulse+i) = *(fInputPulse+i) - aVal;
}

bool KBaselineRemoval::Subtract(double aVal)
{
  //This subtracts aVal from each element of fInputPulse and sets it 
  //equal to the same element in fOutputPulse	

  for(unsigned int i = 0; i < fInputSize; i++)
    *(fOutputPulse+i) = *(fInputPulse+i) - aVal;

  return true;
}
