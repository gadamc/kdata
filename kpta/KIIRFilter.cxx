//______________________________________________________________________
//
// KIIRFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
//
//
//

#include "KIIRFilter.h"
#include <cmath>

//ClassImp(KIIRFilter);

KIIRFilter::KIIRFilter(void)
{
  
  InitializeMembers();
}

KIIRFilter::~KIIRFilter(void)
{
  
}

void KIIRFilter::InitializeMembers(void)
{
  //no local members to initialize
  

}

bool KIIRFilter::RunProcess(void)
{
	/*
	if(fInputPulse.size() < 2) return false;
	 
	 unsigned int N = fInputPulse.size();
	  if (fOutputPulse.size() != 1 + (N/2))
	    fOutputPulse.resize(1+(N/2),0);
	 
	 try{
	   fOutputPulse.at(0) = pow(fInputPulse.at(0),2);
	   fOutputPulse.at(N/2) = pow(fInputPulse.at(N/2),2);
	   
	   for (unsigned int k=1; k<N/2; k++) 
	     fOutputPulse.at(k) = pow(fInputPulse.at(k),2) + pow(fInputPulse.at(N-k),2);
	   
	   return true;
	   
	 }
	 catch (out_of_range &e) {
	   cerr << "KIIRFilter::RunProcess. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
	    
	   return false;
	 }*/
  return true;
  
}

