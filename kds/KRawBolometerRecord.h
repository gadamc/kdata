/*
 *  KRawBolometerRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWBOLOMETERRECORD_H__
#define __KRAWBOLOMETERRECORD_H__

#include "KBolometerRecord.h"
#include "TRef.h"
#include "TRefArray.h"
#include "TBits.h"
#include <vector>
#include "KRawSambaRecord.h"

using namespace std;

class KRawBoloPulseRecord;

//#include "TObject.h"

class KRawBolometerRecord : public KBolometerRecord
{

public:
  //Constructors
  KRawBolometerRecord(void);  //MUST have a default constructor
	KRawBolometerRecord(const KRawBolometerRecord &aRec);
	KRawBolometerRecord& operator=(const KRawBolometerRecord &aRec);
  virtual ~KRawBolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KRawBolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KRawBolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KRawBolometerRecord &aRec) const { return !(*this==aRec); }
	void Compact(void); //make the class as small as possible.

	KRawSambaRecord* GetSambaRecord(void) const {return (KRawSambaRecord*)fSambaRecord.GetObject();}
	KRawBoloPulseRecord* GetPulseRecord(Int_t i) const;
  Int_t GetNumPulseRecords(void) const {return (fPulseRecords.GetEntriesFast());}

	void SetSambaRecord(KRawSambaRecord *aRec) {fSambaRecord = aRec;	}
	void AddPulseRecord(KRawBoloPulseRecord* aPulseRecord);

  TRefArray& pulseRecords(void) { return fPulseRecords;}
private:

	TRef fSambaRecord;  //holds a pointer to the associated Samba record
	TRefArray fPulseRecords; //holds an array of pointers to the BoloPulse sub records associated with this bolometer event

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawBolometerRecord &aRec);


  ClassDef(KRawBolometerRecord,2);
};


#endif // __KRAWBOLOMETERRECORD_H__
