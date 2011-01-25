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
  
  static Double_t GetQValueUncerNR(Double_t energy, Double_t ionUncerZero, Double_t ionUncerCalibEnergy,
                                   Double_t heatUncerZero, Double_t heatUncerCalibEnergy,
                                   Double_t voltageBias, 
                                   Double_t calibEnergy = 356.0, Double_t epsilon = 3.0);
  
  static Double_t GetQValueUncerEM(Double_t energy, Double_t ionUncerZero, Double_t ionUncerCalibEnergy,
                                   Double_t heatUncerZero, Double_t heatUncerCalibEnergy,
                                   Double_t voltageBias, 
                                   Double_t calibEnergy = 356.0, Double_t epsilon = 3.0);
  
  
  static Double_t GetQValueUpperBound(Double_t qValue, Double_t uncer, Double_t conflevel = 0.90);
  static Double_t GetQValueLowerBound(Double_t qValue, Double_t uncer, Double_t conflevel = 0.90);

  static Double_t GetChannelUncertainty(Double_t energy, Double_t uncerZero, 
                                 Double_t uncerCalibEnergy, Double_t calibEnergy = 356.0); //356 keV is the energy of
                                                                                   //the peak of the 133Ba source
    
  static Bool_t IsMyEventInNuclearRecoilBand(Double_t myQvalue, Double_t myEnergy, Double_t ionUncerZero,
                                      Double_t ionUncerCalibEnergy, Double_t heatUncerZero,
                                      Double_t heatUncerCalibEnergy, Double_t voltageBias,
                                      Double_t confidenceLevel = 0.90,
                                      Double_t calibEnergy = 356.0, Double_t epsilon = 3.0);
  
  static Bool_t IsMyEventInEMRecoilBand(Double_t myQvalue, Double_t myEnergy, Double_t ionUncerZero,
                                 Double_t ionUncerCalibEnergy, Double_t heatUncerZero,
                                 Double_t heatUncerCalibEnergy, Double_t voltageBias,
                                 Double_t confidenceLevel = 0.99,
                                 Double_t calibEnergy = 356.0, Double_t epsilon = 3.0);
  
private:

  
  //private methods

  ClassDef(KQUncertainty,1);
};


#endif // __KQUNCERTAINTY_H__
