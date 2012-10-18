/*
*  KSambaRecord.h
*  KDataStructure
*
*  Created by Adam Cox on 3/25/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/

#ifndef __KSAMBARECORD_H__
#define __KSAMBARECORD_H__

#include "TObject.h"
#include <string>
#include <cstdlib>
#include "TString.h"

using namespace std;

class KSambaRecord : public TObject { 

public:
  //Constructors
  KSambaRecord(void);
  KSambaRecord(const KSambaRecord &aRec);
  KSambaRecord& operator=(const KSambaRecord &aRec);
  virtual ~KSambaRecord(void);
  virtual void Clear(Option_t *opt = "C");
  Bool_t IsSame(const KSambaRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KSambaRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KSambaRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);


  string GetRunName(void) const;
  Int_t GetSambaEventNumber(void) const {return fSambaEventNumber;}
  Int_t GetNtpDateSec(void) const {return fNtpDateSec;}
  Int_t GetNtpDateMicroSec(void) const {return fNtpDateMicroSec;}
  Int_t GetSambaDAQNumber(void) const {return fSambaDAQNumber;}
  Int_t GetFileNumber(void) const {return fFileNumber;}
  Bool_t GetRegenerationFlag(void) const { return fRegenerationFlag;}
  Int_t GetTotalDeadTimeSec(void) const { return fTotalDeadTimeSec;}
  Int_t GetTotalDeadTimeMicroSec(void) const { return fTotalDeadTimeMicroSec;}
  Double32_t GetTemperature(void) const { return fTemperature;}
  const char* GetRunType(void) const { return fRunType.c_str();}
  const char* GetRunCondition(void) const { return fRunCondition.c_str();}
  UInt_t GetRunStartPcTimeSec(void) const { return fRunStartPcTimeSec;}
  UInt_t GetRunStartPcTimeMicroSec(void) const { return fRunStartPcTimeMicroSec;}
  Long64_t GetRunStartTriggerStamp(void) const { return fRunStartTriggerStamp;}
  const char* GetSource1Regen(void) const { return fSource1Regen.c_str();}
  const char* GetSource2Regen(void) const { return fSource2Regen.c_str();}
  const char* GetSource1Calib(void) const { return fSource1Calib.c_str();}
  const char* GetSource2Calib(void) const { return fSource2Calib.c_str();}


  void SetRunName(const Char_t* aWord);
  void SetSambaEventNumber(Int_t aWord) {fSambaEventNumber = aWord;}
  void SetNtpDateSec(Int_t aWord) {fNtpDateSec = aWord;}
  void SetNtpDateMicroSec(Int_t aWord) {fNtpDateMicroSec = aWord;}
  void SetSambaDAQNumber(Int_t aWord) {fSambaDAQNumber = aWord;}
  void SetFileNumber(Short_t aVal) {fFileNumber = aVal;}
  void SetRegenerationFlag(Bool_t aVal) {fRegenerationFlag = aVal;}
  void SetTotalDeadTimeSec(Int_t aVal) {fTotalDeadTimeSec = aVal;}
  void SetTotalDeadTimeMicroSec(Int_t aVal) {fTotalDeadTimeMicroSec = aVal;}
  void SetTemperature(Double32_t aVal) {fTemperature = aVal;}
  void SetRunType(const char* aVal) {fRunType = aVal;}
  void SetRunCondition(const char* aVal) {fRunCondition = aVal;}
  void SetRunStartPcTimeSec(UInt_t aVal) {fRunStartPcTimeSec = aVal;}
  void SetRunStartPcTimeMicroSec(UInt_t aVal) {fRunStartPcTimeMicroSec = aVal;}
  void SetRunStartTriggerStamp(Long64_t aVal) {fRunStartTriggerStamp = aVal;}
  void SetSource1Regen(const char* aVal) {fSource1Regen = aVal;}
  void SetSource2Regen(const char* aVal) {fSource2Regen = aVal;}
  void SetSource1Calib(const char* aVal) {fSource1Calib = aVal;}
  void SetSource2Calib(const char* aVal) {fSource2Calib = aVal;}



  Int_t GetRunYear(void) const {return GetRunName()[0] - 'a' + 2000;}
  Int_t GetRunMonth(void) const {return GetRunName()[1] - 'a' + 1;}
  Int_t GetRunDay(void) const {return atoi(GetRunName().substr(2,2).c_str());}

  void print(void);
private:
  Int_t fSambaEventNumber;  //the event number according to the Samba NTP record
  Int_t fNtpDateSec;  //the DateSec according to the Samba NTP record
  Int_t fNtpDateMicroSec; //the microsecond timing of this DateSec according to the Samba NTP record
  Int_t fSambaDAQNumber; //this is found in the name.. a=1, b=2, c=3, etc..
  Bool_t fRegenerationFlag;
  Int_t fTotalDeadTimeSec;
  Int_t fTotalDeadTimeMicroSec;
  Double32_t fTemperature;

  string fRunName;  //run name, such as je25b000. 
  Short_t fFileNumber; //such as 000, 001, 002, ....

  //samba run header information

  string fRunType;
  string fRunCondition;
  UInt_t fRunStartPcTimeSec;
  UInt_t fRunStartPcTimeMicroSec;
  Long64_t fRunStartTriggerStamp;
  string fSource1Regen;
  string fSource2Regen;
  string fSource1Calib;
  string fSource2Calib;



  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KSambaRecord &aRec);

  ClassDef(KSambaRecord,5);
};


#endif // __KSAMBARECORD_H__
