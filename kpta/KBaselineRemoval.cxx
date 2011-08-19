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


#include "KBaselineRemoval.h"

//ClassImp(KBaselineRemoval);

KBaselineRemoval::KBaselineRemoval(void)
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
	double theAve = CalculateAverage();
	if(theAve > -99999)
		return Subtract(theAve);
	else {
    fOutputPulse = fInputPulse;
    return false;
  }

}

double KBaselineRemoval::CalculateAverage(void) const
{
	
	if(fInputSize < 1) return -99999;
	if(fBaselineStart * fBaselineStop < 0) return -99999;
	if(fBaselineStart > 1.0 || fBaselineStop > 1.0) return -99999;
	if(fBaselineStart > fBaselineStop) return -99999;
	
	double theAve = 0;
	int numBins = 0;
	
	for(unsigned int i = fBaselineStart*fInputSize; i < fBaselineStop*fInputSize; i++){
		theAve += *(fInputPulse+i);
		numBins++;
	}
  if(numBins > 0) return theAve/numBins;
	else return -99999;
}

bool KBaselineRemoval::Subtract(double aVal)
{
	//This subtracts aVal from each element of fInputPulse and sets it 
	//equal to the same element in fOutputPulse	

  for(unsigned int i = 0; i < fInputSize; i++)
    *(fOutputPulse+i) = *(fInputPulse+i) - aVal;
			
	return true;
}
