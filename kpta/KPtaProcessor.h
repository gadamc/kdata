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
#include <string.h>
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

  template <class T> void SetInputPulse(const std::vector<T> &aPulse);
  template <class T> void SetInputPulse(const T* aPulse, unsigned int size);
  virtual void SetInputPulse(KPtaProcessor *pta) {SetInputPulse(pta->GetOutputPulse(), pta->GetOutputPulseSize());}
  virtual void SetInputPulse(const KPtaProcessor &pta) {SetInputPulse(pta.GetOutputPulse(), pta.GetOutputPulseSize());}
  virtual void SetInputPulse(const char* aFile);

  //for the memory-savy programmers. 
  virtual void SetInputPulse(double *aPulse);
  virtual void SetOutputPulse(double *aPulse);
  virtual void SetInputPulseSize(unsigned int s){fInputSize = s;}
  virtual void SetOutputPulseSize(unsigned int s){fOutputSize = s;}
  virtual void DeleteInputPulse(void){if(fInputPulse){delete [] fInputPulse; fInputPulse = 0; fInputSize = 0;}}
  virtual void DeleteOutputPulse(void){if(fOutputPulse){delete [] fOutputPulse; fOutputPulse = 0; fOutputSize = 0;}}
  virtual void DoNotDeletePulses(bool opt){fDoNotDelete = opt;}

  virtual double* GetOutputPulse(void) const {return fOutputPulse;}  
  virtual unsigned int GetOutputPulseSize(void) const {return fOutputSize;}
  virtual double* GetInputPulse(void) const {return fInputPulse;}  
  virtual unsigned int GetInputPulseSize(void) const {return fInputSize;}

  virtual const char* GetName(void) const {return fProcessorName.c_str();}
  virtual void SetName(const char* aName){fProcessorName = aName;}

  virtual void AllocateArrays(unsigned int size);

  //make a kind of generic interface supporting these methods in the KPulseAnalysisChain class
  //these methods are overwritten in the KPulseAnalysisChain
  virtual KPtaProcessor* GetProcessor(unsigned int /*i = 0*/) { return this;}
  virtual unsigned int GetNumProcessors(void) const {return 1;}

  

protected:

  std::string fProcessorName;
  double* fInputPulse;
  double* fOutputPulse;
  unsigned int fInputSize;
  unsigned int fOutputSize;
  bool fDoNotDelete;



private:

  //private methods
  void InitializeMembers(void);

  //ClassDef(KPtaProcessor,1);

};

template <class T> void KPtaProcessor::SetInputPulse(const T* aPulse, unsigned int size)
{
  if(size != fInputSize)
    AllocateArrays(size);

  for(unsigned int i = 0; i < size; i++)
    *(fInputPulse+i) = aPulse[i];
}


template <class T> void KPtaProcessor::SetInputPulse(const std::vector<T> &aPulse)
{
  if(aPulse.size() != fInputSize)
    AllocateArrays(aPulse.size());  

  try {
    for(unsigned int i = 0; i < fInputSize; i++){
      *(fInputPulse+i) = aPulse[i]; //use [i]. its faster than .at(i) 
    }
  }
  catch (std::out_of_range& e) {
    //I think this should be impossible... 
    std::cerr << "KPtaProcessor::SetThisToInputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << std::endl;
  }
}


#endif // __KPTAPROCESSOR_H__
