/*
 *  KBoloPulseSubRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __KBOLOPULSESUBRECORD_H__
#define __KBOLOPULSESUBRECORD_H__

#include "KSubRecord.h"
#include <string>
using namespace std;

class KBoloPulseSubRecord : public KSubRecord { 

public:
  //Constructors
  KBoloPulseSubRecord(void);
	KBoloPulseSubRecord(const KBoloPulseSubRecord &aRec);
	KBoloPulseSubRecord& operator=(const KBoloPulseSubRecord &aRec);
  virtual ~KBoloPulseSubRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KBoloPulseSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KBoloPulseSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KBoloPulseSubRecord &aRec) const { return !(*this==aRec); }
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
	void CopyLocalMembers(const KBoloPulseSubRecord &aRec);
	
  ClassDef(KBoloPulseSubRecord,1);
};


#endif // __KBOLOPULSESUBRECORD_H__

