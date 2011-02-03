/*
 * KRun12Temp.cxx
 *
 * Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
 * Created on Thu Jan 27 14:54:11 2011
 * Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
 *
 *  class implementation of "readCalibrationFile.C"
 */

#include "Rtypes.h"
#include "KRun12Temp.h"
#include <cstring>

ClassImp(KRun12Temp);

KRun12Temp::KRun12Temp(const char* aFileName)
{
  if(aFileName != 0)
    ReadCalibrationFile(aFileName);
}

void KRun12Temp::ReadCalibrationFile(const char* aFileName)
{
  //reads a bolometer configuration ASCII file of form "$name $detector_number $fwhm_ion $fwhm_ion356 $fwhm_heat $fwhm_heat356 $voltagebias $radius_fiducial"
  if(aFileName != 0) {
    fTree = new TTree("ConfigTree","values from file");
    fTree->ReadFile(aFileName,"name/C:detector_no/I:fwhm_ion/D:fwhm_ion356:fwhm_heat:fwhm_heat356:bias:r_fiducial");
  }
}

Int_t KRun12Temp::GetCalibrationEntry(const char* aBoloName)
{
  //
  fTree->ResetBranchAddresses();
  char name[100];
  fTree->SetBranchAddress("name",name);
  for(Int_t i = 0; i < fTree->GetEntries(); i++){
    fTree->GetEntry(i);
    if(strcmp(name,aBoloName) == 0)
      return i;
      
  }
  return -1;
}


Int_t KRun12Temp::GetDetectorNumber(Int_t anEntry)
{
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
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t aRadiusFiducial;
  fTree->SetBranchAddress("r_fiducial",&aRadiusFiducial);
  fTree->GetEntry(anEntry);
  return aRadiusFiducial;
}
