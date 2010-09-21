//______________________________________________________________________
//
// KPatternRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPatternRemoval.h"
#include <cmath>

//ClassImp(KPatternRemoval);

KPatternRemoval::KPatternRemoval(void)
{
	SetName("KPatternRemoval");
  InitializeMembers();
}

KPatternRemoval::~KPatternRemoval(void)
{

}

void KPatternRemoval::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fPatternLength = 100; //default pattern length size;
	fBaselineStart = 0;
	fBaselineStop = fPatternLength*20; //40% of a standard ion pulse of length 5000 pts
	fPattern.resize(fPatternLength, 0);
}

bool KPatternRemoval::RunProcess(void)
{
  //cout << "Run Process: " << GetName() << endl;
	if(CalculatePattern())
    return SubtractPattern();
  else {
    fOutputPulse = fInputPulse;
    return false;
  }
}

template<class T> bool KPatternRemoval::SetThisToPattern(vector<T> &aPattern)
{
	fPattern.resize(aPattern.size(), 0);
	
	try {
		for(unsigned int i = 0; i < fPattern.size(); i++){
			fPattern.at(i) = aPattern.at(i);
		}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "KPatternRemoval::SetThisToPattern. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
		fPattern.resize(0);  //reduce the size of the pattern. 
		return false;
	}
	
	
	return true;
}

bool KPatternRemoval::CalculatePattern(void)
{
	if(GetPatternLength() < 1) return false;
	
	fPattern.resize(GetPatternLength(), 0);
	
	// count the number of full patterns that can fit in the range fBaselineStart:fBaselineStop
	// and force the routine to only consider full patterns.
	unsigned int numPatterns = (unsigned int)floor((fBaselineStop - fBaselineStart + 0.5)/(double)(GetPatternLength()));
	
	if(numPatterns < 1) return false; 
	
	unsigned int patternCount = 0;
	unsigned int i;
	try {
		for( i = fBaselineStart; i < fBaselineStart + numPatterns*GetPatternLength() ; i++){
			if(patternCount == fPattern.size())
				patternCount = 0;
			
			fPattern.at(patternCount++) += fInputPulse.at(i)/numPatterns;
		}
	}
	catch (out_of_range& e) {
		cerr << "KPatternRemoval::CalculatePattern. exception caught: " << e.what() << " at input pulse position " << i << " and pattern position " << patternCount-1 << endl;
    cerr << "    InputPulse size: " << fInputPulse.size() << " Pattern size: " << fPattern.size() << endl;
		cerr << "    stopping calculation" << endl;
		return false;
		
	}
	
	return true;
}

bool KPatternRemoval::SubtractPattern(void)
{
	return SubtractPattern(fPattern);
}

bool KPatternRemoval::SubtractPattern(vector<double> &aPattern)
{
	
	fOutputPulse.resize(fInputPulse.size(),0);  //make them the same size.
	unsigned int patternCount = 0;
	
	try {
		for(unsigned int i = 0; i < fOutputPulse.size(); i++){
			if(patternCount == aPattern.size())
				patternCount = 0;
			
			fOutputPulse.at(i) = fInputPulse.at(i) - aPattern.at(patternCount++);
			
		}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "KPatternRemoval::SubtractPattern.  exception caught: " << e.what() << endl;
		cerr << "    stopping calculation";
		return false;
	}
	
	return true;
}

bool KPatternRemoval::SetPattern(vector<double> &aPattern)
{ 
	//Set the pattern and the pattern length;
	fPattern = aPattern; 
	return true;
} 

void KPatternRemoval::SetBaselineStart(unsigned int aVal)
{ 
	if(aVal > fBaselineStop || aVal > fInputPulse.size()){
		cerr << "KPatternRemoval::SetBaselineStart. Invalid value: " << aVal << endl;
		cerr << "   baseline start value unchanged: " << fBaselineStart << endl;
		cerr << "   baseline stop value is : " << fBaselineStop	<< endl;
		cerr << "   pulse size is : " << fInputPulse.size() << endl;
	}
	else fBaselineStart = aVal;

}

void KPatternRemoval::SetBaselineStop(unsigned int aVal)
{ 
	if(aVal < fBaselineStart  || aVal > fInputPulse.size()){
		cerr << "KPatternRemoval::SetBaselineStop. Invalid value: " << aVal << endl;
		cerr << "   baseline stop value unchanged: " << fBaselineStop << endl;
		cerr << "   baseline start value is : " << fBaselineStart	<< endl;
		cerr << "   pulse size is : " << fInputPulse.size() << endl;
	}
	else fBaselineStop = aVal;
}
