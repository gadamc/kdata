//______________________________________________________________________
//
// KQUncertainty.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KQUncertainty.h"
#include "KLindhard.h"
#include "TMath.h"
#include "math.h"

ClassImp(KQUncertainty);

KQUncertainty::KQUncertainty(void)
{

}

KQUncertainty::~KQUncertainty(void)
{
  
}

Double_t KQUncertainty::GetQValue(Double_t energy)
{
  //only for a nuclear recoil event. for electromagnetic, it is 1 by default.
  
  KLindhard lind;
  return lind.GetQValue(energy);
}



Double_t KQUncertainty::GetChannelUncertainty(Double_t energy, Double_t uncerZero, 
                               Double_t uncerCalibEnergy, Double_t calibEnergy)
{
  Double_t uncZero2 = uncerZero*uncerZero;
  Double_t uncBar2 = uncerCalibEnergy*uncerCalibEnergy;
  Double_t e2 = energy*energy;
  
  return sqrt(uncZero2 + e2*(uncBar2 - uncZero2)/(calibEnergy*calibEnergy));
}


Double_t KQUncertainty::GetQValueUncerNR(Double_t energy, Double_t ionUncerZero, Double_t ionUncerCalibEnergy,
                                         Double_t heatUncerZero, Double_t heatUncerCalibEnergy,
                                         Double_t voltageBias, 
                                         Double_t calibEnergy, Double_t epsilon)
{
  if(energy == 0)
    return -999999;
  
  KLindhard lind;
  Double_t ionUncer2 = pow(GetChannelUncertainty(energy, ionUncerZero, ionUncerCalibEnergy, calibEnergy), 2);
  Double_t heatUncer2 = pow(GetChannelUncertainty(energy, heatUncerZero, heatUncerCalibEnergy, calibEnergy), 2);
  Double_t q = lind.GetQValue(energy);
  
  return sqrt( pow(1 + voltageBias * q / epsilon, 2)*ionUncer2 + pow(1 + voltageBias/epsilon, 2)*q*q*heatUncer2 ) / energy;
              
  
}

Double_t KQUncertainty::GetQValueUncerEM(Double_t energy, Double_t ionUncerZero, Double_t ionUncerCalibEnergy,
                                         Double_t heatUncerZero, Double_t heatUncerCalibEnergy,
                                         Double_t voltageBias, 
                                         Double_t calibEnergy, Double_t epsilon)
{
  if(energy == 0)
    return -999999;
  
  Double_t ionUncer2 = pow(GetChannelUncertainty(energy, ionUncerZero, ionUncerCalibEnergy, calibEnergy), 2);
  Double_t heatUncer2 = pow(GetChannelUncertainty(energy, heatUncerZero, heatUncerCalibEnergy, calibEnergy), 2);
  
  return sqrt(ionUncer2 + heatUncer2 ) * (1 + voltageBias/epsilon) / energy;
}

Double_t KQUncertainty::GetQValueUpperBound(Double_t qValue, Double_t uncer, Double_t conflevel)
{
  //At the moment, this assumes that Q is normally distributed about its mean. However, we know this 
  //is not true since Q = Eion/Erecoil and if Eion and Erecoil are guassian distributions, then the 
  //ratio of two guassian distributions is NOT a gaussian. We are currently approximating it to be a
  //gaussian, but this code should be modified.
  //
  //The Confidence level (conflevel) should be between 0 and 1. returns -1 if it is not.
  //
  //Otherwise, this function returns qValue + sqrt(2) * TMath::ErfInverse(conflevel)*uncer;
  //
  //sqrt(2) * TMath::ErfInverse(conflevel) is the number of "sigma" away from the mean for a gaussian distribution
  //
  
  if(conflevel < 0 || conflevel > 1)
    return -1;
  
  return qValue + sqrt(2) * TMath::ErfInverse(conflevel)*uncer;
}

Double_t KQUncertainty::GetQValueLowerBound(Double_t qValue, Double_t uncer, Double_t conflevel)
{
  //At the moment, this assumes that Q is normally distributed about its mean. However, we know this 
  //is not true since Q = Eion/Erecoil and if Eion and Erecoil are guassian distributions, then the 
  //ratio of two guassian distributions is NOT a gaussian. We are currently approximating it to be a
  //gaussian, but this code should be modified.
  //
  //The Confidence level (conflevel) should be between 0 and 1. returns -1 if it is not.
  //
  //Otherwise, this function returns qValue - sqrt(2) * TMath::ErfInverse(conflevel)*uncer;
  //
  //sqrt(2) * TMath::ErfInverse(conflevel) is the number of "sigma" away from the mean for a gaussian distribution
  //
  
  if(conflevel < 0 || conflevel > 1)
    return -1;

  return qValue - sqrt(2) * TMath::ErfInverse(conflevel)*uncer;
}

Bool_t KQUncertainty::IsMyEventInNuclearRecoilBand(Double_t myQvalue, Double_t myEnergy, Double_t ionUncerZero,
                                                   Double_t ionUncerCalibEnergy, Double_t heatUncerZero,
                                                   Double_t heatUncerCalibEnergy, Double_t voltageBias,
                                                   Double_t confidenceLevel,
                                                   Double_t calibEnergy, Double_t epsilon)
{
  
  //compares myQvalue to the allowed range of Q values for myEnergy and the channel energy resolution
  Double_t nucRecoilQValue = GetQValue(myEnergy); 
  Double_t nucRecoilQValueUncer = GetQValueUncerNR(myEnergy, ionUncerZero, ionUncerCalibEnergy,
                                                   heatUncerZero, heatUncerCalibEnergy,
                                                   voltageBias, calibEnergy, epsilon);
  
  
  return (myQvalue < GetQValueUpperBound(nucRecoilQValue, nucRecoilQValueUncer, confidenceLevel) && 
          myQvalue > GetQValueLowerBound(nucRecoilQValue, nucRecoilQValueUncer, confidenceLevel) );
  
}

Bool_t KQUncertainty::IsMyEventInEMRecoilBand(Double_t myQvalue, Double_t myEnergy, Double_t ionUncerZero,
                                                   Double_t ionUncerCalibEnergy, Double_t heatUncerZero,
                                                   Double_t heatUncerCalibEnergy, Double_t voltageBias,
                                                   Double_t confidenceLevel,
                                                   Double_t calibEnergy, Double_t epsilon)
{
  
  //compares myQvalue to the allowed range of Q values for myEnergy and the channel energy resolution
  Double_t emQValue = 1.0; 
  Double_t emQValueUncer = GetQValueUncerNR(myEnergy, ionUncerZero, ionUncerCalibEnergy,
                                                   heatUncerZero, heatUncerCalibEnergy,
                                                   voltageBias, calibEnergy, epsilon);
  
  
  return (myQvalue < GetQValueUpperBound(emQValue, emQValueUncer, confidenceLevel) && 
          myQvalue > GetQValueLowerBound(emQValue, emQValueUncer, confidenceLevel) );
  
}

