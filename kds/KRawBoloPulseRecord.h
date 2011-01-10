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
#include "KRawBolometerRecord.h";
#include "TRef.h"
#include <string>

using namespace std;



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
  
  //getters
  
  string GetChannelName(void) const {return fChannelName;  }
  ULong_t GetPulseTimeWidth(void) const {return fPulseTimeWidth;  }
  vector<Short_t> GetTrace(void) const {return fTrace;  }
  Double32_t GetAmplitude(void) const {return fAmplitude;  }
  Double32_t GetAmplitudeBaseline(void) const {return fAmplitudeBaseline;  }
  Double32_t GetAmplitudeBaselineNoise(void) const {return fAmplitudeBaselineNoise;  }
  
  //setters
  
  void SetChannelName(const char* name) {fChannelName = name;  } 
  void SetPulseTimeWidth(ULong_t aVal) {fPulseTimeWidth = aVal;  }
  void SetTrace(const vector<Short_t> & aTrace) { fTrace = aTrace; }
  void SetTrace(UInt_t size, const Short_t* aData); 
  void SetAmplitude(Double32_t aVal){fAmplitude = aVal;  }
  void SetAmplitudeBaseline(Double32_t aVal){fAmplitudeBaseline = aVal;  }
  void SetAmplitudeBaselineNoise(Double32_t aVal){fAmplitudeBaselineNoise = aVal;  }

  
  KRawBolometerRecord* GetBolometerRecord(void) const {return (KRawBolometerRecord*)fBolometerRecord.GetObject();	}
	void SetBolometerRecord(KRawBolometerRecord *aRec) {fBolometerRecord = aRec;	}
  
private:

  TRef fBolometerRecord;  //the value of this is the pointer to KRawBolometerRecord object that this pulse belongs to.

  string fChannelName;  //the name of the channel. for example: "chaleur ID4"
  ULong_t fPulseTimeWidth; //returns number of ns for each point.  = 1 / f
  vector<Short_t> fTrace; //the raw trace.
  Double32_t fAmplitude; //the pulse amplitude calcuated by the DAQ
  Double32_t fAmplitudeBaseline; //the baseline amplitude calculated by DAQ
  Double32_t fAmplitudeBaselineNoise; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  
  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KRawBoloPulseRecord &aRec);

  ClassDef(KRawBoloPulseRecord,1);
};


#endif // __KRAWBOLOPULSERECORD_H__
