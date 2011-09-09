//______________________________________________________________________
//
// KWindow.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// KWindow class can be used to scale a pulse with a desired window function (at the moment only the Tukey window is implemented)
// To use it you should call the SetTukeyWindow(double alpha) function before processing, the alpha parameter defines the shape
// of the window: alpha = 0 is a rectangular window (pulse remains unchanged), alpha = 1 is a Hann window

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

void KWindow::SetTukeyWindow(double alpha)
{
  if(fInputSize != fCoefSize){
    if(fCoef) delete [] fCoef;
    fCoefSize = fInputSize;
    fCoef = new double[fCoefSize];
  }
  if(fCoefSize != 0){
    double  temp = alpha*(fCoefSize-1)/2.0;
    for(int i = 0; i < (int) temp; i++)
      *(fCoef+i) = 0.5 + 0.5*cos(3.14159265358979*((i/(double)temp) - 1));
    for(unsigned int i = (unsigned int) temp; i < (unsigned int) (fCoefSize-temp); i++)
      *(fCoef+i) = 1.0;
    for(unsigned int i = (unsigned int) (fCoefSize-temp); i < fCoefSize; i++)
      *(fCoef+i) = 0.5 + 0.5*cos(3.14159265358979*((i/(double) temp) - 2.0/alpha - 1));		
  }

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


