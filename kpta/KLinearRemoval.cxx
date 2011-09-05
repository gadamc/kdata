//______________________________________________________________________
//
// KLinearRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Substracts the baseline level from a pulse.
// Use SetBaselineStart and SetBaselineStop to set the region of the pulse
// that is used to calculate the baseline. The default values are 0% and 40%
// 


#include "KLinearRemoval.h"

//ClassImp(KLinearRemoval);

KLinearRemoval::KLinearRemoval(void)
{
  SetName("KLinearRemoval");
  InitializeMembers();
}

KLinearRemoval::KLinearRemoval(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KLinearRemoval"); 
  InitializeMembers();
}

KLinearRemoval::~KLinearRemoval(void)
{

}

void KLinearRemoval::InitializeMembers(void)
{
  //initialize members. 

  fBaselineStop = 0.40;
  fSlope = 0;
}

bool KLinearRemoval::RunProcess(void)
{
  //cout << "Run Process: " << GetName() << endl;
  double theAve = CalculateSlope();
  if(theAve > -99999)
    return Subtract(theAve);
  else {
    //fOutputPulse = fInputPulse;
    return false;
  }

}

double KLinearRemoval::CalculateSlope(void)
{
  //calculates the slope of the line from the start of the pulse
  //to the stop point, set by fBaselineStop. (The offset of the line
  //will always be the first element of the pulse.) The slope is calculated
  //by linear regression
  
  if(fInputSize < 1) return -99999;
  if(fBaselineStop < 0) return -99999;
  if(fBaselineStop > 1.0) return -99999;

  unsigned int size = fBaselineStop*fInputSize;
  double xy(0), y(0);
  
  for(unsigned int i = 0; i < size; i++){
    xy += *(fInputPulse+i) * i;
    y += *(fInputPulse+i);
  }
  
  fSlope = (xy - (size-1) * y/2.) / (size*size*size/12. + size*size - 13.*size/12.);  
  
  return fSlope;
}

void KLinearRemoval::Subtract(double aSlope, unsigned int i)
{
  //This subtracts fInputPulse a line calculated
  //as output_i = aSlope * i - offset 
  //where offset is the first element of fInputPulse
  
  *(fOutputPulse+i) -= aSlope * i - *(fInputPulse);
}

bool KLinearRemoval::Subtract(double aSlope)
{
  //This subtracts fInputPulse a line calculated
  //as output_i = aSlope * i - offset for all i in the pulse
  //where offset is the first element of fInputPulse

  for(unsigned int i = 0; i < fInputSize; i++)
    *(fOutputPulse+i) -= aSlope * i - *(fInputPulse);

  return true;
}
