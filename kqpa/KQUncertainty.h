//______________________________________________________________________
//
// KQUncertainty.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KQUNCERTAINTY_H__
#define __KQUNCERTAINTY_H__

#include "Rtypes.h"


class KQUncertainty  { 

public:
  //Constructors
  KQUncertainty(void);
  virtual ~KQUncertainty(void);

  static Double_t GetQValue(Double_t energy); //only for a nuclear recoil event. for electromagnetic, it is 1 by default.
  static Double_t GetChannelUncertainty(Double_t energy, Double_t uncerZero, 
                                        Double_t uncerCalibEnergy, Double_t calibEnergy = 356.0); //356 keV is the energy of
                                                                                                  //the peak of the 133Ba source
  static Double_t GetNuclRecWidth(Double_t energy, Double_t ionUncer,
                                  Double_t heatUncer, Double_t voltageBias, 
                                  Double_t epsilon = 3.0, Double_t inherentWidth = 0.04);
  
  static Double_t GetElecRecoilWidth(Double_t energy, Double_t ionUncer,
                                     Double_t heatUncer, Double_t voltageBias, 
                                     Double_t epsilon = 3.0);
  
  static Double_t GetQValueUpperBound(Double_t qValue, Double_t width, Double_t conflevel = 0.90);
  static Double_t GetQValueLowerBound(Double_t qValue, Double_t width, Double_t conflevel = 0.90);
  
  static Bool_t IsInNuclearRecoilBand(Double_t myQvalue, Double_t myEnergy, Double_t nuclRecWidth,
                                      Double_t confidenceLevel = 0.90);
  
  static Bool_t IsInNuclearRecoilBand(Double_t myQvalue, Double_t Erecoil, Double_t Eion, 
                                      Double_t Eheat, Double_t uncerIonZero, Double_t uncerIonCalib,
                                      Double_t uncerHeatZero, Double_t uncerHeatCalib,
                                      Double_t voltBias, Double_t confidenceLevel = 0.90, Double_t epsilon = 3.0,
                                      Double_t ionCalibEnergy = 356.0, Double_t heatCalibEnergy = 356.0,
                                      Double_t inherentWidth = 0.04);
  
  
  static Bool_t IsInElecRecoilBand(Double_t myQvalue, Double_t emWidth,
                                   Double_t confidenceLevel = 0.99);
  
  static Bool_t IsInElecRecoilBand(Double_t myQvalue, Double_t Erecoil, Double_t Eion, 
                                   Double_t Eheat, Double_t uncerIonZero, Double_t uncerIonCalib,
                                   Double_t uncerHeatZero, Double_t uncerHeatCalib,
                                   Double_t voltBias, Double_t confidenceLevel = 0.99, Double_t epsilon = 3.0,
                                   Double_t ionCalibEnergy = 356.0, Double_t heatCalibEnergy = 356.0);

  
private:

  
  //private methods

  ClassDef(KQUncertainty,1);
};


#endif // __KQUNCERTAINTY_H__
