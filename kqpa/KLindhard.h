
//______________________________________________________________________
//
// KLindhard.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KLINDHARD_H__
#define __KLINDHARD_H__

#include "Rtypes.h"

class TF1;

class KLindhard  { 

public:
  //Constructors
  KLindhard(void);
  virtual ~KLindhard(void);

  TF1* GetFormula(void) const { return fLindhardFormula;  }
  Double_t GetQValue(Double_t anEnergy) const;
  Double_t GetQMeanValue(Double_t anEnergy,
                                Double_t anEnergyUncertainty) const;
  
  static Double_t GetMinDistanceToLindhard(Double_t anEnergyRecoil,Double_t aQ,
                                           Double_t aScaleEnergyRecoil = 1,
                                           Double_t aScaleQ = 1,
                                           const Char_t* aMinimizer = "GSLMultiMin",
                                           const Char_t* aMethod = "SteepestDescent",
                                           Int_t aMaxNumFunctionCalls = 100000,
                                           Int_t aMaxNumIterations = 10000,
                                           Double_t aTolerance = 0.0001);
  static Double_t GetEOfMinDistanceLindhard(Double_t anEnergyRecoil,Double_t aQ,
                                            Double_t aScaleEnergyRecoil = 1,
                                            Double_t aScaleQ = 1,
                                            const Char_t* aMinimizer = "GSLMultiMin",
                                            const Char_t* aMethod = "SteepestDescent",
                                            Int_t aMaxNumFunctionCalls = 100000,
                                            Int_t aMaxNumIterations = 10000,
                                            Double_t aTolerance = 0.0001);
  static Double_t GetArcLength(Double_t anEnergyRecoil,
                               Double_t aScaleFactor = 1,
                               Double_t aRefEnergyRecoil = 0);

  //getters
 
  Double_t GetParameterA(void) const;
  Double_t GetParameterB(void) const;
  
  //setters
  
  void SetParameterA(Double_t aVal);
  void SetParameterB(Double_t aVal);
 

private:

  TF1 *fLindhardFormula;
  

  ClassDef(KLindhard,1);
};


#endif // __KLINDHARD_H__
