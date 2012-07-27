//______________________________________________________________________
//
// KCorrelation.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KCORRELATION_H__
#define __KCORRELATION_H__

#include "KConvolution.h"
#include <vector>


class KCorrelation : public KConvolution { 
  
public:
  //Constructors
  KCorrelation(void);
  //for the memory-savy programmers
  KCorrelation(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KCorrelation(void);

  virtual void SetResponse(std::vector<double> &r){SetTheResponse(r, true);}
  virtual void SetResponse(std::vector<float> &r){SetTheResponse(r, true);}
  virtual void SetResponse(std::vector<int> &r){SetTheResponse(r, true);}
  virtual void SetResponse(std::vector<short> &r){SetTheResponse(r, true);}

  virtual void SetResponse(const double* resp, unsigned int size){SetTheResponse(resp, size, true);}
  virtual void SetResponse(const float* resp, unsigned int size){SetTheResponse(resp, size, true);}
  virtual void SetResponse(const int* resp, unsigned int size){SetTheResponse(resp, size, true);}
  virtual void SetResponse(const short* resp, unsigned int size){SetTheResponse(resp, size, true);}
  
  
protected:
 

private:  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KCorrelation,1);
  
};
#endif
