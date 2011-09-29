//______________________________________________________________________
//
// KRingBuffer.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// KRingBuffer is a generic circular buffer which can be used to store a desired amount of
// pulses, spectra etc. (any double* objects of the same length). The output pulse of this class
// is a pulse delayed by the size of buffer if the buffer is full and a zero pulse if it is not
// the case. This class also provides some basic statistics (mean and std. deviation) for the
// stored content.

#include "KRingBuffer.h"
#include <cmath>
#include <iostream>
#include <string.h>

using namespace std;
//ClassImp(KRingBuffer);

KRingBuffer::KRingBuffer(void)
{
  SetName("KRingBuffer");
  InitializeMembers();
}

KRingBuffer::KRingBuffer(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize)
: KPtaProcessor(inPulse, inSize, outPulse, outsize)
{
  SetName("KRingBuffer"); 
  InitializeMembers();
}

KRingBuffer::~KRingBuffer(void)
{
  if(fBuffer) delete [] fBuffer;
  if(fMeanValues) delete [] fMeanValues;
  if(fDeviationValues) delete [] fDeviationValues;
  if(fSums) delete [] fSums;
  if(fSqSums) delete [] fSqSums;
    
}


void KRingBuffer::SetBufferSize(unsigned int BufferSize)
{
    if(BufferSize != fBufferSize || fBuffer == 0){
      if(fBuffer) delete [] fBuffer;
      fBufferSize = BufferSize;
      if(fInputSize != 0)
        fBuffer = new double[fBufferSize*fInputSize];
      else
        fBuffer = 0;
    }
    memset(fBuffer, 0.0, fBufferSize*fInputSize*sizeof(double));

}

bool KRingBuffer::RunProcess(void)
{
  if(fInputPulse == 0 || fOutputPulse == 0) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }
  if( fOutputSize != fInputSize) {
    cerr << "input and output pulses must be of the same size. nothing done. " << endl;
    return false;
  }
  if(fBufferSize == 0){
    cerr<<"buffer size is not set"<<endl;
    return false;
  }  
  
  // substract the pulse, which will be removed from the sums
  if(fNumStoredPulses == fBufferSize)
    for(unsigned int i = 0; i < fInputSize; i++){
      *(fSums+i) -= *(fBuffer+fPos*fInputSize+i);
      *(fSqSums+i) -= *(fBuffer+fPos*fInputSize+i)*(*(fBuffer+fPos*fInputSize+i));
    }
  // push the removed pulse to the output, store new pulse and update the sums  
  for(unsigned int i = 0; i < fInputSize; i++){
    *(fOutputPulse+i) = *(fBuffer+fPos*fInputSize+i);
    *(fBuffer+fPos*fInputSize+i) = *(fInputPulse+i);
    *(fSums+i) +=  *(fInputPulse+i);
    *(fSqSums+i) += *(fInputPulse+i)*(*(fInputPulse+i));
  }
  
  fPos = ( ++fPos ) % fBufferSize;
  
  
  if(fNumStoredPulses < fBufferSize)
    fNumStoredPulses++;
  fRecalculateStatistics = true;   
  
  return true;
}

void KRingBuffer::RecalculateStatistics(void)
{
  for(unsigned int i = 0; i< fInputSize; i++){
    *(fMeanValues+i) = *(fSums+i)/(double) fNumStoredPulses;
    *(fDeviationValues+i) = sqrt(*(fSqSums+i)/(double) fNumStoredPulses - *(fMeanValues+i)*(*(fMeanValues+i)));
  }
  fRecalculateStatistics = false;
}

void KRingBuffer::InitializeMembers(void)
{
  fBuffer = 0;
  fMeanValues = 0;
  fDeviationValues = 0;
  fBufferSize = 0;
  fNumStoredPulses = 0;
  fPos = 0;
  fSums = 0;
  fSqSums = 0;
  fRecalculateStatistics = true;
  
}

void KRingBuffer::Reset(void)
{
  // reset the content of the ring buffer
  fPos = 0;
  fNumStoredPulses = 0;
  fRecalculateStatistics = true;
  fBufferSize = (fBufferSize == 0) ? 1 : fBufferSize;
  if(fBuffer) delete [] fBuffer;
  fBuffer = new double[ fBufferSize*fInputSize ];
  memset(fBuffer, 0, fBufferSize*fInputSize*sizeof(double));
  if(fMeanValues) delete [] fMeanValues;
  if(fDeviationValues) delete [] fDeviationValues;
  fMeanValues = new double[ fInputSize ];
  fDeviationValues = new double[ fInputSize ];
  memset(fMeanValues, 0, fInputSize*sizeof(double));
  memset(fDeviationValues, 0, fInputSize*sizeof(double));
  if(fSums) delete [] fSums;
  if(fSqSums) delete [] fSqSums;
  fSums = new double[ fInputSize ];
  fSqSums = new double[ fInputSize ];
  memset(fSums, 0, fInputSize*sizeof(double));
  memset(fSqSums, 0, fInputSize*sizeof(double));
}

double* KRingBuffer::GetMeanValues(void)
{
  // returns a pulse, which contains the mean value of all stored pulse values for every
  // pulse sample
  if(fRecalculateStatistics)
    RecalculateStatistics();
  return fMeanValues;
}

double* KRingBuffer::GetDeviationValues(void)
{
  // return a pulse, which contains the standard deviation of all stored pulse values for every
  // pulse sample
  if(fRecalculateStatistics)
    RecalculateStatistics();
  return fDeviationValues;
}


void KRingBuffer::AllocateArrays(unsigned int size)
{
  KPtaProcessor::AllocateArrays(size);
  Reset();  
}
