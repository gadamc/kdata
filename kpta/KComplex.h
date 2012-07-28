//______________________________________________________________________
//
// KComplex.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#ifndef __KCOMPLEX_H__
#define __KCOMPLEX_H__
#define PI 3.14159265358979
//#include "Rtypes.h"

class KComplex {

public:
  KComplex(void);
  virtual ~KComplex(void);

  static double* GetHalfComplexMagnitude(double* input, unsigned int size); 
  static double* GetHalfComplexPhase(double* input, unsigned int size);

private:
  //ClassDef(KComplex,1);
};


#endif // __KCOMPLEX_H__

