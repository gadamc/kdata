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
#include <cstring>

using namespace std;

ClassImp(KPatternRemoval);

KPatternRemoval::KPatternRemoval(void)
{
	SetName("KPatternRemoval");
  InitializeMembers();
}

KPatternRemoval::~KPatternRemoval(void)
{
  if(fPattern){
    delete [] fPattern;
    fPattern = 0;
    fPatternLength = 0;
  }
}

void KPatternRemoval::InitializeMembers(void)
{
  //sets the initial values
  //default pattern length = 100 points
  //default baseline start is point 0
  //default baseline stop is 20*pattern length (2000 points)
  

	fPatternLength = 100; //default pattern length size;
	fBaselineStart = 0;
	fBaselineStop = fPatternLength*20; //40% of a standard ion pulse of length 5000 pts
	fPattern = new double[fPatternLength];
  fUseExternalPattern = false;
}

bool KPatternRemoval::RunProcess(void)
{
  //Calls CalculatePattern and then, if successful, subtracts that 
  //pattern from the input pulse and assigns the result to the output pulse.
  //If fUseExternalPattern is true, then it will not call CalculatePattern, but instead
  //use whatever pattern is already stored.
  //This means, the following are equivalent.
  // SetPattern(aPattern);
  // RunProcess();
  //
  // and
  //
  // RunProcess(aPattern);
  //
  
  //cout << "Run Process: " << GetName() << endl;

  if (!fUseExternalPattern) 
    return SubtractPattern();
    
  else return (CalculatePattern() ? SubtractPattern() : false);
 /* 
	if(CalculatePattern())
    return SubtractPattern();
  else 
    return false;
  */
  
}

template<class T> bool KPatternRemoval::RunProcess(std::vector<T> &aPattern)
{
  //Subtracts aPattern from the input pulse and assigns the result to the output pulse.
  
  return (SetPattern(aPattern) ? SubtractPattern() : false);
  
  /*
  if(SetPattern(aPattern))
    return SubtractPattern();
  else
    return false;
  */
}


template<class T> bool KPatternRemoval::SetPattern(std::vector<T> &aPattern)
{
  //sets the pattern and sets fUseExternalPattern to true if
  //the memory for the pattern is properly allocated.
  //otherwise, fUseExternalPattern is not changed. 
  //
  //If this methods returns true, then
  //this means, the following are equivalent.
  // SetPattern(aPattern);
  // RunProcess();
  //
  // and
  //
  // RunProcess(aPattern);
  //
  
	if(SetPatternLength(aPattern.size()))
    return false;
	
	for(unsigned int i = 0; i < fPatternLength; i++){
			*(fPattern+i) = aPattern.at(i);
	}
  fUseExternalPattern;
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
  
  if(fPattern == 0){
    cerr << "Pattern pointer not allocated" << endl;
    return false;
  }
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  memset(fPattern, 0, fPatternLength);
	
	// count the number of full patterns that can fit in the range fBaselineStart:fBaselineStop
	// and force the routine to only consider full patterns.
	unsigned int numPatterns = (unsigned int)floor((fBaselineStop - fBaselineStart + 0.5)/(double)(fPatternLength));
	
	if(numPatterns < 1) return false; 
	
	unsigned int patternCount = 0;
  unsigned int maxPosition = fBaselineStart + numPatterns*fPatternLength ;

	for(unsigned int i = fBaselineStart; i < maxPosition; i++){
		if(patternCount == fPatternLength)
			patternCount = 0;
		
		*(fPattern + patternCount++) += *(fInputPulse+i)/numPatterns;
	}
	
	return true;
}

bool KPatternRemoval::SubtractPattern(void)
{
  //This subtracts the calculated fPattern from the fInputPulse and assigns the
  //result to the fOutputPulse
  //
  //If fPatternLength is set to zero, the input pulse is copied to the output pulse.
  //This could be useful when dealing with heat pulses (where there should be no pattern)
  //but this processor is part of a KPulseAnalysisChain and you just want to set 
  
  if (fPatternLength == 0) {
     memcpy(fOutputPulse,fInputPulse, (fOutputSize >= fInputSize) ? fInputSize : fOutputSize );
     return true;  
   } //return true in case pattern size is zero. 
     //this could be the case when dealing with 
     //heat pulses, but one wants to keep using the
     //same pulse chain processor, but sets the pattern length 
     //to zero.
	
	if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fPattern == 0) {
    cerr << "fPattern is not allocated." << endl;
    return false;
  }
  
  unsigned int patternCount = 0;
	for(unsigned int i = 0; i < fOutputSize; i++){
		if(patternCount == fPatternLength)
			patternCount = 0;
		
		*(fOutputPulse+i) = *(fInputPulse+i) - *(fPattern + patternCount++);
		
	}

	return true;
}



void KPatternRemoval::SetBaselineStart(unsigned int aVal)
{ 
  //set's the absolute start point for defining where the average
  //pattern is calculated.
  
	if(aVal > fBaselineStop || aVal > fInputSize){
		cerr << "KPatternRemoval::SetBaselineStart. Out of range: " << aVal << endl;
		cerr << "   baseline start value unchanged: " << fBaselineStart << endl;
		cerr << "   baseline stop value is : " << fBaselineStop	<< endl;
		cerr << "   pulse size is : " << fInputSize << endl;
	}
	else fBaselineStart = aVal;

}

void KPatternRemoval::SetBaselineStop(unsigned int aVal)
{ 
  //set's the absolute stop point for defining where the average
  //pattern is calculated.
  
	if(aVal < fBaselineStart  || aVal > fInputSize){
		cerr << "KPatternRemoval::SetBaselineStop. Out of range: " << aVal << endl;
		cerr << "   baseline stop value unchanged: " << fBaselineStop << endl;
		cerr << "   baseline start value is : " << fBaselineStart	<< endl;
		cerr << "   pulse size is : " << fInputSize << endl;
	}
	else fBaselineStop = aVal;
}

bool KPatternRemoval::SetPatternLength(unsigned int aLength)
{ 
  //sets the length of the pattern. This allocates memory and 
  //destroys the previous pattern if aLength is different from the 
  //current pattern length. 
  //This returns false if aLength is larger than the input pulse size.
  //
  
  if(aLength < fInputSize)
    return false;
  
  if(aLength != fPatternLength){
    if(fPattern) {
      delete [] fPattern;
      fPattern = 0;
    }
    if(aLength > 0) fPattern = new double[aLength];
    fPatternLength = aLength;
  }
  return true;
}
