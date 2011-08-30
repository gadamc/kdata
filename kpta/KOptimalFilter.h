//______________________________________________________________________
//
// KOptimalFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KOPTIMALFILTER_H__
#define __KOPTIMALFILTER_H__

#include "KPtaProcessor.h"

class KHalfComplexToRealDFT;
class KHalfComplexPower;

class KOptimalFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KOptimalFilter(void);
  virtual ~KOptimalFilter(void);
  //for the memory-savy programmers
  KOptimalFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
	virtual bool RunProcess(void);
  virtual bool BuildFilter(void);
  
	virtual double* GetNoiseSpectrum(void){return fNoiseSpectrum;}
  virtual unsigned int GetNoiseSpectrumSize(void){return fNoiseSpectrumSize;}
  virtual double* GetTemplateDFT(void){return fTemplateDFT;}
  virtual unsigned int GetTemplateDFTSize(void){return fTemplateDFTSize;}
  virtual double* GetOptimalFilter(void){return fOptFilter;}
  virtual unsigned int GetOptimalFilterSize(void){return fOptFilterSize;}
  
	
  virtual void SetNoiseSpectrum(std::vector<double> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<float> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<int> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<short> &r){SetTheNoiseSpectrum(r);}

  virtual void SetNoiseSpectrum(const double* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const float* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const int* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const short* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  
  
  virtual void SetTemplateDFT(std::vector<double> &r){SetTheTemplateDFT(r);}
  virtual void SetTemplateDFT(std::vector<float> &r){SetTheTemplateDFT(r);}
  virtual void SetTemplateDFT(std::vector<int> &r){SetTheTemplateDFT(r);}
  virtual void SetTemplateDFT(std::vector<short> &r){SetTheTemplateDFT(r);}

  virtual void SetTemplateDFT(const double* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  virtual void SetTemplateDFT(const float* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  virtual void SetTemplateDFT(const int* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  virtual void SetTemplateDFT(const short* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  
  
	
protected:
  double *fNoiseSpectrum;
  unsigned int fNoiseSpectrumSize;
  template <class T> void SetTheNoiseSpectrum(std::vector<T> &resp, bool reverse = false);
  template <class T> void SetTheNoiseSpectrum(const T* resp, unsigned int size, bool reverse = false);
  
  double *fTemplateDFT;
  unsigned int fTemplateDFTSize;
  template <class T> void SetTheTemplateDFT(std::vector<T> &resp, bool reverse = false);
  template <class T> void SetTheTemplateDFT(const T* resp, unsigned int size, bool reverse = false);
  
  double *fOptFilter;
  unsigned int fOptFilterSize;
  bool fRecalculate;
  
  double *fOptFilterAndSignal;
  unsigned int fOptFilterAndSignalSize;
  
  KHalfComplexToRealDFT *fHc2r;
  KHalfComplexPower *fHcPower;
  
private:
  //private methods
  void InitializeMembers(void);


 // ClassDef(KOptimalFilter,1);
  
};


template <class T> void KOptimalFilter::SetTheNoiseSpectrum(std::vector<T> &resp, bool reverse)
{
  if (resp.size() != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = resp.size();
    fNoiseSpectrum = new double[fNoiseSpectrumSize];
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
      *(fNoiseSpectrum+i) = resp[i];

  else
    for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
      *(fNoiseSpectrum+i) = resp[fNoiseSpectrumSize-1 - i];
  
  fRecalculate = true;
}

template <class T> void KOptimalFilter::SetTheNoiseSpectrum(const T* resp, unsigned int size, bool reverse)
{
  if (size != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = size;
    fNoiseSpectrum = new double[fNoiseSpectrumSize];    
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
      *(fNoiseSpectrum+i) = resp[i];

  else
    for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
      *(fNoiseSpectrum+i) = resp[fNoiseSpectrumSize-1 - i];
      
  fRecalculate=true;
  //std::copy(resp, resp + size, fNoiseSpectrum);
}



template <class T> void KOptimalFilter::SetTheTemplateDFT(std::vector<T> &resp, bool reverse)
{
  if (resp.size() != fTemplateDFTSize){
    if(fTemplateDFT) delete [] fTemplateDFT;
    fTemplateDFTSize = resp.size();
    fTemplateDFT = new double[fTemplateDFTSize];
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fTemplateDFTSize; i++)
      *(fTemplateDFT+i) = resp[i];

  else
    for(unsigned int i = 0; i < fTemplateDFTSize; i++)
      *(fTemplateDFT+i) = resp[fTemplateDFTSize-1 - i];

  fRecalculate=true;
}

template <class T> void KOptimalFilter::SetTheTemplateDFT(const T* resp, unsigned int size, bool reverse)
{
  if (size != fTemplateDFTSize){
    if(fTemplateDFT) delete [] fTemplateDFT;
    fTemplateDFTSize = size;
    fTemplateDFT = new double[fTemplateDFTSize];    
  }
  
  if(!reverse)
    for(unsigned int i = 0; i < fTemplateDFTSize; i++)
      *(fTemplateDFT+i) = resp[i];

  else
    for(unsigned int i = 0; i < fTemplateDFTSize; i++)
      *(fTemplateDFT+i) = resp[fTemplateDFTSize-1 - i];
 
  fRecalculate=true;     
  //std::copy(resp, resp + size, fTemplateDFT);
}



#endif