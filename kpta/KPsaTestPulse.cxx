//______________________________________________________________________
//
// KPsaTestPulse.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPsaTestPulse.h"
#include <cmath>
#include <iostream>
#include <stdexcept> 
#include <cstdlib>
#include <ctime>


KPsaTestPulse::KPsaTestPulse(void)
{

  InitializeMembers();
}

KPsaTestPulse::~KPsaTestPulse(void)
{

}

void KPsaTestPulse::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fAmp = 100;
	fPeriod = 100;
	fPulse.resize(5000,0);
}

void KPsaTestPulse::CreateSineWave(void)
{
	try{
		for (unsigned int i = 0; i < fPulse.size(); i++) {
			fPulse.at(i) = fAmp*sin(2*3.1415926*i/fPeriod);
		}
	}
	catch (out_of_range &e) {
		cerr << "KPsaTestPulse::CreateSineWave. Exception caught: " << e.what() << endl;
	}
	
}

void KPsaTestPulse::CreateSquareWave(void)
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
		cerr << "KPsaTestPulse::CreateSquareWave. Exception caught: " << e.what() << endl;
	}
}

void KPsaTestPulse::CreateWhiteNoise(void)
{
	srand( time(NULL));
	
	try{
		for (unsigned int i = 0; i < fPulse.size(); i++) {
			fPulse.at(i) = rand() % (fAmp+1);
		}
	}
	catch (out_of_range &e) {
		cerr << "KPsaTestPulse::CreateWhiteNoise. Exception caught: " << e.what() << endl;
	}
}
