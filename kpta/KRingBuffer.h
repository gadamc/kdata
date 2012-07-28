//______________________________________________________________________
//
// KRingBuffer.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KRINGBUFFER_H__
#define __KRINGBUFFER_H__

#include "KPtaProcessor.h"
#include <string.h>

class KRingBuffer : public KPtaProcessor { 

public:
  //Constructors
  KRingBuffer(void);
  //for the memory-savy programmers
  KRingBuffer(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KRingBuffer(void);

  virtual bool RunProcess(void);
	virtual void SetBufferSize(unsigned int BufferSize);
  virtual double* GetBuffer(void) {return fBuffer;}
  virtual unsigned int GetBufferSize(void) {return fBufferSize;}
  virtual void AllocateArrays(unsigned int size);
  virtual void RecalculateStatistics(void);
  virtual double* GetMeanValues(void);
  virtual double* GetDeviationValues(void);
  virtual unsigned int GetNumStoredPulses(void) {return fNumStoredPulses;}
  virtual void Reset(void);

protected:
  double *fBuffer;
  double *fMeanValues;
  double *fDeviationValues;
  unsigned int fBufferSize;
  unsigned int fNumStoredPulses;
  unsigned int fPos;
  double *fSums;
  double *fSqSums;
  bool fRecalculateStatistics;

private:
  //private methods
  void InitializeMembers(void);

  ClassDef(KRingBuffer,1);

};


#endif
