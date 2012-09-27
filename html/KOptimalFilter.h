//______________________________________________________________________
//
// KOptimalFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KOPTIMALFILTER_H__
#define __KOPTIMALFILTER_H__

#include "KPtaProcessor.h"
#include "KHalfComplexArray.h"

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
  
  virtual void SetToRecalculate(bool option = true){fRecalculate= option;}
  template <class T> void SetNoiseSpectrum(std::vector<T> &r);
  template <class T> void SetNoiseSpectrum(const T* resp, unsigned int size);

  // virtual void SetNoiseSpectrum(std::vector<double> &r){SetTheNoiseSpectrum(r);}
  // virtual void SetNoiseSpectrum(std::vector<float> &r){SetTheNoiseSpectrum(r);}
  // virtual void SetNoiseSpectrum(std::vector<int> &r){SetTheNoiseSpectrum(r);}
  // virtual void SetNoiseSpectrum(std::vector<short> &r){SetTheNoiseSpectrum(r);}

  // virtual void SetNoiseSpectrum(const double* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  // virtual void SetNoiseSpectrum(const float* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  // virtual void SetNoiseSpectrum(const int* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  // virtual void SetNoiseSpectrum(const short* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  
  virtual void SetNoiseSpectrum(double* resp);
  virtual void SetNoiseSpectrumSize(unsigned int size);
  
  template <class T> void SetTemplateDFT(std::vector<T> &r);
  template <class T> void SetTemplateDFT(const T* resp, unsigned int size);

  // virtual void SetTemplateDFT(std::vector<double> &r){SetTheTemplateDFT(r);}
  // virtual void SetTemplateDFT(std::vector<float> &r){SetTheTemplateDFT(r);}
  // virtual void SetTemplateDFT(std::vector<int> &r){SetTheTemplateDFT(r);}
  // virtual void SetTemplateDFT(std::vector<short> &r){SetTheTemplateDFT(r);}

  // virtual void SetTemplateDFT(const double* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  // virtual void SetTemplateDFT(const float* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  // virtual void SetTemplateDFT(const int* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  // virtual void SetTemplateDFT(const short* resp, unsigned int size){SetTheTemplateDFT(resp, size);}
  
  virtual void SetTemplateDFT(double* resp);
  virtual void SetTemplateDFTSize(unsigned int size);
  
  virtual void SetOutputPulse(double *aPulse);
  virtual void SetOutputPulseSize(unsigned int s);
  
  virtual double GetChiSquared(unsigned int aTimeBin);
  
protected:
  double *fNoiseSpectrum;
  unsigned int fNoiseSpectrumSize;
  //template <class T> void SetTheNoiseSpectrum(std::vector<T> &resp);
  //template <class T> void SetTheNoiseSpectrum(const T* resp, unsigned int size);
  
  double *fTemplateDFT;
  unsigned int fTemplateDFTSize;
  //template <class T> void SetTheTemplateDFT(std::vector<T> &resp);
  //template <class T> void SetTheTemplateDFT(const T* resp, unsigned int size);
  
  double *fOptFilter;
  unsigned int fOptFilterSize;
  bool fRecalculate;
  
  double *fOptFilterAndSignal;
  unsigned int fOptFilterAndSignalSize;
  
  double *fTemplatePower;
  unsigned int fTemplatePowerSize;
  
  KHalfComplexToRealDFT *fHc2r;
  KHalfComplexPower *fHcPower;
  
private:
  //private methods
  void InitializeMembers(void);

  KHalfComplexArray fComplex;
  
  //ClassDef(KOptimalFilter,1);
  
};


template <class T> void KOptimalFilter::SetNoiseSpectrum(std::vector<T> &resp)
{
  if (resp.size() != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = resp.size();
    fNoiseSpectrum = new double[fNoiseSpectrumSize];
  }
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    *(fNoiseSpectrum+i) = resp[i];
  
  fRecalculate = true;
  //std::copy(resp.begin(), resp.end(), fNoiseSpectrum);
}

template <class T> void KOptimalFilter::SetNoiseSpectrum(const T* resp, unsigned int size)
{
  if (size != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = size;
    fNoiseSpectrum = new double[fNoiseSpectrumSize];    
  }
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    *(fNoiseSpectrum+i) = resp[i];
      
  fRecalculate=true;
  //std::copy(resp, resp + size, fNoiseSpectrum);
}



template <class T> void KOptimalFilter::SetTemplateDFT(std::vector<T> &resp)
{
  if (resp.size() != fTemplateDFTSize){
    if(fTemplateDFT) delete [] fTemplateDFT;
    fTemplateDFTSize = resp.size();
    fTemplateDFT = new double[fTemplateDFTSize];
  }
  
  for(unsigned int i = 0; i < fTemplateDFTSize; i++)
    *(fTemplateDFT+i) = resp[i];

  fRecalculate=true;
  //std::copy(resp.begin(), resp.end(), fNoiseSpectrum);
}

template <class T> void KOptimalFilter::SetTemplateDFT(const T* resp, unsigned int size)
{
  if (size != fTemplateDFTSize){
    if(fTemplateDFT) delete [] fTemplateDFT;
    fTemplateDFTSize = size;
    fTemplateDFT = new double[fTemplateDFTSize];    
  }
  
  for(unsigned int i = 0; i < fTemplateDFTSize; i++)
    *(fTemplateDFT+i) = resp[i];
 
  fRecalculate=true;     
  //std::copy(resp, resp + size, fTemplateDFT);
}



#endif