/*
 *  EdwSystemRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWSYSTEMRECORDBASE_H__
#define __EDWSYSTEMRECORDBASE_H__

#include "Rtypes.h"

class EdwSystemRecordBase  { 

public:
	//Constructors
	EdwSystemRecordBase(void);
	EdwSystemRecordBase(const EdwSystemRecordBase &aRec);
	EdwSystemRecordBase& operator=(const EdwSystemRecordBase &aRec);
	virtual ~EdwSystemRecordBase(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwSystemRecordBase &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwSystemRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwSystemRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
	
	//getters
	//Int_t GetRunNumber(void) {return fRunNumber;}
	Double_t GetRunStartTime(void) const {return fRunStartTime;}
	Double_t GetRunEndTime(void) const {return fRunEndTime;}
	Bool_t IsSystemOn(void) const {return fIsSystemOn;}	
	
	//setters
	//void SetRunNumber(Int_t aNum) {fRunNumber = aNum;}
	void SetRunStartTime(Double_t aNum) {fRunStartTime = aNum;}
	void SetRunEndTime(Double_t aNum) {fRunEndTime = aNum;}
	void SetIsSystemOn(Bool_t a) {fIsSystemOn = a;}
	
private:
	
	//Int_t fRunNumber; //run number of the system
	Double_t fRunStartTime; //start time, in unix time, for the beginning of the current system run
	Double_t fRunEndTime; //start time, in unix time, for the beginning of the current system run
	Bool_t fIsSystemOn; //set to true if the system is on.
	//Double_t fTSLSysRecord;
	
protected:
	//force the derived classes to write their own call for
	//the fTimeSinceLastSystemRecord since each derived class
	//will be of a different "type" and will want to use
	//a different name so that the analysis "user" will 
	//not be confused. See EdwMuonVetoSystemRecordBase for example
//	Double_t GetTimeSinceLastSystemRecord(void) const {return fTSLSysRecord;}
//	void SetTimeSinceLastSystemRecord(Double_t aNum) {fTSLSysRecord = aNum;}
		
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwSystemRecordBase &aRec);

	ClassDef(EdwSystemRecordBase,2);
};


#endif // __EDWSYSTEMRECORDBASE_H__
