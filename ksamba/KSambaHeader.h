//______________________________________________________________________
//
// KSambaHeader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAHEADER_H__
#define __KSAMBAHEADER_H__

#include "KSambaDetector.h";
#include "Rtypes.h"
#include "TObject.h"
#include <string>
#include <vector>

using namespace std;

class KSambaHeader : public TObject { 

public:
  //Constructors
  KSambaHeader(void);
  virtual ~KSambaHeader(void);

  const char* GetRunName(void) const { return fRunName.c_str();}
  Bool_t GetEndian(void) const { return fEndian;}
  const char* GetVersion(void) const { return fVersion.c_str();}
  const char* GetRelease(void) const { return fRelease.c_str();}
  Double_t GetEchantillonage(void) const { return fEchantillonage;}
  Int_t GetNumBolos(void) const { return fNumBolos;}
  Int_t GetNumChannels(void) const { return fNumChannels;}
  const char* GetDate(void) const { return fDate.c_str();}
  const char* GetIntitule(void) const { return fIntitule.c_str();}
  const char* GetRunType(void) const { return fRunType.c_str();}
  const char* GetRunCondition(void) const { return fRunCondition.c_str();}
  UInt_t GetRunStartPcTimeSec(void) const { return fRunStartPcTimeSec;}
  UInt_t GetRunStartPcTimeMicroSec(void) const { return fRunStartPcTimeMicroSec;}
  Long64_t GetRunStartTriggerStamp(void) const { return fRunStartTriggerStamp;}
  const char* GetSource1Regen(void) const { return fSource1Regen.c_str();}
  const char* GetSource2Regen(void) const { return fSource2Regen.c_str();}
  const char* GetSource1Calib(void) const { return fSource1Calib.c_str();}
  const char* GetSource2Calib(void) const { return fSource2Calib.c_str();}
  const char* GetDataSource(void) const { return fDataSource.c_str();}
  Double32_t GetDataBufferLength(void) const { return fDataBufferLength;}
  Bool_t GetIsStreamMode(void) const { return fStreamMode;}

  
  void SetRunName(const char* aVal) {fRunName = aVal;}
  void SetEndian(Bool_t aVal) {fEndian = aVal;}
  void SetVersion(const char* aVal) {fVersion = aVal;}
  void SetRelease(const char* aVal) {fRelease = aVal;}
  void SetEchantillonage(Double_t aVal) {fEchantillonage = aVal;}
  void SetNumBolos(Int_t aVal) {fNumBolos = aVal;}
  void SetNumChannels(Int_t aVal) {fNumChannels = aVal;}
  void SetDate(const char* aVal) {fDate = aVal;}
  void SetIntitule(const char* aVal) {fIntitule = aVal;}
  void SetRunType(const char* aVal) {fRunType = aVal;}
  void SetRunCondition(const char* aVal) {fRunCondition = aVal;}
  void SetRunStartPcTimeSec(UInt_t aVal) {fRunStartPcTimeSec = aVal;}
  void SetRunStartPcTimeMicroSec(UInt_t aVal) {fRunStartPcTimeMicroSec = aVal;}
  void SetRunStartTriggerStamp(Long64_t aVal) {fRunStartTriggerStamp = aVal;}
  void SetSource1Regen(const char* aVal) {fSource1Regen = aVal;}
  void SetSource2Regen(const char* aVal) {fSource2Regen = aVal;}
  void SetSource1Calib(const char* aVal) {fSource1Calib = aVal;}
  void SetSource2Calib(const char* aVal) {fSource2Calib = aVal;}
  void SetDataSource(const char* aVal) {fDataSource = aVal;}
  void SetDataBufferLength(Double32_t aVal) {fDataBufferLength = aVal;}
  void SetIsStreamMode(Bool_t aVal) {fStreamMode = aVal;}

  KSambaDetector* AddDetector(void);
  KSambaDetector* GetDetectorFromList(const char* detname);
  Bool_t IsInDetectorList(const char *detname);
  UInt_t GetDetectorListSize(void) const { return fDetectors.size();  }
  KSambaDetector* GetDetectorFromList(UInt_t i);
  
private:


  Bool_t fEndian; //equals true if big, false if little
  string fRunName;
  string fVersion;
  string fRelease;
  Double_t fEchantillonage;
  Int_t fNumBolos;
  Int_t fNumChannels;
  string fDate;
  string fIntitule;

  vector<KSambaDetector *> fDetectors;

  //run header information
  string fRunType;
  string fRunCondition;
  UInt_t fRunStartPcTimeSec;
  UInt_t fRunStartPcTimeMicroSec;
  Long64_t fRunStartTriggerStamp;
  string fSource1Regen;
  string fSource2Regen;
  string fSource1Calib;
  string fSource2Calib;
  string fDataSource;
  Double32_t fDataBufferLength; //size of Samba's internal data buffer in ms
  Bool_t fStreamMode;
  
  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaHeader,1);
};


#endif // __KSAMBAHEADER_H__
