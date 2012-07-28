//______________________________________________________________________
//
// KPtaTestPulse.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This is a test-pulse class that generates a few standard pulse
// shapes that one can use to pass through Pulse Trace Analysis processors
// to test the processors. For example, creating a sine wave with this class
// and then pass this through a power spectrum processor, should result
// in a peak at a particular frequency (plus some spread due to the finite
// length of the pulse).
//
// Note: I don't think the square wave code is working. All are welcome
// to fix it, of course!


#include "KPtaTestPulse.h"
#include <cmath>
#include <iostream>
#include <stdexcept> 
#include <cstdlib>
#include <ctime>

using namespace std;

ClassImp(KPtaTestPulse);


KPtaTestPulse::KPtaTestPulse(void)
{

  InitializeMembers();
}

KPtaTestPulse::~KPtaTestPulse(void)
{

}

void KPtaTestPulse::InitializeMembers(void)
{
  
  
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
