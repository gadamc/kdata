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

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
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

  virtual double* GetTemplatePower(void){return fTemplatePower;}
  virtual unsigned int GetTemplatePowerSize(void){return fTemplatePowerSize;}
  virtual double* GetOptFilterAndSignal(void){return fOptFilterAndSignal;}
  virtual unsigned int GetOptFilterAndSignalSize(void){return fOptFilterAndSignalSize;}
  
  virtual double GetAmpEstimatorDenominator(void){return fAmpEstimatorDenominator;}
  
  virtual void SetToRecalculate(bool option = true){fRecalculate= option;}
  template <class T> void SetNoiseSpectrum(std::vector<T> &r);
  template <class T> void SetNoiseSpectrum(const T* resp, unsigned int size);


  virtual void SetNoiseSpectrum(double* resp);
  virtual void SetNoiseSpectrumSize(unsigned int size);
  
  template <class T> void SetTemplateDFT(std::vector<T> &r);
  template <class T> void SetTemplateDFT(const T* resp, unsigned int size);

  
  virtual void SetTemplateDFT(double* resp);
  virtual void SetTemplateDFTSize(unsigned int size);
  
  virtual void SetOutputPulse(double *aPulse);
  virtual void SetOutputPulseSize(unsigned int s);
  
  virtual double GetChiSquared(double amp, unsigned int index);

  inline double GetPhase(double freq_k, double index){return  2.0 * M_PI * freq_k * index / (2.0*((double)fNoiseSpectrumSize - 1.0));}
  double GetChiSquareElement(double optimalAmp, unsigned int index, unsigned int freq_k, bool debug = false);

  virtual unsigned int GetOptimalFilterTimeRangeMin(void){return fOptimalFilterTimeRangeMin;}
  virtual void SetOptimalFilterTimeRangeMin(unsigned int a){fOptimalFilterTimeRangeMin = a;}

  virtual int GetOptimalFilterTimeRangeMax(void){return fOptimalFilterTimeRangeMax;}
  virtual void SetOptimalFilterTimeRangeMax(int a){fOptimalFilterTimeRangeMax = a;}

protected:
  double *fNoiseSpectrum;
  unsigned int fNoiseSpectrumSize;
  
  double *fTemplateDFT;
  unsigned int fTemplateDFTSize;
 
  double *fOptFilter;
  unsigned int fOptFilterSize;
  bool fRecalculate;
  
  double *fOptFilterAndSignal;
  unsigned int fOptFilterAndSignalSize;
  
  double *fTemplatePower;
  unsigned int fTemplatePowerSize;
  
  KHalfComplexToRealDFT *fHc2r;
  KHalfComplexPower *fHcPower;
  
  double fAmpEstimatorDenominator;  //should be the variance / N 

  unsigned int fOptimalFilterTimeRangeMin;  //the minimum search range for the optimal filter amplitude estimator (default = 0)
  int fOptimalFilterTimeRangeMax;  //the maximim search range for the optimal filter amplitude estimator (default = pulse size)

  bool fUseInverseFFT; 

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
