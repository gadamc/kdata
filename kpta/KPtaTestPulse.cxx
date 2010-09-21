//______________________________________________________________________
//
// KPtaTestPulse.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPtaTestPulse.h"
#include <cmath>
#include <iostream>
#include <stdexcept> 
#include <cstdlib>
#include <ctime>


KPtaTestPulse::KPtaTestPulse(void)
{

  InitializeMembers();
}

KPtaTestPulse::~KPtaTestPulse(void)
{

}

void KPtaTestPulse::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fAmp = 100;
	fPeriod = 100;
	fPulse.resize(5000,0);
}

void KPtaTestPulse::CreateSineWave(void)
{
	try{
		for (unsigned int i = 0; i < fPulse.size(); i++) {
			fPulse.at(i) = fAmp*sin(2*3.1415926*i/fPeriod);
		}
	}
	catch (out_of_range &e) {
		cerr << "KPtaTestPulse::CreateSineWave. Exception caught: " << e.what() << endl;
	}
	
}

void KPtaTestPulse::CreateSquareWave(void)
{
	try{
		for (unsigned int i = 0; i < fPulse.size(); i++) {
			int a = 1 - 2*((i*fPeriod) % 2);
			if(a >= 0)
				fPulse.at(i) = fAmp;
			else 
				fPulse.at(i) = -fAmp;
		}
	}
	catch (out_of_range &e) {
		cerr << "KPtaTestPulse::CreateSquareWave. Exception caught: " << e.what() << endl;
	}
}

void KPtaTestPulse::CreateWhiteNoise(void)
{
	srand( time(NULL));
	
	try{
		for (unsigned int i = 0; i < fPulse.size(); i++) {
			fPulse.at(i) = rand() % (fAmp+1);
		}
	}
	catch (out_of_range &e) {
		cerr << "KPtaTestPulse::CreateWhiteNoise. Exception caught: " << e.what() << endl;
	}
}
