/*
 *  KPtaProcessor.h
 *  kData
 *
 *  Created by Adam Cox on 8/24/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KPTAPROCESSOR_H__
#define __KPTAPROCESSOR_H__

#include <stdexcept> 
#include <iostream>
#include <vector>

using namespace std;

class KPtaProcessor  { 

public:
  KPtaProcessor(void);
  virtual ~KPtaProcessor(void);

  //all derived classes must over-ride this method
  virtual bool RunProcess(void) = 0;
  
  //this weird pattern is so that SetInputPulse can be overloaded in derived classes.
  virtual void SetInputPulse(vector<double> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(vector<float> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(vector<int> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(vector<short> &aPulse){SetTheInputPulse(aPulse);}
    
  virtual void SetInputPulse(const double* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const float* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const int* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const short* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  
	virtual void SetInputPulse(const char* aFile);
	
	virtual double* GetOutputPulse(void) const {return fOutputPulse;}  
  virtual unsigned int GetOutputPulseSize(void) const {return fOutputSize;}
	virtual double* GetInputPulse(void) const {return fInputPulse;}  
  virtual unsigned int GetInputPulseSize(void) const {return fOutputSize;}

	virtual const char* GetName(void) const {return fProcessorName.c_str();}
	virtual void SetName(const char* aName){fProcessorName = aName;}
	
protected:

	string fProcessorName;
	double* fInputPulse;
	double* fOutputPulse;
  unsigned int fInputSize;
  unsigned int fOutputSize;
	
	template <class T> void SetTheInputPulse(const vector<T> &aPulse);
  template <class T> void SetTheInputPulse(const T* aPulse, unsigned int size);
  virtual void AllocateArrays(unsigned int size);
  
  
private:
	
  //private methods
  void InitializeMembers(void);
	
  
};


#endif // __KPTAPROCESSOR_H__
