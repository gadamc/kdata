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

#include "KSubRecord.h"
#include <string>
#include <cstdlib>
#include "TString.h"

using namespace std;

class KSambaRecord : public KSubRecord { 

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
	
	//getters
	
	string GetRunName(void) const;
	Int_t GetSambaEventNumber(void) const {return fSambaEventNumber;}
	Int_t GetNtpDateSec(void) const {return fNtpDateSec;}
	Int_t GetNtpDateMicroSec(void) const {return fNtpDateMicroSec;}
	Int_t GetSambaDAQNumber(void) const {return fSambaDAQNumber;}
  Int_t GetFileNumber(void) const {return fFileNumber;}

  
	//setters
	void SetRunName(const Char_t* aWord);
	void SetSambaEventNumber(Int_t aWord) {fSambaEventNumber = aWord;}
	void SetNtpDateSec(Int_t aWord) {fNtpDateSec = aWord;}
	void SetNtpDateMicroSec(Int_t aWord) {fNtpDateMicroSec = aWord;}
	void SetSambaDAQNumber(Int_t aWord) {fSambaDAQNumber = aWord;}
	void SetFileNumber(Short_t aVal) {fFileNumber = aVal;}
  
	//calculated quantities
	Int_t GetRunYear(void) const {return GetRunName()[0] - 'a' + 2000;}
	Int_t GetRunMonth(void) const {return GetRunName()[1] - 'a' + 1;}
	Int_t GetRunDay(void) const {return atoi(GetRunName().substr(2,2).c_str());}
	
	void print(void);
private:
	Int_t fSambaEventNumber;  //the event number according to the Samba NTP record
	Int_t fNtpDateSec;  //the DateSec according to the Samba NTP record
	Int_t fNtpDateMicroSec; //the microsecond timing of this DateSec according to the Samba NTP record
	Int_t fSambaDAQNumber; //this is found in the name.. a=1, b=2, c=3, etc..
		
	string fRunName;  //run name, such as je25b000. 
  Short_t fFileNumber; //such as 000, 001, 002, ....
  
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KSambaRecord &aRec);
	
  ClassDef(KSambaRecord,2);
};


#endif // __KSAMBARECORD_H__
