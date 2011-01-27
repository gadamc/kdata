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

ClassImp(KRun12Temp);

KRun12Temp::KRun12Temp()
{

}

void KRun12Temp::ReadCalibrationFile(const char* aFileName)
{
  fTree = new TTree("ConfigTree","values from file");
  fTree->ReadFile(file,"name/C:detector_no/I:fwhm_ion/D:fwhm_ion356:fwhm_heat:fwhm_heat356:bias:r_fiducial");
}

Int_t KRun12Temp::GetCalibrationEntry(const char* aBoloMeterName)
{
  fTree->ResetBranchAddresses();
  char name[100];
  fTree->SetBranchAddress("name",name);
  for(Int_t i = 0; i < t->GetEntries(); i++){
    fTree->GetEntry(i);
    if(strcmp(name,aBoloMeterName) == 0)
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
  if(anEntry > t->GetEntries()-1) return -1;
  t->ResetBranchAddresses();
  Double_t anFWHMIon;
  fTree->SetBranchAddress("fwhm_ion",&anFWDMIon);
  fTree->GetEntry(anEntry);
  return anFWHMIonValue/(2 *sqrt(2 *log(2)));
}

Double_t KRun12Temp::GetUncerIonCalib(Int_t anEntry)
{
  if(anEntry < 0) return -1;
  if(anEntry > t->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t anFWHMIon356;
  fTree->SetBranchAddress("fwhm_ion356",&anFWHMIon);
  fTree->GetEntry(anEntry);
  return anFWHMIon356/(2 *sqrt(2 *log(2)));
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
}

Double_t KRun12Temp::GetVoltageBias(Int_t anEntry)
{
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  t->ResetBranchAddresses();
  Double_t aBias;
  t->SetBranchAddress("bias",&vBias);
  t->GetEntry(anEntry);
  return aBias;
}

Double_t KRun12Temp::GetRadius(Int_t anEntry)
{
  if(anEntry < 0) return -1;
  if(anEntry > fTree->GetEntries()-1) return -1;
  fTree->ResetBranchAddresses();
  Double_t aRadiusFiducial;
  fTree->SetBranchAddress("r_fiducial",&aRadiusFiducial);
  t->GetEntry(entry);
  return val;
}
