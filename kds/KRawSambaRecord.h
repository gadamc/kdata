/*
 *  KRawSambaRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWSAMBARECORD_H__
#define __KRAWSAMBARECORD_H__

#include "KSambaRecord.h"
//#include "TObject.h"

class KRawSambaRecord : public KSambaRecord
{
	
public:
  //Constructors
  KRawSambaRecord(void); //MUST have a default constructor
	KRawSambaRecord(const KRawSambaRecord &aRec);
	KRawSambaRecord& operator=(const KRawSambaRecord &aRec);
  virtual ~KRawSambaRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KRawSambaRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KRawSambaRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KRawSambaRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
  void SetSambaDelay(Double32_t aVal) {fSambaDelay = aVal;  }
  void SetTriggerBit1(Int_t aVal) {fTriggerBit1 = aVal;  }
  void SetTriggerBit2(Int_t aVal) {fTriggerBit2 = aVal;  }
  void SetDataSource(const char* aVal) {fDataSource = aVal;}
  void SetDataBufferLength(Double32_t aVal) {fDataBufferLength = aVal;}
  void SetIsStreamMode(Bool_t aVal) {fStreamMode = aVal;}

  
  Double32_t GetSambaDelay(void) const { return fSambaDelay;  }
  Int_t GetTriggerBit1(void) const { return fTriggerBit1;  }
  Int_t GetTriggerBit2(void) const { return fTriggerBit2;  }
  const char* GetDataSource(void) const { return fDataSource.c_str();}
  Double32_t GetDataBufferLength(void) const { return fDataBufferLength;}
  Bool_t GetIsStreamMode(void) const { return fStreamMode;}
  
private:
	
  Double32_t fSambaDelay; 
  UInt_t fTriggerBit1; //Samba's Liste1 event trigger bit map
	UInt_t fTriggerBit2; //Samba's Liste2 event trigger bit map
  string fDataSource;
  Double32_t fDataBufferLength; //size of Samba's internal data buffer in ms
  Bool_t fStreamMode;
  
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawSambaRecord &aRec);
  ClassDef(KRawSambaRecord,2);
};


#endif // __KRAWSAMBARECORD_H__
