//______________________________________________________________________
//
// KPeakFinder.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// It is highly recommended to use the same memory for both the input and output pulses.
// This will save processing time. Otherwise, the input pulse values will be copied directly
// to the output pulse's memory location, which takes N CPU operations.
//
// The input and output pulses should be the same size. 
//

#include "KPeakFinder.h"
#include "KRootMeanSquare.h"
#include <cmath>
#include <iostream>

using namespace std;

KPeakFinder::KPeakFinder(void)
{
  SetName("KPeakFinder");
  InitializeMembers();
  fExtraWeakPeakPositions.reserve(1000);
  fWeakPeakPositions.reserve(1000);
  fStrictPeakPositions.reserve(1000);
}

KPeakFinder::KPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KPeakFinder"); 
   InitializeMembers();
   fExtraWeakPeakPositions.reserve(1000);
   fWeakPeakPositions.reserve(1000);
   fStrictPeakPositions.reserve(1000);
}

KPeakFinder::~KPeakFinder(void)
{
 
}

void KPeakFinder::InitializeMembers(void)
{
  fPolarity=false;
}

void KPeakFinder::EraseVectors(void)
{
  fExtraWeakPeakPositions.erase(fExtraWeakPeakPositions.begin(), fExtraWeakPeakPositions.end());
  fWeakPeakPositions.erase(fWeakPeakPositions.begin(), fWeakPeakPositions.end());
  fStrictPeakPositions.erase(fStrictPeakPositions.begin(), fStrictPeakPositions.end());
  
}
bool KPeakFinder::RunProcess(void)
{
  if(fInputPulse == 0) {
    cerr << "KPeakFinder::RunProcess. input and output arrays have not been set." << endl;
    return false;
  }
  
  if(fOutputPulse != fInputPulse){  //the output pulse pointer is not the same as the input pulse 
    //and, from the previous IF statement, we know fOutputPulse is not zero.
    int size = fOutputSize >= fInputSize ? fInputSize : fOutputSize;
    //of course, the input and output pulses SHOULD be the same size....
    memcpy(fOutputPulse, fInputPulse, size * sizeof(double));
  }   //if fOutputPulse == fInputPulse, then I assume that they are already the same pulse and
  //no memcpy operation is performed. 
  
  EraseVectors();
  KRootMeanSquare mrms;
  double rms = mrms.GetRms( fInputPulse, 0, fInputSize);
  
  double xweakThresh = fPolarity ? rms*fExtraWeakAmp : -rms*fExtraWeakAmp;
  double weakThresh = fPolarity ? rms*fWeakAmp : -rms*fWeakAmp;
  double strictThresh = fPolarity ? rms*fStrictAmp : -rms*fStrictAmp;
  
  
  if(fPolarity){
    for(unsigned int k = 0; k < fInputSize; k++){
      if (*(fInputPulse+k) >= xweakThresh) fExtraWeakPeakPositions.push_back(k);
      if (*(fInputPulse+k) >= weakThresh) fWeakPeakPositions.push_back(k);
      if (*(fInputPulse+k) >= strictThresh) fStrictPeakPositions.push_back(k);
    }
  }
  else {
    for(unsigned int k = 0; k < fInputSize; k++){
      if (*(fInputPulse+k) <= xweakThresh) fExtraWeakPeakPositions.push_back(k);
      if (*(fInputPulse+k) <= weakThresh) fWeakPeakPositions.push_back(k);
      if (*(fInputPulse+k) <= strictThresh) fStrictPeakPositions.push_back(k);

    }
  }
  
  return true;
  
}

void KPeakFinder::SetHeatDefault(void)
{
  fExtraWeakAmp = 5.5;
  fWeakAmp = 4.5;
  fStrictAmp = 3.5;
}

void KPeakFinder::SetIonDefault(void)
{
  fExtraWeakAmp = 9.;
  fWeakAmp = 7.;
  fStrictAmp = 5.;
}
