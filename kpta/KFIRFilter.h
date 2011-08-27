//______________________________________________________________________
//
// KFIRFilter.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KFIRFILTER_H__
#define __KFIRFILTER_H__

#include "KPtaProcessor.h"
#include <vector>


class KFIRFilter : public KPtaProcessor { 
  
public:
  //Constructors
  KFIRFilter(void);
  //for the memory-savy programmers
  KFIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KFIRFilter(void);
  
	virtual bool RunProcess(void);
	
  virtual double* GetCoefficients(void){return fCoefficients;}
  virtual unsigned int GetCoefficientsSize(void){return fCoefSize;}
	
  virtual void SetCoefficients(std::vector<double> &c){SetTheCoefficients(c);}
  virtual void SetCoefficients(std::vector<float> &c){SetTheCoefficients(c);}
  virtual void SetCoefficients(std::vector<int> &c){SetTheCoefficients(c);}
  virtual void SetCoefficients(std::vector<short> &c){SetTheCoefficients(c);}

  virtual void SetCoefficients(const double* coef, unsigned int size){SetTheCoefficients(coef, size);}
  virtual void SetCoefficients(const float* coef, unsigned int size){SetTheCoefficients(coef, size);}
  virtual void SetCoefficients(const int* coef, unsigned int size){SetTheCoefficients(coef, size);}
  virtual void SetCoefficients(const short* coef, unsigned int size){SetTheCoefficients(coef, size);}
  
protected:
  template <class T> void SetTheCoefficients(std::vector<T> &coef);
  template <class T> void SetTheCoefficients(const T* coef, unsigned int size);
  
  double * fCoefficients;
  unsigned int fCoefSize;

private:  
  //private methods
  void InitializeMembers(void);
  
  //ClassDef(KFIRFilter,1);
  
};

template <class T> void KFIRFilter::SetTheCoefficients(std::vector<T> &coef)
{
  if (coef.size() != fCoefSize){
    if(fCoefficients) delete [] fCoefficients;
    fCoefSize = coef.size();
    fCoefficients = new double[fCoefSize];
  }
  for(unsigned int i = 0; i < fCoefSize; i++)
    *(fCoefficients+i) = coef[i];
}

template <class T> void KFIRFilter::SetTheCoefficients(const T* coef, unsigned int size)
{
  if (size != fCoefSize){
    if(fCoefficients) delete [] fCoefficients;
    fCoefSize = size;
    fCoefficients = new double[fCoefSize];    
  }
  for(unsigned int i = 0; i < fCoefSize; i++)
    *(fCoefficients+i) = coef[i];
    
  //std::copy(coef, coef + size, fCoefficients);
}

#endif
