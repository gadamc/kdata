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

class KConvolution : public KPtaProcessor { 
  
public:
  //Constructors
  KConvolution(void);
  virtual ~KConvolution(void);
  
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
  template <class T> void SetTheResponse(std::vector<T> &resp);
  template <class T> void SetTheResponse(const T* resp, unsigned int size);
  
private:
  //private methods
  void InitializeMembers(void);


 // ClassDef(KConvolution,1);
  
};
#endif

template <class T> void KConvolution::SetTheResponse(std::vector<T> &resp)
{
  if (resp.size() != fResponseSize){
    if(fResponse) delete [] fResponse;
    fResponseSize = resp.size();
    fResponse = new double[fResponseSize];
  }
  for(unsigned int i = 0; i < fResponseSize; i++)
    *(fResponse+i) = resp[i];
}

template <class T> void KConvolution::SetTheResponse(const T* resp, unsigned int size)
{
  if (size != fResponseSize){
    if(fResponse) delete [] fResponse;
    fResponseSize = size;
    fResponse = new double[fResponseSize];    
  }
  for(unsigned int i = 0; i < fResponseSize; i++)
    *(fResponse+i) = resp[i];

  //std::copy(resp, resp + size, fResponse);
}
