//______________________________________________________________________
//
// KNoiseWhitening.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2012 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KNOISEWHITENING_H__
#define __KNOISEWHITENING_H__

#include "KPtaProcessor.h"

class KNoiseWhitening : public KPtaProcessor { 

public:
  //Constructors
  KNoiseWhitening(void);
  //for the memory-savy programmers
  KNoiseWhitening(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KNoiseWhitening(void);

  virtual bool RunProcess(void);
  
  //getters
  virtual double* GetNoiseSpectrum(void){return fNoiseSpectrum;}
  virtual unsigned int GetNoiseSpectrumSize(void){return fNoiseSpectrumSize;}
  
  //setters
  virtual void SetNoiseSpectrum(std::vector<double> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<float> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<int> &r){SetTheNoiseSpectrum(r);}
  virtual void SetNoiseSpectrum(std::vector<short> &r){SetTheNoiseSpectrum(r);}

  virtual void SetNoiseSpectrum(const double* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const float* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const int* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  virtual void SetNoiseSpectrum(const short* resp, unsigned int size){SetTheNoiseSpectrum(resp, size);}
  
  virtual void SetNoiseSpectrum(double* resp) {fNoiseSpectrum = resp;} //memory experts only
  virtual void SetNoiseSpectrumSize(unsigned int size) {fNoiseSpectrumSize = size;} //memory experts only
  
protected:
  double *fNoiseSpectrum;
  unsigned int fNoiseSpectrumSize;
  template <class T> void SetTheNoiseSpectrum(std::vector<T> &resp);
  template <class T> void SetTheNoiseSpectrum(const T* resp, unsigned int size);
  
private:

  //private methods
  void InitializeMembers(void);

  //ClassDef(KNoiseWhitening,1);
  
};

template <class T> void KNoiseWhitening::SetTheNoiseSpectrum(std::vector<T> &resp)
{
  if (resp.size() != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = resp.size();
    fNoiseSpectrum = new double[fNoiseSpectrumSize];
  }
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    *(fNoiseSpectrum+i) = resp[i];
}

template <class T> void KNoiseWhitening::SetTheNoiseSpectrum(const T* resp, unsigned int size)
{
  if (size != fNoiseSpectrumSize){
    if(fNoiseSpectrum) delete [] fNoiseSpectrum;
    fNoiseSpectrumSize = size;
    fNoiseSpectrum = new double[fNoiseSpectrumSize];    
  }
  
  for(unsigned int i = 0; i < fNoiseSpectrumSize; i++)
    *(fNoiseSpectrum+i) = resp[i];
  
}

#endif // __KNOISEWHITENING_H__
