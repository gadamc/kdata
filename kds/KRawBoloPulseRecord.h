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
#include <string>

using namespace std;

class TH1;
class TH1D;
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
  
  virtual const vector<Short_t>& GetTrace(void) const {return fTrace;}  
  virtual Double32_t GetDaqAmplitude(void) const {return fDaqAmplitude;  }
  virtual Double32_t GetDaqAmplitudeBaseline(void) const {return fDaqAmplitudeBaseline;  }
  virtual Double32_t GetDaqAmplitudeBaselineNoise(void) const {return fDaqAmplitudeBaselineNoise;  }
  virtual Double32_t GetPulseRiseTime(void) const { return fPulseRiseTime;}

  virtual void SetTrace(const vector<Short_t> & aTrace) { fTrace = aTrace; }
  virtual void SetTrace(UInt_t size, const Short_t* aData); 
  virtual void SetDaqAmplitude(Double32_t aVal){fDaqAmplitude = aVal;  }
  virtual void SetDaqAmplitudeBaseline(Double32_t aVal){fDaqAmplitudeBaseline = aVal;  }
  virtual void SetDaqAmplitudeBaselineNoise(Double32_t aVal){fDaqAmplitudeBaselineNoise = aVal;  }
  virtual void SetPulseRiseTime(Double32_t aVal) {fPulseRiseTime = aVal;}


  virtual void FillHistogram(TH1 &hist);
  virtual void FillGraph(TGraph &graph);
  
  virtual TH1D* GetHist(void);
  virtual TGraph* GetGraph(void);

  KRawBolometerRecord* GetBolometerRecord(void) const {return (KRawBolometerRecord*)fBolometerRecord.GetObject();	}
	void SetBolometerRecord(KRawBolometerRecord *aRec) {fBolometerRecord = aRec;	}
  
private:

  vector<Short_t> fTrace; //the raw trace.

  Double32_t fDaqAmplitude; //the pulse amplitude calcuated by the DAQ
  Double32_t fDaqAmplitudeBaseline; //the baseline amplitude calculated by DAQ
  Double32_t fDaqAmplitudeBaselineNoise; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  Double32_t fPulseRiseTime; //rise time calculated by the DAQ


  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KRawBoloPulseRecord &aRec);
  
  ClassDef(KRawBoloPulseRecord,5);
};


#endif // __KRAWBOLOPULSERECORD_H__
