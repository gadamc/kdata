//______________________________________________________________________
//
// KWindow.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// KWindow class can be used to scale a pulse with a desired window function.

#include "KWindow.h"
#include <cmath>
#include <iostream>
#include <string.h>

using namespace std;
//ClassImp(KWindow);

KWindow::KWindow(void)
{
  SetName("KWindow");
  InitializeMembers();
}

KWindow::KWindow(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
: KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KWindow"); 
  InitializeMembers();
}

KWindow::~KWindow(void)
{
  if(fCoef) delete [] fCoef;
}


void KWindow::SetCoefficients(const double *coef, unsigned int coefSize){
  if(coefSize == fInputSize){
    if(coefSize != fCoefSize){
      if(fCoef) delete fCoef;
      fCoefSize = coefSize;
      fCoef = new double[fCoefSize];
    }
    for(unsigned int i = 0; i < coefSize; i++){
      *(fCoef+i) = *(coef+i);
    }
    fCoefSize = fInputSize;
  }
  else
    cerr<<"the number of coefficients should be the same as the input size! nothing done"<<endl;
  
}

bool KWindow::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  
  if(fCoefSize == 0){
    cerr<<"window coefficients are not set"<<endl;
    return false;
  }
  //clear the output pulse
  memset(fOutputPulse, 0, fInputSize*sizeof(double));
  
  fOutputSize = fInputSize;
  
  for(unsigned int i = 0 ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  *(fInputPulse+i) * *(fCoef+i); 
  
  return true;
}



void KWindow::InitializeMembers(void)
{
  fCoef = 0;
  fCoefSize = 0;
  
}


