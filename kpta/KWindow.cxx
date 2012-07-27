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

ClassImp(KWindow);

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
  if(fWindow) delete [] fWindow;
}


void KWindow::SetWindow(const double *window, unsigned int windowSize)
{
    if(windowSize != fWindowSize || fWindow == 0){
      if(fWindow) delete [] fWindow;
      fWindowSize = windowSize;
      fWindow = new double[fWindowSize];
    }
    memcpy(fWindow, window, fWindowSize*sizeof(double));

}

bool KWindow::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. nothing done. " << endl;
    return false;
  }
  if(fWindowSize == 0){
    cerr<<"WARNING: window is not set! => output = input"<<endl;
    for(unsigned int i = 0 ; i < fOutputSize; i++)
      *(fOutputPulse+i) =  *(fInputPulse+i);
    return true;
  }  
  
  if(fWindowSize != fOutputSize){
    cerr<<"window must be the same size as the pulse"<<endl;
    return false;
  }
 
  for(unsigned int i = 0 ; i < fOutputSize; i++)
    *(fOutputPulse+i) =  *(fInputPulse+i) * *(fWindow+i); 
  
  return true;
}



void KWindow::InitializeMembers(void)
{
  fWindow = 0;
  fWindowSize = 0;
  
}


