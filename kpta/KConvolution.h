//______________________________________________________________________
//
// KConvolution.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KCONVOLUTION_H__
#define __KCONVOLUTION_H__

#include "KPtaProcessor.h"
#include <string.h>
#include <iostream>

class KConvolution : public KPtaProcessor { 
  
public:
  //Constructors
  KConvolution(void);
  virtual ~KConvolution(void);
  //for the memory-savy programmers
  KConvolution(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
	virtual bool RunProcess(void);
	
	virtual double* GetResponse(void){return fResponse;}
  virtual unsigned int GetResponseSize(void){return fResponseSize;}
  
  template <class T> void SetResponse(std::vector<T> &resp, bool reverse = false);
  template <class T> void SetResponse(const T* resp, unsigned int size, bool reverse = false);

protected:
  double *fResponse;
  unsigned int fResponseSize;

  
private:
  //private methods
  void InitializeMembers(void);
  


  //ClassDef(KConvolution,1);
  
};


template <class T> void KConvolution::SetResponse(std::vector<T> &resp, bool reverse)
{

  if (resp.size() != fResponseSize){
    if(fResponse) delete [] fResponse;
    fResponseSize = resp.size();
    fResponse = new double[fResponseSize];
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fResponseSize; i++)
      *(fResponse+i) = resp[i];

  else
    for(unsigned int i = 0; i < fResponseSize; i++)
      *(fResponse+i) = resp[fResponseSize-1 - i];
}

template <class T> void KConvolution::SetResponse(const T* resp, unsigned int size, bool reverse)
{
  if (size != fResponseSize){
    if(fResponse) delete [] fResponse;
    fResponseSize = size;
    fResponse = new double[fResponseSize];    
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fResponseSize; i++)
      *(fResponse+i) = resp[i];

  else
    for(unsigned int i = 0; i < fResponseSize; i++)
      *(fResponse+i) = resp[fResponseSize-1 - i];

  //std::copy(resp, resp + size, fResponse);
}

#endif
