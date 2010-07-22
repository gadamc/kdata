/*
 *  EdwBoloPulseSubRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __EDWBOLOPULSESUBRECORDBASE_H__
#define __EDWBOLOPULSESUBRECORDBASE_H__

#include "EdwSubRecordBase.h"
#include <string>
using namespace std;

class EdwBoloPulseSubRecordBase : public EdwSubRecordBase { 

public:
  //Constructors
  EdwBoloPulseSubRecordBase(void);
	EdwBoloPulseSubRecordBase(const EdwBoloPulseSubRecordBase &aRec);
	EdwBoloPulseSubRecordBase& operator=(const EdwBoloPulseSubRecordBase &aRec);
  virtual ~EdwBoloPulseSubRecordBase(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const EdwBoloPulseSubRecordBase &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwBoloPulseSubRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwBoloPulseSubRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//getters
	Int_t GetChannelNameSize(void) const {return fChannelNameSize;}
	string GetChannelName(void) const {string str=""; if(fChannelName==0) return str; str = fChannelName; return str;}
	Double_t GetPeakAmp(void) const {return fPeakAmp;}
	Double_t GetBaselineMean(void) const {return fBaselineMean;}
	Double_t GetBaselineFWHM(void) const {return fBaselineFWHM;}
	
	//setters
	void SetChannelName(const Char_t *aWord, Int_t aSize);
	void SetPeakAmp(Double_t aWord) {fPeakAmp = aWord;}
	void SetBaselineMean(Double_t aWord) {fBaselineMean = aWord;}
	void SetBaselineFWHM(Double_t aWord) {fBaselineFWHM = aWord;}
	
private:
	Int_t fChannelNameSize; //number of charaters in the channel name.
	Char_t *fChannelName; //[fChannelNameSize] Guard, Heat, A, B, etc... Currently not used.
	Double_t fPeakAmp;  //measured by Samba (found in NTP file) - Currently not used
	Double_t fBaselineMean; //measured by Samba (found in NTP file) - Currently not used
	Double_t fBaselineFWHM; //measured by Samba (found in NTP file) - Currently not used

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwBoloPulseSubRecordBase &aRec);
	
  ClassDef(EdwBoloPulseSubRecordBase,1);
};


#endif // __EDWBOLOPULSESUBRECORDBASE_H__

