//______________________________________________________________________
//
// KPulseShifter.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// KPulseShifter class can be used to shift a pulse to left (negative shift value) or to the right (positive shift value)
// There are three modes available: 
//      Mode 0 = unknown samples at the edge are filled with the first/last value of the pulses
//      Mode 1 = cyclically shifting
//      Mode 2 = unknown samples at the edge are filled with zeros

#include "KPulseShifter.h"
#include <cmath>
#include <iostream>
#include <string.h>

using namespace std;

//ClassImp(KPulseShifter);

KPulseShifter::KPulseShifter(void)
{
  SetName("KPulseShifter");
  InitializeMembers();
}

KPulseShifter::KPulseShifter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
: KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KPulseShifter"); 
  InitializeMembers();
}

KPulseShifter::~KPulseShifter(void)
{
}


bool KPulseShifter::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. nothing done. " << endl;
    return false;
  }
  
  if(fPulseShift == 0 && (fInputPulse == fOutputPulse))
    return true; //in this case, there is nothing to do... save cpu. don't loop.
    
  switch(fMode){
    case 0:
      {
        // Shifting and filling the unknown points at the edge with the first/last values in the trace
        for(unsigned int i = 0 ; i < fOutputSize; i++)
          if( (int)(i - fPulseShift)>=0 &&  (int)(i-fPulseShift)<(int)fOutputSize)
            *(fOutputPulse+i) = *(fInputPulse+i-fPulseShift);
          else{
            if((i - fPulseShift)<0)
              *(fOutputPulse+i) = *(fInputPulse);
            else
              *(fOutputPulse+i) = *(fInputPulse+fOutputSize-1);
          }
          break;          
    }
    case 1:
    {
      // Cyclically shifting the trace
      for(unsigned  int i = 0 ; i < fOutputSize; i++)
        *(fOutputPulse+i) = *(fInputPulse+((i-fPulseShift+fOutputSize)%fOutputSize));
      break;
    }
    case 2:
      {
        // Shifting and filling the unknown points at the edge with zeros
        for(unsigned int i = 0 ; i < fOutputSize; i++)
          if( (int)(i-fPulseShift)<0  || (int)(i-fPulseShift)>=(int)fOutputSize )
            *(fOutputPulse+i) = 0;
          else
            *(fOutputPulse+i) = *(fInputPulse+i-fPulseShift);
          
        break;
      }
    default:
    {
       cerr<<"unknown shifting mode. nothing done!"<<endl;
       return false;
    }
          
        
  }
    return true;
}



void KPulseShifter::InitializeMembers(void)
{
  fPulseShift = 0;
  fMode = 1;
  
}


