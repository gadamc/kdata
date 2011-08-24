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
//#include "Rtypes.h"
 
class KPtaProcessor  { 

public:
  KPtaProcessor(void);
  virtual ~KPtaProcessor(void);

  //for the memory-savy programmers
  KPtaProcessor(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize){
    SetInputPulse(inPulse); SetInputPulseSize(inSize); SetOutputPulse(outPulse); SetOutputPulseSize(outsize);
  }
  //all derived classes must over-ride this method
  virtual bool RunProcess(void) = 0;
  
  //this weird pattern of coding is so that SetInputPulse can be overloaded in derived classes.
  //and because using template functions in ROOT's CINT isn't trivial.
  virtual void SetInputPulse(std::vector<double> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(std::vector<float> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(std::vector<int> &aPulse){SetTheInputPulse(aPulse);}
  virtual void SetInputPulse(std::vector<short> &aPulse){SetTheInputPulse(aPulse);}
    
  virtual void SetInputPulse(const double* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const float* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const int* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}
  virtual void SetInputPulse(const short* aPulse, unsigned int size){SetTheInputPulse(aPulse, size);}

	virtual void SetInputPulse(const char* aFile);
	
	//for the memory-savy programmers. 
  virtual void SetInputPulse(double *aPulse);
  virtual void SetOutputPulse(double *aPulse);
  virtual void SetInputPulseSize(unsigned int s){fInputSize = s;}
  virtual void SetOutputPulseSize(unsigned int s){fOutputSize = s;}
  virtual void DeleteInputPulse(void){if(fInputPulse){delete [] fInputPulse; fInputPulse = 0; fInputSize = 0;}}
  virtual void DeleteOutputPulse(void){if(fOutputPulse){delete [] fOutputPulse; fOutputPulse = 0; fOutputSize = 0;}}
  virtual void DoNotDeletePulses(bool opt){fDoNotDelete = opt;}
  //
  
	virtual double* GetOutputPulse(void) const {return fOutputPulse;}  
  virtual unsigned int GetOutputPulseSize(void) const {return fOutputSize;}
	virtual double* GetInputPulse(void) const {return fInputPulse;}  
  virtual unsigned int GetInputPulseSize(void) const {return fOutputSize;}

	virtual const char* GetName(void) const {return fProcessorName.c_str();}
	virtual void SetName(const char* aName){fProcessorName = aName;}
	
	
	
	//ClassDef(KPtaProcessor,1)
	
protected:

	std::string fProcessorName;
	double* fInputPulse;
	double* fOutputPulse;
  unsigned int fInputSize;
  unsigned int fOutputSize;
  bool fDoNotDelete;
  
	template <class T> void SetTheInputPulse(const std::vector<T> &aPulse);
  template <class T> void SetTheInputPulse(const T* aPulse, unsigned int size);
  virtual void AllocateArrays(unsigned int size);
  
  
  
private:
	
  //private methods
  void InitializeMembers(void);
	
  
  
};

template <class T> void KPtaProcessor::SetTheInputPulse(const T* aPulse, unsigned int size)
{
  if(size != fInputSize)
    AllocateArrays(size);

  for(unsigned int i = 0; i < size; i++)
    *(fInputPulse+i) = aPulse[i];
}


template <class T> void KPtaProcessor::SetTheInputPulse(const std::vector<T> &aPulse)
{
  if(aPulse.size() != fInputSize)
    AllocateArrays(aPulse.size());  
	
	try {
		for(unsigned int i = 0; i < fInputSize; i++){
			*(fInputPulse+i) = aPulse.at(i);
		}
	}
	catch (std::out_of_range& e) {
		//I think this should be impossible... 
		std::cerr << "KPtaProcessor::SetThisToInputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << std::endl;
	}
}


#endif // __KPTAPROCESSOR_H__
