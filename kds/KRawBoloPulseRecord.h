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
#include <string>

using namespace std;

class KRawBoloPulseRecord : public KBoloPulseRecord { 

public:
  //Constructors
  KRawBoloPulseRecord(void);
  virtual ~KRawBoloPulseRecord(void);
  virtual void Clear(Option_t *opt = "C");

  //getters
  
  string GetChannelName(void) const {return fChannelName;  }
  ULong_t GetPulseTimeWidth(void) const {return fPulseTimeWidth;  }
  vector<Short_t> GetTrace(void) const {return fTrace;  }
  Double32_t GetSambaAmplitude(void) const {return fSambaAmplitude;  }
  Double32_t GetSambaBaselineAmplitude(void) const {return fSambaBaselineAmplitude;  }
  Double32_t GetSambaNoiseAmplitude(void) const {return fSambaNoiseAmplitude;  }
  
  //setters
  
  void SetChannelName(const char* name);
  void SetPulseTimeWidth(ULong_t aVal);
  void SetTrace(const vector<Short_t> & aTrace) { fTrace = aTrace; }
  void SetTrace(UShort_t n, const Short_t* aData) ; 
  void SetSambaAmplitude(Double32_t aVal){fSambaAmplitude = aVal;  }
  void SetSambaBaselineAmplitude(Double32_t aVal){fSambaBaselineAmplitude = aVal;  }
  void SetSambaNoiseAmplitude(Double32_t aVal){fSambaNoiseAmplitude = aVal;  }

private:

  string fChannelName;  //the name of the channel. for example: "chaleur ID4"
  ULong_t fPulseTimeWidth; //returns number of ns for each point.  = 1 / f
  vector<Short_t> fTrace; //the raw trace.
  Double32_t fSambaAmplitude; //the pulse amplitude calcuated by Samba
  Double32_t fSambaBaselineAmplitude; //the baseline amplitude calculated by Samba
  Double32_t fSambaNoiseAmplitude; //the width of the distribution of the baseline amplitude for this run period, calculated by Samba
  
  //private methods
  void InitializeMembers(void);

  ClassDef(KRawBoloPulseRecord,1);
};


#endif // __KRAWBOLOPULSERECORD_H__
