//_____________________________________________
//
// KQLindhardProjection.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/24/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class offers the possibility to get a TH2D histogram, which contains
// the least Euclidean distances l(s) between (E_Recoil,Q) data pairs and the
// Lindhard function Q = 0.165 * E_Recoil^0.185 with respect to the arc length s
// of the Lindhard function. 
// 
// The use is analogous to KQProjection


#include "KQLindhardProjection.h"

ClassImp(KQLindhardProjection);

KQLindhardProjection::KQLindhardProjection(const Char_t* aSourceFile,
                           const Char_t* aBoloName,
                           const Char_t* aCategoryName,
                           Double_t aNumBinsEnergyRecoil,
                           Double_t anEnergyRecoilMin,
                           Double_t anEnergyRecoilMax,
                           Double_t aNumBinsQ,
                           Double_t aQMin,
                           Double_t aQMax,
                           const Char_t* aHistogramName,
                           const Char_t* aMinimizer,
                           const Char_t* aMethod)
                           : KQProjection(aSourceFile,
                                          aBoloName,
                                          aCategoryName,
                                          aNumBinsEnergyRecoil,
                                          anEnergyRecoilMin,
                                          anEnergyRecoilMax,
                                          aNumBinsQ,
                                          aQMin,
                                          aQMax,
                                          aHistogramName),
                              fMinimizer(aMinimizer),
                              fMethod(aMethod)
{
}



Bool_t KQLindhardProjection::MakeHistogram()
{
  // This class makes the histogram by filling with (s,l)-pairs for all read events,
  // where s is the arc length on the Lindhard function for the specific event
  // and l is its minimal distance to the Lindhard function in the E_Recoil-Q plane. 
  // As E_Recoil has the dimension of an energy, while Q is dimensionless,
  // in the calculations the energy inverval fEnergyRecoilMax - fEnergyRecoilMin
  // refers to a unit length
  // on the Q axis. The arc length is 0 for E_Recoil = fEnergyRecoilMin
  
  if(!fIsDataRead)
    this->ReadData(fEnergyRecoilMin,fEnergyRecoilMax);
  
    
  
  
  
  Double_t theDataX[fData.size()];
  Double_t theDataY[fData.size()];
  KLindhard aLindhard;
  
  for(Int_t k = 0; k< fData.size(); ++k) {
    theDataX[k] = KLindhard::GetArcLength(fData[k].GetEnergyRecoil(),
                                          fEnergyRecoilMax - fEnergyRecoilMin,
                                          fEnergyRecoilMin);
    theDataY[k] = KLindhard::GetMinDistanceToLindhard(fData[k].GetEnergyRecoil(),
                                                      fData[k].GetQvalue(),
                                                      fEnergyRecoilMax-fEnergyRecoilMin,
                                                      1,fMinimizer.c_str(),
                                                      fMethod.c_str(),
                                                      10000,
                                                      1000,
                                                      0.01);
    theDataY[k] *= 
    aLindhard.GetQValue(fData[k].GetEnergyRecoil())<fData[k].GetQvalue() ? 1 : -1;
  }
  
  fHistogram->SetBins(fNumBinsEnergyRecoil,
                      KLindhard::GetArcLength(fData[0].GetEnergyRecoil(),
                                              fEnergyRecoilMax-fEnergyRecoilMin,
                                              fEnergyRecoilMin),
                      KLindhard::GetArcLength(fData[fData.size()-1].GetEnergyRecoil(),
                                              fEnergyRecoilMax-fEnergyRecoilMin,
                                              fEnergyRecoilMin),
                      fNumBinsQ,fQMin,fQMax);
                                 
  for(Int_t k = 0; k< fData.size(); ++k)
    fHistogram->Fill(theDataX[k],theDataY[k]);
  
  return true;

}
