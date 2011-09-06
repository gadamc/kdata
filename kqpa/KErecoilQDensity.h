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
#include "TF2.h"
#include "Math/WrappedMultiTF1.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "Math/GSLMCIntegrator.h"
#include "KQUncertainty.h"
#include "KNeganovLuke.h"
#include <iostream>
using namespace std;

class KErecoilQDensity {
  
public:
  KErecoilQDensity(void);
  virtual ~KErecoilQDensity(void);

   static Double_t SingleEventProbDensity(Double_t *x, Double_t * par);
   static Double_t MultiEventProbDensity(Double_t* x,Double_t* par);
   static Double_t SingleEventTrueProbDensity(Double_t* x, Double_t* par);
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
   static Double_t MultiEventProbabilityOfAtLeastOneEvent(Double_t* x,Double_t* par);
   
   static vector<Double_t> ConvoluteProbabilities(vector<Double_t> vectorA,
                                              vector<Double_t> vectorB);
   static vector<Double_t> GetDistributionOfMultipleEvents(vector<Double_t> aVector);
   static Double_t* GetDistributionOfMultipleEvents(UInt_t aSize, Double_t* aVector);
   


  
 
  
private:
  
  ClassDef(KErecoilQDensity,1);
};


#endif // __KERECOILQDENSITY_H__