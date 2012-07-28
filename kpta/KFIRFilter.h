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

#include "KConvolution.h"
#include <vector>


class KFIRFilter : public KConvolution { 
  
public:
  //Constructors
  KFIRFilter(void);
  //for the memory-savy programmers
  KFIRFilter(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KFIRFilter(void);
  
	virtual bool RunProcess(void);
	
  virtual double* GetCoefficients(void){return GetResponse();}
  virtual unsigned int GetCoefficientsSize(void){return GetResponseSize();}
	
  virtual void SetCoefficients(std::vector<double> &c){SetResponse(c);}
  virtual void SetCoefficients(std::vector<float> &c){SetResponse(c);}
  virtual void SetCoefficients(std::vector<int> &c){SetResponse(c);}
  virtual void SetCoefficients(std::vector<short> &c){SetResponse(c);}

  virtual void SetCoefficients(const double* coef, unsigned int size){SetResponse(coef, size);}
  virtual void SetCoefficients(const float* coef, unsigned int size){SetResponse(coef, size);}
  virtual void SetCoefficients(const int* coef, unsigned int size){SetResponse(coef, size);}
  virtual void SetCoefficients(const short* coef, unsigned int size){SetResponse(coef, size);}
  
protected:
 

private:  
  //private methods
  void InitializeMembers(void);
  
  //ClassDef(KFIRFilter,1);
  
};
#endif
