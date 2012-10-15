//______________________________________________________________________
//
// KAmpBoloPulseRecord.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KAMPBOLOPULSERECORD_H__
#define __KAMPBOLOPULSERECORD_H__

#include "KBoloPulseRecord.h"
#include "TRef.h"
#include "TRefArray.h"
#include <string>
#include "KAmpBolometerRecord.h"

class KPulseAnalysisRecord;


class KAmpBoloPulseRecord : public KBoloPulseRecord { 

public:
  //Constructors
  KAmpBoloPulseRecord(void);
  KAmpBoloPulseRecord(const KAmpBoloPulseRecord &aRec);
  virtual ~KAmpBoloPulseRecord(void);
  virtual void Clear(Option_t *opt = "C");
  KAmpBoloPulseRecord& operator=(const KAmpBoloPulseRecord &aRec);
  Bool_t IsSame(const KAmpBoloPulseRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KAmpBoloPulseRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KAmpBoloPulseRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);


  const char* GetChannelName(void) const {return fChannelName.c_str();  }
  UInt_t GetPulseTimeWidth(void) const {return fPulseTimeWidth;  }
  Int_t GetPretriggerSize(void) const { return fPretriggerSize;}
  Int_t GetFilterSize(void) const { return fFilterSize;}
  UInt_t GetPulseLength(void) const { return fPulseLength;}
  Double32_t GetHeatPulseStampWidth(void) const { return fHeatPulseStampWidth;}
  Double32_t GetSampleFrequency(void) const {return 1./(GetPulseTimeWidth()*1.e-9);}
  Double32_t GetNyquistFrequency(void) const { return GetSampleFrequency()/2.;}
  Int_t GetCryoPosition(void) const { return fCryoPosition;}
  const char* GetPolarFet(void) const { return fPolarFet.c_str();}
  Double32_t GetCorrPied(void) const { return fCorrPied;}
  Double32_t GetCompModul(void) const { return fCompModul;}
  Double32_t GetCompModulation(void) const { return GetCompModul();}
  Double32_t GetCorrTrngl(void) const { return fCorrTrngl;}
  Double32_t GetAmplModul(void) const { return fAmplModul;}
  Double32_t GetAmplModulation(void) const { return GetAmplModul();}
  Bool_t GetIsHeatPulse(void) const { return fIsHeatPulse;}

  void SetChannelName(const char* name) {fChannelName = name;  } 
  void SetPulseTimeWidth(ULong_t aVal) {fPulseTimeWidth = aVal;  }
  void SetPretriggerSize(Int_t aVal) {fPretriggerSize = aVal;}
  void SetFilterSize(Int_t aVal) {fFilterSize = aVal;}
  void SetPulseLength(UInt_t aVal) {fPulseLength = aVal;}
  void SetHeatPulseStampWidth(Double32_t aVal) {fHeatPulseStampWidth = aVal;}
  void SetCryoPosition(Int_t aVal) {fCryoPosition = aVal;}
  void SetPolarFet(const char* aVal) {fPolarFet = aVal;}
  void SetCorrPied(Double32_t aVal) {fCorrPied = aVal;}
  void SetCompModul(Double32_t aVal) {fCompModul = aVal;}
  void SetCompModulation(Double32_t aVal) {SetCompModul(aVal);}
  void SetCorrTrngl(Double32_t aVal) {fCorrTrngl = aVal;}
  void SetAmplModul(Double32_t aVal) {fAmplModul = aVal;}
  void SetAmplModulation(Double32_t aVal) {SetAmplModul(aVal);}
  void SetIsHeatPulse(Bool_t aVal) {fIsHeatPulse = aVal;}

  KAmpBolometerRecord* GetBolometerRecord(void) const {return (KAmpBolometerRecord*)fBolometerRecord.GetObject();	}
  void SetBolometerRecord(KAmpBolometerRecord *aRec) {fBolometerRecord = aRec;	}

  KPulseAnalysisRecord* GetPulseAnalysisRecord(Int_t i) const;
  KPulseAnalysisRecord* GetPulseAnalysisRecord(const char* name) const;
  virtual Int_t GetNumPulseAnalysisRecords(void) const {return fPulseAnaRecords.GetEntriesFast();} 
  virtual void AddPulseAnalysisRecord(KPulseAnalysisRecord* aPulseRecord);

  TRefArray& analysisRecords() {return fPulseAnaRecords;}
private:

  TRef fBolometerRecord;  //the value of this is the pointer to KAmpBoloPulseRecord object that this pulse belongs to.

  std::string fChannelName;  //the name of the channel. for example: "chaleur ID4"
  UInt_t fPulseTimeWidth; //holds the number of ns for each point.  = 1 / f. Default is 10.080 micro sec

  //move these into a separate amplitude record
  //Double32_t fAmplitude; //the pulse amplitude calcuated by the DAQ
  //Double32_t fAmplitudeBaseline; //the baseline amplitude calculated by DAQ
  //Double32_t fPulseRiseTime;  
  //Double32_t fAmplitudeBaselineNoise; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  Int_t fPretriggerSize;
  Int_t fFilterSize;
  UInt_t fPulseLength;
  Double32_t fHeatPulseStampWidth;  //size of the heat pulse for this bolometer in stamps (same as D2 in Samba)
  Int_t fCryoPosition;
  std::string fPolarFet;

  //these are only valid for chaleur signals, but to keep the structure
  //to have just one type of pulse record (could change to have an ionization and heat pulse)
  //these value are included in all pulse records.... why aren't these in the Samba record?
  Double32_t fCorrPied;  //heat modulation parameter
  Double32_t fCompModul; //heat modulation compensation(?)
  Double32_t fCorrTrngl;  //heat modultion parameter
  Double32_t fAmplModul;  //amplitude of heat modulation
  Bool_t fIsHeatPulse;

  TRefArray fPulseAnaRecords;

  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KAmpBoloPulseRecord &aRec);

  ClassDef(KAmpBoloPulseRecord,2);
};


#endif // __KAMPBOLOPULSERECORD_H__
