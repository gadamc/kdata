/*
 *  EdwSambaSubRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWSAMBASUBRECORDBASE_H__
#define __EDWSAMBASUBRECORDBASE_H__

#include "EdwSubRecordBase.h"
#include <string>
#include <cstdlib>

using namespace std;

class EdwSambaSubRecordBase : public EdwSubRecordBase { 

public:
  //Constructors
  EdwSambaSubRecordBase(void);
	EdwSambaSubRecordBase(const EdwSambaSubRecordBase &aRec);
	EdwSambaSubRecordBase& operator=(const EdwSambaSubRecordBase &aRec);
  virtual ~EdwSambaSubRecordBase(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwSambaSubRecordBase &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwSambaSubRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwSambaSubRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//getters
	//void GetRunName(void){return fRunName;}
	//void GetFileName(void){return fFileName;}
	string GetRunName(void) const;
	Int_t GetRunNameSize(void) const {return fRunNameSize; }
	string GetFileName(void) const;
	Int_t GetFileNameSize(void) const {return fFileNameSize; }
	Int_t GetSambaEventNumber(void) const {return fSambaEventNumber;}
	Int_t GetNtpDateSec(void) const {return fNtpDateSec;}
	Int_t GetNtpDateMicroSec(void) const {return fNtpDateMicroSec;}
	Int_t GetSambaDAQNumber(void) const {return fSambaDAQNumber;}
	Int_t GetPosition(void) const {return fPosition;}
	Double_t GetDelai(void) const {return fDelai;}
	
	Int_t GetSambaMajorVersionNum(void) const {return fSambaMajorVersionNum;}
	Int_t GetSambaMinorVersionNum(void) const {return fSambaMinorVersionNum;}
	
	//setters
	void SetRunName(const Char_t* aWord, Int_t aSize);
	void SetFileName(const Char_t* aWord, Int_t aSize);
	void SetSambaEventNumber(Int_t aWord) {fSambaEventNumber = aWord;}
	void SetNtpDateSec(Int_t aWord) {fNtpDateSec = aWord;}
	void SetNtpDateMicroSec(Int_t aWord) {fNtpDateMicroSec = aWord;}
	void SetSambaDAQNumber(Int_t aWord) {fSambaDAQNumber = aWord;}
	
	void SetPosition(Int_t aWord) {fPosition = aWord;}
	void SetDelai(Double_t aWord) {fDelai = aWord;}
	
	void SetSambaMajorVersionNum(Int_t aWord) {fSambaMajorVersionNum = aWord;}
	void SetSambaMinorVersionNum(Int_t aWord) {fSambaMinorVersionNum = aWord;}

	//calculated quantities
	Int_t GetRunYear(void) const {return GetRunName()[0] - 'a' + 2000;}
	Int_t GetRunMonth(void) const {return GetRunName()[1] - 'a' + 1;}
	Int_t GetRunDay(void) const {return atoi(GetRunName().substr(2,2).c_str());}
	
	
private:
	Int_t fSambaEventNumber;  //the event number according to the Samba NTP record
	Int_t fNtpDateSec;  //the DateSec according to the Samba NTP record
	Int_t fNtpDateMicroSec; //the microsecond timing of this DateSec according to the Samba NTP record
	Int_t fSambaDAQNumber; //this is found in the name.. a=1, b=2, c=3, etc..
		
	Int_t fRunNameSize;
	Char_t *fRunName; //[fRunNameSize]  ka26b004, for example
	Int_t fFileNameSize;
	Char_t *fFileName; //[fFileNameSize] ka26b004_000, for example
	Int_t fPosition;
	Double_t fDelai; //time since last event on this mac
	
	Int_t fSambaMajorVersionNum;  //Samba version number
	Int_t fSambaMinorVersionNum;  //Samba version number
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwSambaSubRecordBase &aRec);
	
  ClassDef(EdwSambaSubRecordBase,1);
};


#endif // __EDWSAMBASUBRECORDBASE_H__
