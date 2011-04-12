//_______________________________________________________________________
//
// KBoloConfig.cxx
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
// Created on Thu Jan 27 14:54:11 2011
// Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
//
//
// This class stores  information  about a specific bolometer .
// Heat and ion channel baseline uncertainties and uncertainties
// for calibration energy E=356.0 keV are stored as well as channel voltages
// 
// KRun12Temp is temporarily used to read the uncertainties and channel voltages 

#include "Rtypes.h"
#include "KBoloConfig.h"
#include "string.h"

ClassImp(KBoloConfig);

KBoloConfig::KBoloConfig(const Char_t* aBoloName,const Char_t* aFileName)
{

  fBoloName = aBoloName;
  fFileName = aFileName;
}

Bool_t KBoloConfig::Load(const Char_t* aBoloName,const Char_t* aFileName) 
{
  //This method loads the bolometer configuration for bolometer "aBoloName" from ASCII file "aFileName".
  //temporary class KRun12Temp is used.
  if(fBoloName!=aBoloName && strcmp(aBoloName,""))
    fBoloName = aBoloName;
  if(fFileName!=aFileName && strcmp(aFileName,""))
    fFileName = aFileName;
  
  fEnergyCalibration=356.0;
  if(strcmp(aFileName,"")) {
    KRun12Temp aConfigFile;
    if(!aConfigFile.ReadCalibrationFile(aFileName))
      return false;
    Int_t anIndex = aConfigFile.GetCalibrationEntry(aBoloName);
    if(anIndex==-1)
      return false;
    fSigmaIonCalibration=aConfigFile.GetUncerIonCalib(anIndex);
    fSigmaHeatCalibration=aConfigFile.GetUncerHeatCalib(anIndex);
    fSigmaIonZero=aConfigFile.GetUncerIonZero(anIndex);
    fSigmaHeatZero=aConfigFile.GetUncerHeatZero(anIndex);
    fChannelVoltages.clear();
    fChannelVoltages.push_back(aConfigFile.GetVoltageBias(anIndex));
  }
  
  cout << "KBoloConfig::Load(): no file loaded" << endl;
  return false;
}
