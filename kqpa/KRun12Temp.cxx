//___________________________________________________________________
//
//  KRun12Temp.cxx
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
// Created on Thu Jan 27 14:54:11 2011
// Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
//
//  class implementation of "readCalibrationFile.C"
//  
// This class reads the uncertainties and channel

#include "Rtypes.h"
#include "KRun12Temp.h"
#include <cstring>

ClassImp(KRun12Temp);

KRun12Temp::KRun12Temp(const Char_t* aFileName)
{
  fFileName = aFileName;
}

Bool_t KRun12Temp::ReadCalibrationFile(const Char_t* aFileName)
{
  //This method reads a bolometer configuration ASCII file of form "$name $detector_number $fwhm_ion $fwhm_ion356 $fwhm_heat $fwhm_heat356 $voltagebias $radius_fiducial"
  if(fFileName!=aFileName)
    fFileName = aFileName;
  if(fFileName!="") {
    fTree = new TTree("ConfigTree","values from file");
    Int_t aNumEntries = fTree->ReadFile(aFileName,"name/C:detector_no/I:fwhm_ion/D:\
    fwhm_ion356:fwhm_heat:fwhm_heat356:bias:r_fiducial");
    
    if(!aNumEntries) {
      cout << "KRun12Temp::ReadCalibrationFile(): file " << aFileName << 
      " does not exist or has no entries" << endl;
      return false;
    }
    return true;
  }
  
  cout << "KRun12Temp::ReadCalibrationFile(): no file specified " << endl;
  return false;
}

Int_t KRun12Temp::GetCalibrationEntry(const char* aBoloName)
{
  //This method gets the entry for the specified bolometer
  fTree->ResetBranchAddresses();
  char name[100];
  fTree->SetBranchAddress("name",name);
  for(Int_t i = 0; i < fTree->GetEntries(); i++){
    fTree->GetEntry(i);
    if(strcmp(name,aBoloName) == 0)
      return i;
      
  }
  cout << "KRun12Temp::GetCalibrationEntry(): entry for detector "
  << aBoloName << "not found" << endl;
  return -1;
}


Int_t KRun12Temp::GetDetectorNumber(Int_t anEntry)
{
  //This method gets the detector number for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Int_t aDetectorNumber;
  fTree->SetBranchAddress("detector_no",&aDetectorNumber);
  fTree->GetEntry(anEntry);
  return aDetectorNumber;
}

Double_t KRun12Temp::GetUncerIonZero(Int_t anEntry)
{
  //This method gets the baseline uncertainty of the ion channel  for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t anFWHMIon;
  fTree->SetBranchAddress("fwhm_ion",&anFWHMIon);
  fTree->GetEntry(anEntry);
  return anFWHMIon/(2 *sqrt(2 *log(2)));
  
  //return anFWHMIon;
}

Double_t KRun12Temp::GetUncerIonCalib(Int_t anEntry)
{
  //This method gets the uncertainty of the ion channel for calibration energy E=356 keV for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t anFWHMIon356;
  fTree->SetBranchAddress("fwhm_ion356",&anFWHMIon356);
  fTree->GetEntry(anEntry);
  return anFWHMIon356/(2 *sqrt(2 *log(2)));

  //return anFWHMIon356;
}

Double_t KRun12Temp::GetUncerHeatZero(Int_t anEntry)
{
  //This method gets the baseline uncertainty of the heat channel for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t anFWHMHeat;
  fTree->SetBranchAddress("fwhm_heat",&anFWHMHeat);
  fTree->GetEntry(anEntry);
  return anFWHMHeat/(2 *sqrt(2 *log(2)));

  //return anFWHMHeat;
}

Double_t KRun12Temp::GetUncerHeatCalib(Int_t anEntry)
{
  //gets the uncertainty of the heat channel for calibration energy E=356 keV for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t anFWHMHeat356;
  fTree->SetBranchAddress("fwhm_heat356",&anFWHMHeat356);
  fTree->GetEntry(anEntry);
  return anFWHMHeat356/(2 *sqrt(2 *log(2)));

  //return anFWHMHeat356;
}

Double_t KRun12Temp::GetVoltageBias(Int_t anEntry)
{
  //gets the voltage bias for the bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t aBias;
  fTree->SetBranchAddress("bias",&aBias);
  fTree->GetEntry(anEntry);
  return aBias;
}

Double_t KRun12Temp::GetRadius(Int_t anEntry)
{
  //gets the radius of bolometer corresponding to entry anEntry
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t aRadiusFiducial;
  fTree->SetBranchAddress("r_fiducial",&aRadiusFiducial);
  fTree->GetEntry(anEntry);
  return aRadiusFiducial;
}
