
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
