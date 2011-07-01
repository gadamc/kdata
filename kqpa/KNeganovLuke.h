//______________________________________________________________________
//
// KNeganovLuke.h
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

#ifndef __KNEGANOVLUKE_H__
#define __KNEGANOVLUKE_H__

#include "Rtypes.h"

class KNeganovLuke {
  
public:
	KNeganovLuke(void);
	virtual ~KNeganovLuke(void);
	
  static void GetRecoilAndQValue(Double_t &q, Double_t &eRecoil, 
                          Double_t eIon, Double_t eHeat,
                          Double_t voltBias, Double_t epsilon);
  
  static void GetIonAndHeatEnergy(Double_t &eIon, Double_t &eHeat,
                           Double_t q, Double_t eRecoil,
                           Double_t voltBias, Double_t epsilon);
  
private:
	
	ClassDef(KNeganovLuke, 1);
};


#endif // __KNEGANOVLUKE_H__

