//______________________________________________________________________
//
// KLindhard.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KLindhard.h"
#include "TF1.h"

ClassImp(KLindhard);

KLindhard::KLindhard(void)
{

  fLindhardFormula = new TF1("lindhard","[0]*pow(x,[1])",0.,1000.); //range between 0 and 1000 keV. 
  
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
  return fLindhardFormula->GetParameter(0);
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

