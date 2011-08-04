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
   static Double_t SingleEventMarginalDensityErecoil(Double_t* x,
                                                     Double_t* par);
   static Double_t SingleEventMomentErecoil(Double_t* x,
                                                   Double_t* par);
   static Double_t MultiEventMarginalDensityErecoil(Double_t* x,
                                                    Double_t* par);

   static Double_t SingleEventMarginalDensityQ(Double_t* x,
                                                    Double_t* par);
   static Double_t MultiEventMarginalDensityQ(Double_t* x,
                                                    Double_t* par);
   static Double_t SingleEventMomentQ(Double_t *x,
                                                             Double_t* par);
   static Double_t MultiEventCummulativeProbDensity(Double_t* x,Double_t* par);

  
 
  
private:
  
  ClassDef(KErecoilQDensity,1);
};


#endif // __KERECOILQDENSITY_H__