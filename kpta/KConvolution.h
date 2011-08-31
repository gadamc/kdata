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
	
  virtual void SetResponse(std::vector<double> &r){SetTheResponse(r);}
  virtual void SetResponse(std::vector<float> &r){SetTheResponse(r);}
  virtual void SetResponse(std::vector<int> &r){SetTheResponse(r);}
  virtual void SetResponse(std::vector<short> &r){SetTheResponse(r);}

  virtual void SetResponse(const double* resp, unsigned int size){SetTheResponse(resp, size);}
  virtual void SetResponse(const float* resp, unsigned int size){SetTheResponse(resp, size);}
  virtual void SetResponse(const int* resp, unsigned int size){SetTheResponse(resp, size);}
  virtual void SetResponse(const short* resp, unsigned int size){SetTheResponse(resp, size);}
	
protected:
  double *fResponse;
  unsigned int fResponseSize;
  template <class T> void SetTheResponse(std::vector<T> &resp, bool reverse = false);
  template <class T> void SetTheResponse(const T* resp, unsigned int size, bool reverse = false);
  
private:
  //private methods
  void InitializeMembers(void);


 // ClassDef(KConvolution,1);
  
};


template <class T> void KConvolution::SetTheResponse(std::vector<T> &resp, bool reverse)
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

template <class T> void KConvolution::SetTheResponse(const T* resp, unsigned int size, bool reverse)
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
