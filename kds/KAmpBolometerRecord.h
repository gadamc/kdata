/*
 *  KAmpBolometerRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KPAWBOLOMETERRECORD_H__
#define __KPAWBOLOMETERRECORD_H__

#include "KBolometerRecord.h"
#include "TRef.h"
#include "TRefArray.h"
#include "KRawSambaRecord.h"

class KAmpBoloPulseRecord;


//#include "TObject.h"

class KAmpBolometerRecord : public KBolometerRecord
{

public:
  //Constructors
  KAmpBolometerRecord(void);  //MUST have a default constructor
	KAmpBolometerRecord(const KAmpBolometerRecord &aRec);
	KAmpBolometerRecord& operator=(const KAmpBolometerRecord &aRec);
  virtual ~KAmpBolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KAmpBolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KAmpBolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KAmpBolometerRecord &aRec) const { return !(*this==aRec); }
	void Compact(void); //make the class as small as possible.

	KRawSambaRecord* GetSambaRecord(void) const {return (KRawSambaRecord*)fSambaRecord.GetObject();}
	KAmpBoloPulseRecord* GetPulseRecord(Int_t i) const;
  Int_t GetNumPulseRecords(void) const { return  fPulseRecords.GetEntriesFast(); }

	void SetSambaRecord(KRawSambaRecord *aRec) {fSambaRecord = aRec;	}
	void AddPulseRecord(KAmpBoloPulseRecord* aPulseRecord);

  TRefArray& pulseRecords() {return fPulseRecords;}
private:

	TRef fSambaRecord;  //holds a pointer to the associated Samba record
	TRefArray fPulseRecords; //holds an array of pointers to the BoloPulse sub records associated with this bolometer event

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KAmpBolometerRecord &aRec);


  ClassDef(KAmpBolometerRecord,2);
};


#endif // __KPAWBOLOMETERRECORD_H__
