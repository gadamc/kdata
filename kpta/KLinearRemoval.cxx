//______________________________________________________________________
//
// KLinearRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Substracts the baseline level from a pulse.
// Use SetBaselineStart and SetBaselineStop to set the region of the pulse
// that is used to calculate the baseline. This assumes you are calculating the line from
// the beginning to the pulse up to some percentage of the full pulse length. By default,
// the line is calculated up to 40% of the full pulse. This means this object can be used
// for ion and heat pulses without adjusting any parameters. Of course, since the ion pulses
// are located closer to the end of the pulse, the fBaselineStop value could be set to a higher value
// if desired.
// 


#include "KLinearRemoval.h"
#include <iostream>

using namespace std;
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
  fOffset = 0;
}

bool KLinearRemoval::RunProcess(void)
{
  //cout << "Run Process: " << GetName() << endl;

  return CalculateLine() ? Subtract() : false;
  
  /*
  if(CalculateLine())
    return Subtract();
  else {
    //fOutputPulse = fInputPulse;
    return false;
  }*/

}

bool KLinearRemoval::CalculateLine(void)
{
  //calculates the slope of the line from the start of the pulse
  //to the stop point, set by fBaselineStop. (The offset of the line
  //will always be the first element of the pulse.) The slope is calculated
  //by linear regression.
  
  if(fInputSize < 1) return false;
  if(fBaselineStop < 0) return false;
  if(fBaselineStop > 1.0) return false;

  unsigned int size = (fBaselineStop*fInputSize);
  double xy(0), y(0);
  //double x(0), xx(0);
  for(unsigned int i = 0; i< size; i++){
    xy += *(fInputPulse+i) * i;
    y += *(fInputPulse+i);
    //x += i;
    //xx += i*i;
  }
  //standard calculation
  //fSlope = (size*xy - x*y) / (size*xx - x*x);
  //fOffset = (xx*y - x*xy) / (size*xx - x*x);
  
  //fast calculation based upon known series calculations
  double Sii = ((double)size*(double)size*(double)size/3.) - ((double)size*(double)size/2.) + ((double)size/6.);
  double Si = size*(size - 1) / 2.;
  double denom = size*Sii - (Si*Si);
  fSlope =  (size*xy - Si * y) / denom;
  fOffset =  ( Sii*y - Si*xy ) / denom;
  
  return true;
}

void KLinearRemoval::Subtract(double aSlope, double aOffset, unsigned int i)
{
  //This subtracts fInputPulse a line calculated
  //as output_i = aSlope * i - offset 
  //where offset is the first element of fInputPulse
  
  *(fOutputPulse+i) = *(fInputPulse) - (aSlope * i + aOffset);
}

bool KLinearRemoval::Subtract(void)
{
  //This subtracts fInputPulse a line calculated
  //as output_i = aSlope * i - offset for all i in the pulse
  //where offset is the first element of fInputPulse

  for(unsigned int i = 0; i < fInputSize; i++)
    *(fOutputPulse+i) = *(fInputPulse+i) - (fSlope * i + fOffset);

  return true;
}
