//______________________________________________________________________
//
// KRawBoloPulseRecord.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KRAWBOLOPULSERECORD_H__
#define __KRAWBOLOPULSERECORD_H__

#include "KBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "TRef.h"
#include <string>

using namespace std;

class TH1;
class TGraph;

class KRawBoloPulseRecord : public KBoloPulseRecord { 

public:
  //Constructors
  KRawBoloPulseRecord(void);
  KRawBoloPulseRecord(const KRawBoloPulseRecord &aRec);
  virtual ~KRawBoloPulseRecord(void);
  virtual void Clear(Option_t *opt = "C");
  KRawBoloPulseRecord& operator=(const KRawBoloPulseRecord &aRec);
  Bool_t IsSame(const KRawBoloPulseRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KRawBoloPulseRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KRawBoloPulseRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
  
  
  const char* GetChannelName(void) const {return fChannelName.c_str();  }
  UInt_t GetPulseTimeWidth(void) const {return fPulseTimeWidth;  }
  vector<Short_t> GetTrace(void) const {return fTrace;  }
  Double32_t GetAmplitude(void) const {return fAmplitude;  }
  Double32_t GetAmplitudeBaseline(void) const {return fAmplitudeBaseline;  }
  Double32_t GetAmplitudeBaselineNoise(void) const {return fAmplitudeBaselineNoise;  }
  Int_t GetPretriggerSize(void) const { return fPretriggerSize;}
  Double32_t GetPulseRiseTime(void) const { return fPulseRiseTime;}
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
  void SetTrace(const vector<Short_t> & aTrace) { fTrace = aTrace; }
  void SetTrace(UInt_t size, const Short_t* aData); 
  void SetAmplitude(Double32_t aVal){fAmplitude = aVal;  }
  void SetAmplitudeBaseline(Double32_t aVal){fAmplitudeBaseline = aVal;  }
  void SetAmplitudeBaselineNoise(Double32_t aVal){fAmplitudeBaselineNoise = aVal;  }
  void SetPretriggerSize(Int_t aVal) {fPretriggerSize = aVal;}
  void SetPulseRiseTime(Double32_t aVal) {fPulseRiseTime = aVal;}
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

  void FillHistogram(TH1 &hist);
  void FillGraph(TGraph &graph);

  
  KRawBolometerRecord* GetBolometerRecord(void) const {return (KRawBolometerRecord*)fBolometerRecord.GetObject();	}
	void SetBolometerRecord(KRawBolometerRecord *aRec) {fBolometerRecord = aRec;	}
  
private:

  TRef fBolometerRecord;  //the value of this is the pointer to KRawBolometerRecord object that this pulse belongs to.

  string fChannelName;  //the name of the channel. for example: "chaleur ID4"
  UInt_t fPulseTimeWidth; //holds the number of ns for each point.  = 1 / f. Default is 10.080 micro sec
  vector<Short_t> fTrace; //the raw trace.
  Double32_t fAmplitude; //the pulse amplitude calcuated by the DAQ
  Double32_t fAmplitudeBaseline; //the baseline amplitude calculated by DAQ
  Double32_t fAmplitudeBaselineNoise; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  Int_t fPretriggerSize;
  Double32_t fPulseRiseTime;
  Int_t fFilterSize;
  UInt_t fPulseLength;
  Double32_t fHeatPulseStampWidth;  //size of the heat pulse for this bolometer in stamps (same as D2 in Samba)
  Int_t fCryoPosition;
  string fPolarFet;
  
  //these are only valid for chaleur signals, but to keep the structure
  //to have just one type of pulse record (could change to have an ionization and heat pulse)
  //these value are included in all pulse records.... why aren't these in the Samba record?
  Double32_t fCorrPied;  //heat modulation paramter
  Double32_t fCompModul; //heat modulation compensation(?)
  Double32_t fCorrTrngl;  //heat modultion parameter
  Double32_t fAmplModul;  //amplitude of heat modulation
  Bool_t fIsHeatPulse;

  
  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KRawBoloPulseRecord &aRec);

  ClassDef(KRawBoloPulseRecord,3);
};


#endif // __KRAWBOLOPULSERECORD_H__
