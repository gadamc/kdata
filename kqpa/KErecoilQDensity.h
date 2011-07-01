//______________________________________________________________________
//
// KErecoilQDensity.h
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Friday 1. July 2011
//
//
// A simple class that calculates the probability density for a true 
// value of (ERecoil,Q) for a specifed experimental value (EIon, EHeat)
// (Bayesian probabilty)
// This can be done for a single event as well as for multiple events
// As the true values of different events are supposed to be uncorrelated
// the probability density for multiple events is the product of single event
// probabilities

#ifndef __KERECOILQDENSITY_H__
#define __KERECOILQDENSITY_H__

#include "Rtypes.h"
#include "TMath.h"

class KErecoilQDensity {
  
public:
  KErecoilQDensity(void);
  virtual ~KErecoilQDensity(void);

   static Double_t SingleEventProbDensity(Double_t *x, Double_t * par);
   static Double_t MultiEventProbDensity(Double_t* x,Double_t* par);
  
 
  
private:
  
  ClassDef(KErecoilQDensity,1);
};


#endif // __KERECOILQDENSITY_H__