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


  KAmpBolometerRecord* GetBolometerRecord(void) const {return (KAmpBolometerRecord*)fBolometerRecord.GetObject(); }
  void SetBolometerRecord(KAmpBolometerRecord *aRec) {fBolometerRecord = aRec;  }

  KPulseAnalysisRecord* GetPulseAnalysisRecord(Int_t i) const;
  KPulseAnalysisRecord* GetPulseAnalysisRecord(const char* name) const;
  virtual Int_t GetNumPulseAnalysisRecords(void) const {return fPulseAnaRecords.GetEntriesFast();} 
  virtual void AddPulseAnalysisRecord(KPulseAnalysisRecord* aPulseRecord);

  TRefArray& analysisRecords() {return fPulseAnaRecords;}
private:

  TRefArray fPulseAnaRecords;

  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KAmpBoloPulseRecord &aRec);

  ClassDef(KAmpBoloPulseRecord,4);
};


#endif // __KAMPBOLOPULSERECORD_H__
