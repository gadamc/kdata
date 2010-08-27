//______________________________________________________________________
//
// KBaselineRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
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
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fBaselineStart = 0.0;
	fBaselineStop = 0.40;
}

bool KBaselineRemoval::RunProcess(void)
{
	double theAve = CalculateAverage();
	if(theAve > -90000)
		return Subtract(theAve);
	else return false;

}

double KBaselineRemoval::CalculateAverage(void) const
{
	
	if(fInputPulse.size() < 1) return -1;
	if(fBaselineStart * fBaselineStop < 0) return -1;
	if(fBaselineStart > 1.0 || fBaselineStop > 1.0) return -1;
	if(fBaselineStart > fBaselineStop) return -1;
	
	double theAve = 0;
	int numBins = 0;
	
	
	try {
		for(unsigned int i = fBaselineStart*fInputPulse.size(); i < fBaselineStop*fInputPulse.size(); i++){
			theAve += fInputPulse.at(i);
			numBins++;
		}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "KBaselineRemoval::CalculateAverage. exception caught: " << e.what() << endl;
		cerr << "   stoping calculation" << endl;
		theAve = -1;
	}
	
	if(theAve > 0 && numBins > 0) return theAve/numBins;
	else return -90000;
}

bool KBaselineRemoval::Subtract(double aVal)
{
	//This subtracts aVal from each element of fInputPulse and sets it 
	//equal to the same element in fOutputPulse
	fOutputPulse.resize(fInputPulse.size());  //make them the same size.
	
	try {
			for(unsigned int i = 0; i < fInputPulse.size(); i++){
				fOutputPulse.at(i) = fInputPulse.at(i) - aVal;
			}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "KBaselineRemoval::Subtract. exception caught: " << e.what() << endl;
		cerr << "    stopping calculation";
		return false;
	}
	
	return true;
}
