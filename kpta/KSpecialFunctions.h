/*
 *  KSpecialFunctions.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KSPECIALFUNCTIONS_H__
#define __KSPECIALFUNCTIONS_H__

#include "Rtypes.h"

class KSpecialFunctions  { 

public:
  //Constructors
  KSpecialFunctions(void);
  virtual ~KSpecialFunctions(void);

  static double ErrF(double x);
  static double IncompleteGamma(double a, double x);
  static double GammaLn(double x);
  static void Gser(double *gamser, double a, double x, double* gln);
  static void Gcf(double *gammcf, double a, double x, double *gln);
  
private:
  ClassDef(KSpecialFunctions,1);

};


#endif // __KSPECIALFUNCTIONS_H__
