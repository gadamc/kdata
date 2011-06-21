//______________________________________________________________________
//
// KPatternRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This class subtracts an average "pattern" from a waveform. 
// The pattern is calcuated from a particular region of waveform.
// You can set this region via the SetPatternStart and SetPatternStop


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
  //sets the initial values
  //default pattern length = 100 points
  //default baseline start is point 0
  //default baseline stop is 20*pattern length (2000 points)
  
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
  //This calculates the average pattern (of length fPatternLength) 
  //as found along the baseline between the fBaselineStart and fBaselineStop values. 
  //The average pattern is saved locally in the fPattern.
  //If the range between fBaselineStart and fBaselineStop is smaller than the fPatternLength
  //then this returns false. Otherwise, it calculates the number of full
  //patterns available in the range (numPatterns) and then calculates the average pattern
  //between fBaselineStart and fBaselineStart+numPatterns*fPatternLength. 
  
	if(GetPatternLength() <= 0){
    fPattern.resize(fPatternLength, 0);
    return true;
  }
	
  fPattern.clear();
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
  //This subtracts the calculated fPattern from the fInputPulse and assigns the
  //result to the fOutputPulse
  
	return SubtractPattern(fPattern);
}

bool KPatternRemoval::SubtractPattern(vector<double> &aPattern)
{
	//This subtracts aPattern from the fInputPulse and assigns the
  //result to the fOutputPulse. This does not alter the pattern stored
  //in fPattern, or the value of fPatternLength
  
	fOutputPulse.resize(fInputPulse.size(),0);  //make them the same size.
	unsigned int patternCount = 0;
	
  if (aPattern.size() == 0) {
    fOutputPulse = fInputPulse;
    return true;  
  } //return true in case pattern size is zero. 
    //this could be the case when dealing with 
    //heat pulses, but one wants to keep using the
    //same pulse chain processor, but sets the pattern length 
    //to zero.
  
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
  //You can insert your own pattern here. 
  
	fPattern = aPattern; 
	return true;
} 

void KPatternRemoval::SetBaselineStart(unsigned int aVal)
{ 
  //set's the absolute start point for defining where the average
  //pattern is calculated.
  
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
  //set's the absolute stop point for defining where the average
  //pattern is calculated.
  
	if(aVal < fBaselineStart  || aVal > fInputPulse.size()){
		cerr << "KPatternRemoval::SetBaselineStop. Invalid value: " << aVal << endl;
		cerr << "   baseline stop value unchanged: " << fBaselineStop << endl;
		cerr << "   baseline start value is : " << fBaselineStart	<< endl;
		cerr << "   pulse size is : " << fInputPulse.size() << endl;
	}
	else fBaselineStop = aVal;
}
