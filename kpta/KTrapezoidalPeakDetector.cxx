//______________________________________________________________________
//
// KTrapezoidalPeakDetector.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KTrapezoidalPeakDetector.h"
#include <cmath>
#include <iostream>

using namespace std;

//ClassImp(KTrapezoidalPeakDetector);

KTrapezoidalPeakDetector::KTrapezoidalPeakDetector(void)
{
  SetName("KTrapezoidalPeakDetector");
  InitializeMembers();
}

KTrapezoidalPeakDetector::KTrapezoidalPeakDetector(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
  : KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
   SetName("KTrapezoidalPeakDetector"); 
   InitializeMembers();
}

KTrapezoidalPeakDetector::~KTrapezoidalPeakDetector(void)
{
  //if(fTrapFilters) delete [] fTrapFilters;
}


void KTrapezoidalPeakDetector::InitializeMembers(void)
{
  fDecayTimeConstant = 0.0;
}

void KTrapezoidalPeakDetector::AddTrapFilter(unsigned int aRiseTime, unsigned int aFlatTopWidth)
{
	KTrapezoidalFilter* n = new KTrapezoidalFilter();
  n->SetParams(fDecayTimeConstant, aRiseTime, aFlatTopWidth);
	n->SetInputPulse(fInputPulse,fInputSize);
  fTrapFilters.push_back(n);
}

bool KTrapezoidalPeakDetector::RunProcess(void)
{
    
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "KTrapezoidalPeakDetector: input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "KTrapezoidalPeakDetector: input and output pulses must be of the same size. " << endl;
    return false;
  }
  if(fDecayTimeConstant <= 0.0){
    cerr << "KTrapezoidalPeakDetector: decay constant does not have a valid value. " <<endl;
		return false;
  }
  if(fTrapFilters.size()==0){
		cerr << "KTrapezoidalPeakDetector: no trapezoidal filters added to the detection chain. " <<endl;
		return false;
	}
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));
  
	vector<KTrapezoidalFilter* >::iterator it;
	double *sqsumDerivative = new double[fOutputSize];
	memset(sqsumDerivative, 0, fOutputSize*sizeof(double));
	double *sqsumPattern = new double[fOutputSize];
	memset(sqsumPattern, 0, fOutputSize*sizeof(double));
	
	for(it = fTrapFilters.begin(); it< fTrapFilters.end(); it++){
		unsigned int rise = (*it)->GetRiseTime();
		unsigned int flat = (*it)->GetFlatTopWidth();
		unsigned int width = (2 * rise + flat);
		
		if( !(*it)->RunProcess() ){
			cerr << "KTrapezoidalPeakDetector: trapezoidal filter failed (rise,flat) = ("
				<< rise << "," << flat << ")" << endl;
			// clean up
			if(sqsumDerivative) delete [] sqsumDerivative;
			if(sqsumPattern) delete [] sqsumPattern;
			return false;
		}
		
		// calculate the second derivative
		double *fDerivative = new double[fOutputSize];
		memset(fDerivative, 0, fOutputSize*sizeof(double));
		
		
		for(unsigned int n = 1; n < fOutputSize; n++){
			*(fDerivative + n) = *((*it)->GetOutputPulse() + n) - *((*it)->GetOutputPulse()+n-1);
			if( n > 2)
				*(fDerivative + n-2) = *(fDerivative + n-2) - *(fDerivative + n-3);
		}
		*(fDerivative + fOutputSize-2) = *(fDerivative + fOutputSize-2)-*(fDerivative + fOutputSize-3);
		*(fDerivative + fOutputSize-1) = *(fDerivative + fOutputSize-1)-*(fDerivative + fOutputSize-2);
		
		// computing the correlation in the second derivative
		
		
		for(unsigned int n = width; n < fOutputSize-width; n++){
			// first peak in the pattern is the same for all filters, so it should contribute to the sum only once!
			if( it == fTrapFilters.begin() ){
				*(fOutputPulse + n) += *(fDerivative + n)*1.0;
				*(sqsumDerivative + n) += *(fDerivative + n) * (*(fDerivative + n));
				*(sqsumPattern + n) += 1.0;
			}
			if( flat == 0 ){
				*(fOutputPulse + n) += *(fDerivative + n + rise)*(-2.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise) + (*(fDerivative + n + rise));
				*(sqsumPattern + n) += 4.0;				
			}
			else{
				*(fOutputPulse + n) += *(fDerivative + n + rise)*(-1.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise) + (*(fDerivative + n + rise));
				*(fOutputPulse + n) += *(fDerivative + n + rise + flat)*(-1.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise + flat) + (*(fDerivative + n + rise +flat));
				*(sqsumPattern + n) += 2.0;				
			}
			*(fOutputPulse + n) += *(fDerivative + n + 2*rise + flat)*(1.0);
			*(sqsumDerivative + n) += *(fDerivative + n + 2*rise + flat) + (*(fDerivative + n + 2*rise +flat));
			*(sqsumPattern + n) += 1.0;
		}
		
		// clean up
		if(fDerivative) delete [] fDerivative;
	}
	
	// normalize the correlation coefficient
	double denom = 0.0;
	for(unsigned int n = 0; n < fOutputSize; n++){
		denom = sqrt(*(sqsumDerivative + n) * (*(sqsumPattern + n)));
		if( denom != 0.0 )
			*(fOutputPulse + n) /= denom;
		
	}
	
	// clean up
	if(sqsumDerivative) delete [] sqsumDerivative;
	if(sqsumPattern) delete [] sqsumPattern;
	
  return true;
}



