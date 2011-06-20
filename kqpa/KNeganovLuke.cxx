//______________________________________________________________________
//
// KNeganovLuke.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Sunday 30. January 2011
//
//
// A simple class that calculates the quenching value and recoil energy
// for a measured ionization energy, heat energy, voltage bias and 
// average electron-ion pair production potential in Ge (epsilon).
//
// In addition, a reverse of this function is found here. That is, given
// the recoil energy, the quenching value, the voltage bias and epsilon, 
// the energies on the ionization and heat channels are calculated.
//
// Both methods are static, so an instance of this class doesn't need
// to be created. Simply call KNeganoveLuke::GetRecoilAndQValue(...)
//
//


#include "KNeganovLuke.h"

ClassImp(KNeganovLuke);

KNeganovLuke::KNeganovLuke(void)
{
}

KNeganovLuke::~KNeganovLuke(void)
{
}

void KNeganovLuke::GetRecoilAndQValue(Double_t &q, Double_t &eRecoil,
                                        Double_t eIon, Double_t eHeat,
                                        Double_t voltBias, Double_t epsilon)
{
  //Calculates the quenching value, q, and the recoil energy 
  //given the ionization and heat energies, and the voltage bias 
  //and average electron-ion pair potential (epsilon = 3.0 V). 
  //
  //The values are assigned to q and eRecoil
  
  eRecoil = (1 + voltBias/epsilon)*eHeat  - voltBias*eIon/epsilon;
  q = eIon/eRecoil;
}

void KNeganovLuke::GetIonAndHeatEnergy(Double_t &eIon, Double_t &eHeat,
                                         Double_t q, Double_t eRecoil,
                                         Double_t voltBias, Double_t epsilon)
{
  //This method will calculation ionization and heat energies based upon the q-value,
  //the recoil energy, the voltage bias and the electron-ion pair potential (3.0 V)
  //
  //The values are assigned to eIon and eHeat
  
  eIon = q * eRecoil;
  
  eHeat = (eRecoil + voltBias*eIon/epsilon)/(1 + voltBias/epsilon); 
  
}