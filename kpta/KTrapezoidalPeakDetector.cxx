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

ClassImp(KTrapezoidalPeakDetector);

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
  vector<KTrapezoidalFilter* >::iterator it;
  for(it = fTrapFilters.begin(); it< fTrapFilters.end(); it++)
    if(*it)
      delete (*it);
  //debug
  if(fDerivative) delete [] fDerivative;
  if(fCorrelation) delete [] fCorrelation;
}


void KTrapezoidalPeakDetector::InitializeMembers(void)
{
  fDecayTimeConstant = 0.0;
  fOrderFilter1.SetOrder(1);
  fOrderFilter1.SetInitOutputValue(0.0);
  fOrderFilter2.SetOrder(1);
  fOrderFilter1.SetInitOutputValue(0.0);
  maxWidth = 0;
  fThreshold = 0.95;
  
}

void KTrapezoidalPeakDetector::AddTrapFilter(unsigned int aRiseTime, unsigned int aFlatTopWidth)
{
	KTrapezoidalFilter* n = new KTrapezoidalFilter();
  n->SetParams(fDecayTimeConstant, aRiseTime, aFlatTopWidth);
  if( maxWidth < (2*aRiseTime+aFlatTopWidth))
    maxWidth = 2*aRiseTime+aFlatTopWidth;
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
		return true;
	}
  
  //clear the output pulse
  memset(fOutputPulse, 0, fOutputSize*sizeof(double));
  
	vector<KTrapezoidalFilter* >::iterator it;
	
	unsigned int rise;
  unsigned int flat;
  fDerivative = new double[fOutputSize];
  fCorrelation = new double[fOutputSize];
  double *sqsumDerivative = new double[fOutputSize];
  double sqsumPattern;
  double denom = 0.0;
	for(it = fTrapFilters.begin(); it< fTrapFilters.end(); it++){
		rise = (*it)->GetRiseTime();
		flat = (*it)->GetFlatTopWidth();
		    
		(*it)->SetInputPulse(fInputPulse, fInputSize);
		if( !(*it)->RunProcess() ){
			cerr << "KTrapezoidalPeakDetector: trapezoidal filter failed (rise,flat) = ("
				<< rise << "," << flat << ")" << endl;
			// clean up
			if(sqsumDerivative) delete [] sqsumDerivative;
			return false;
		}
		
		// calculate the second derivative
		//double *fDerivative = new double[fOutputSize];
    // debug:
    
    
		memset(fDerivative, 0, fOutputSize*sizeof(double));
    memset(fCorrelation, 0, fOutputSize*sizeof(double));
    memset(sqsumDerivative, 0, fOutputSize*sizeof(double));
    
    
    
    fOrderFilter1.SetInputPulse((*it)->GetOutputPulse(),(*it)->GetOutputPulseSize());
    fOrderFilter1.RunProcess();
    fOrderFilter2.SetInputPulse(fOrderFilter1.GetOutputPulse(),fOrderFilter1.GetOutputPulseSize());
    fOrderFilter2.RunProcess();
    fDerivative = fOrderFilter2.GetOutputPulse();
		
    
// a faster second derivative calculation, commentend out, because it does not work properly yet		
// 		for(unsigned int n = 1; n < fOutputSize; n++){
// 			*(fDerivative + n) = *((*it)->GetOutputPulse() + n) - *((*it)->GetOutputPulse()+n-1);
// 			if( n > 2)
// 				*(fDerivative + n-2) = *(fDerivative + n-2) - *(fDerivative + n-3);
// 		}
// 		*(fDerivative + fOutputSize-2) = *(fDerivative + fOutputSize-2)-*(fDerivative + fOutputSize-3);
// 		*(fDerivative + fOutputSize-1) = *(fDerivative + fOutputSize-1)-*(fDerivative + fOutputSize-2);
// 		

		// calculate the correlation in the second derivative
		for(unsigned int n = maxWidth; n < fOutputSize-maxWidth; n++){
			// first peak in the pattern is the same for all filters, so it should contribute to the sum only once!
      sqsumPattern = 0.;
			if( it == fTrapFilters.begin() ){
				*(fCorrelation + n) += *(fDerivative + n)*1.0;
				*(sqsumDerivative + n) += *(fDerivative + n) * (*(fDerivative + n));
				sqsumPattern += 1.0;
			}
			if( flat == 0 ){
				*(fCorrelation + n) += *(fDerivative + n + rise)*(-2.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise) * (*(fDerivative + n + rise));
				sqsumPattern += 4.0;				
			}
			else{
				*(fCorrelation + n) += *(fDerivative + n + rise)*(-1.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise) * (*(fDerivative + n + rise));
				*(fCorrelation + n) += *(fDerivative + n + rise + flat)*(-1.0);
				*(sqsumDerivative + n) += *(fDerivative + n + rise + flat) * (*(fDerivative + n + rise +flat));
				sqsumPattern += 2.0;				
			}
			*(fCorrelation + n) += *(fDerivative + n + 2*rise + flat)*(1.0);
			*(sqsumDerivative + n) += *(fDerivative + n + 2*rise + flat) * (*(fDerivative + n + 2*rise +flat));
			sqsumPattern += 1.0;
		}
		
		denom = 0.0;
    //cout << " sqsumPattern: " << sqsumPattern << endl;
    for(unsigned int n = maxWidth; n < fOutputSize-maxWidth; n++){
      denom = sqrt(*(sqsumDerivative + n) * sqsumPattern);
      if( (denom > 0.05) && abs(*(fCorrelation+n))> 0.01){
        *(fCorrelation + n) /= denom;
        if( abs(*(fCorrelation + n)) > fThreshold)
          *(fOutputPulse + n) += *(fCorrelation + n);
      }
    }
		// clean up
    //debug
		//if(fDerivative) delete [] fDerivative;
	}
	
	
	//normalize the correlation coefficient
  //cout << " fTrapFilters.size(): "<< fTrapFilters.size() << endl;
	for(unsigned int n = maxWidth; n < fOutputSize-maxWidth; n++)
    *(fOutputPulse + n) /= fTrapFilters.size();
      
	
	// clean up
	if(sqsumDerivative) delete [] sqsumDerivative;
	
  return true;
}



