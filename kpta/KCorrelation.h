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

  template <class T> void SetResponse(std::vector<T> &resp){KConvolution::SetResponse<T>(resp,true);}
  template <class T> void SetResponse(const T* resp, unsigned int size){KConvolution::SetResponse(resp, size, true);}
  
  
protected:
 

private:  
  //private methods
  void InitializeMembers(void);
  
  //ClassDef(KCorrelation,1);
  
};
#endif
