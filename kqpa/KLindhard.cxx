//______________________________________________________________________
//
// KLindhard.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// A wrapper for a Lindhard formula, which uses a TF1.
// The Lindhard formula for the quenching factor of a nuclear recoil
// as a function of energy is
//
// Q = A * E^(B)
//
// where E is the recoil energy of the nucleus and A, and B are constants
// that are dependent upon the material. For Germanium, these values
// are A = 0.165 and B = 0.185. However, this class allows you to 
// change those values via the SetParameterA/B methods. 
//



#include "KLindhard.h"
#include "TF1.h"

ClassImp(KLindhard);

KLindhard::KLindhard(void)
{

  fLindhardFormula = new TF1("lindhard","[0]*pow(x,[1])",0.,1000.); 
  //range between 0 and 1000 keV. 
  
  //set the default values for Germanium
  SetParameterA(0.165);
  SetParameterB(0.185); 
}

KLindhard::~KLindhard(void)
{
  if(fLindhardFormula){
    delete fLindhardFormula;
    fLindhardFormula = 0;
  }

}

Double_t KLindhard::GetParameterA(void) const
{
  return fLindhardFormula->GetParameter(0);
}

Double_t KLindhard::GetParameterB(void) const
{
  return fLindhardFormula->GetParameter(1);
}

void KLindhard::SetParameterA(Double_t aVal)
{
  fLindhardFormula->SetParameter(0, aVal);
}

void KLindhard::SetParameterB(Double_t aVal)
{
  fLindhardFormula->SetParameter(1, aVal);
}

Double_t KLindhard::GetQValue(Double_t anEnergy) const
{
  return fLindhardFormula->Eval(anEnergy);
}

