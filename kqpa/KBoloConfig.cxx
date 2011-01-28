/*
 * KBoloConfig.cxx
 *
 * Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
 * Created on Thu Jan 27 14:54:11 2011
 * Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
 *
 *
 */

#include "Rtypes.h"
#include "KBoloConfig.h"

ClassImp(KBoloConfig);

KBoloConfig::KBoloConfig(string aBoloName,string aFileName)
{
  //loads the bolometer configuration for bolometer "aBoloName" from ASCII file "aFileName"
  //temporary class KRun12Temp is used
  Load(aBoloName,aFileName);


}

void KBoloConfig::Load(string aBoloName,string aFileName) 
{
  fBoloName = aBoloName;
  fEnergyCalibration=356.0;
  if(aFileName!="") {
  KRun12Temp aConfigFile(aFileName.c_str());
    Int_t anIndex = aConfigFile.GetCalibrationEntry(aBoloName.c_str());
    fSigmaIonCalibration=aConfigFile.GetUncerIonCalib(anIndex);
    fSigmaHeatCalibration=aConfigFile.GetUncerHeatCalib(anIndex);
    fSigmaIonZero=aConfigFile.GetUncerIonZero(anIndex);
    fSigmaHeatZero=aConfigFile.GetUncerHeatZero(anIndex);
  }
}
