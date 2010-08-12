/*
 *  EdwMuonModuleSubRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWMUONMODULESUBRECORDBASE_H__
#define __EDWMUONMODULESUBRECORDBASE_H__

#include "EdwSubRecordBase.h"

const Int_t kNumPmtsPerMuonModule = 2;

class EdwMuonModuleSubRecordBase : public EdwSubRecordBase  { 

public:
  //Constructors
  EdwMuonModuleSubRecordBase(void);
	EdwMuonModuleSubRecordBase(const EdwMuonModuleSubRecordBase &aRec);
	EdwMuonModuleSubRecordBase& operator=(const EdwMuonModuleSubRecordBase &aRec);
  virtual ~EdwMuonModuleSubRecordBase(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const EdwMuonModuleSubRecordBase &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwMuonModuleSubRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwMuonModuleSubRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	Bool_t IsNeutronCounter();
	Bool_t IsHardTrigger();  //both TDCs have values greater than -1. 
													//this doesn't check the common stop. 
	
	//getters
	Int_t GetModuleNumber(void) const {return fModuleNumber;}
	Int_t GetTdc(Int_t i) const; 
	Int_t GetAdc(Int_t i) const;
	Int_t GetAdcGroup(void) const {return fAdcGroup;}
	
	//setters
	void SetModuleNumber(Int_t aVal) {fModuleNumber = aVal;}
	Int_t SetTdc(Int_t i, Int_t aVal);
	Int_t SetAdc(Int_t i, Int_t aVal);
	void SetAdcGroup(Int_t aVal) {fAdcGroup = aVal;}
	
private:

	Int_t fModuleNumber;  //the muon module number for this sub record
	Int_t fTdc[kNumPmtsPerMuonModule];  //the TDC values recorded for this module in this event
	Int_t fAdc[kNumPmtsPerMuonModule];  //the ADC values recorded for this module
	Int_t fAdcGroup; //the ADC group number - formerly known as the Geometry.
	
	//HELP! HOW DO WE DISTINGUISH WHICH SIDE OF THE MUON MODULE 
	//CORRESPONDS TO EITHER [0] OR [1] IN THESE ARRAYS?
	//MAYBE A HOLGER KNOWS
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwMuonModuleSubRecordBase &aRec);
  ClassDef(EdwMuonModuleSubRecordBase,2);
};


#endif // __EDWMUONMODULESUBRECORDBASE_H__
