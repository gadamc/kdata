/*
 *  KBoloPulseRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __KBOLOPULSERECORD_H__
#define __KBOLOPULSERECORD_H__

#include "KSubRecord.h"
#include <string>
using namespace std;

enum kPulseChannelType {
	kCollectrodeType = 1,
	kVetoType = 2,
	kGuardType = 3,
	kHeatType = 4
};

class KBoloPulseRecord : public KSubRecord { 

public:
  //Constructors
  KBoloPulseRecord(void);
	KBoloPulseRecord(const KBoloPulseRecord &aRec);
	KBoloPulseRecord& operator=(const KBoloPulseRecord &aRec);
  virtual ~KBoloPulseRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KBoloPulseRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KBoloPulseRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//getters
	//Int_t GetChannelNameSize(void) const {return fChannelNameSize;}
	//string GetChannelName(void) const {string str=""; if(fChannelName==0) return str; str = fChannelName; return str;}
	//Double_t GetPeakAmp(void) const {return fPeakAmp;}
	//Double_t GetBaselineMean(void) const {return fBaselineMean;}
	//Double_t GetBaselineFWHM(void) const {return fBaselineFWHM;}
	Int_t GetPulseType(void) const {return fPulseType;}
	Int_t GetChannelNumber(void) const {return fChannelNumber;}
	
	Bool_t IsCollectrode(void) const {	return (fPulseType == kCollectrodeType) ? true : false; 	}
	Bool_t IsVeto(void) const {	return (fPulseType == kVetoType) ? true : false; 	}
	Bool_t IsGuard(void) const {	return (fPulseType == kGuardType) ? true : false; 	}
	Bool_t IsHeat(void) const {	return (fPulseType == kHeatType) ? true : false; 	}
	
	//setters
	//void SetChannelName(const Char_t *aWord, Int_t aSize);
	//void SetPeakAmp(Double_t aWord) {fPeakAmp = aWord;}
	//void SetBaselineMean(Double_t aWord) {fBaselineMean = aWord;}
	//void SetBaselineFWHM(Double_t aWord) {fBaselineFWHM = aWord;}
	void SetChannelNumber(Int_t aNum) {fChannelNumber = aNum;}
	
	void SetPulseIsCollectrodeType(void) {fPulseType = kCollectrodeType;}
	void SetPulseIsVetoType(void) {fPulseType = kVetoType;}
	void SetPulseIsGuardType(void) {fPulseType = kGuardType;}
	void SetPulseIsHeatType(void) {fPulseType = kHeatType;}

	
private:
	Int_t fChannelNumber; 
	Int_t fPulseType;  
	
	//Int_t fChannelNameSize; //number of charaters in the channel name.
	//Char_t *fChannelName; //[fChannelNameSize] Guard, Heat, A, B, etc... Currently not used.
	//Double_t fPeakAmp;  //measured by Samba (found in NTP file) - Currently not used
	//Double_t fBaselineMean; //measured by Samba (found in NTP file) - Currently not used
	//Double_t fBaselineFWHM; //measured by Samba (found in NTP file) - Currently not used

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KBoloPulseRecord &aRec);
	
  ClassDef(KBoloPulseRecord,2);
};


#endif // __KBOLOPULSERECORD_H__

