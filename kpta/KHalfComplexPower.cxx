//______________________________________________________________________
//
// KHalfComplexPower.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Input pulse is assumed to be of the half-complex form
//   (r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1).
// Calculates rk*rk + ik*ik, for each k, with i0 = 0 and iN/2 = 0.
// The output "pulse" is this power spectrum and N/2 + 1 values long. 
//
//
//

#include "KHalfComplexPower.h"
#include <cmath>

KHalfComplexPower::KHalfComplexPower(void)
{

  InitializeMembers();
}

KHalfComplexPower::~KHalfComplexPower(void)
{

}

void KHalfComplexPower::InitializeMembers(void)
{
  //no local members to initialize
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

bool KHalfComplexPower::RunProcess(void)
{
	if(fInputPulse.size() < 2) return false;
	
	unsigned int N = fInputPulse.size();
  fOutputPulse.resize(1+(N/2),0);
	
	try{
		fOutputPulse.at(0) = pow(fInputPulse.at(0),2);
		fOutputPulse.at(N/2) = pow(fInputPulse.at(N/2),2);
		
		for (unsigned int k=1; k<N/2; k++) 
			fOutputPulse.at(k) = pow(fInputPulse.at(k),2) + pow(fInputPulse.at(N-k),2);
		
		return true;
		
	}
	catch (out_of_range &e) {
		cerr << "KHalfComplexPower::RunProcess. exception caught: " << e.what() << " ending the copy of the pulse." << endl;

		return false;
	}

}
