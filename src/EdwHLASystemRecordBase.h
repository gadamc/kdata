/*
 *  EdwHLASystemRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLASYSTEMRECORDBASE_H__
#define __EDWHLASYSTEMRECORDBASE_H__

#include "Rtypes.h"

class EdwHLASystemRecordBase  { 

public:
  //Constructors
  EdwHLASystemRecordBase(void);
	EdwHLASystemRecordBase(const EdwHLASystemRecordBase &aRec);
	EdwHLASystemRecordBase& operator=(const EdwHLASystemRecordBase &aRec);
  virtual ~EdwHLASystemRecordBase(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwHLASystemRecordBase &/*aRec*/, Bool_t /*bPrint = false*/) const;
	Bool_t operator==(const EdwHLASystemRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLASystemRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
	
	/*
	Double_t GetTimeToPreviousBoloSysEvent(void){return fDtPreviousBolo;}
	Double_t GetTimeToNextBoloSysEvent(void){return fDtNextBolo;}
	Double_t GetTimeToPreviousMuonSysEvent(void){return fDtPreviousMuon;}
	Double_t GetTimeToNextMuonSysEvent(void){return fDtNextMuon;}	
	
	Int_t GetPreviousBoloGSEventNumber(void) const {return fPreviousBoloGSEventNumber;}
	Int_t GetNextBoloGSEventNumber(void) const {return fNextBoloGSEventNumber;}
	Int_t GetPreviousMuonGSEventNumber(void) const {return fPreviousMuonGSEventNumber;}
	Int_t GetNextMuonGSEventNumber(void) const {return fNextMuonGSEventNumber;}
	
	void SetTimeToPreviousBoloSysEvent(Double_t aval){fDtPreviousBolo = aval;}
	void SetTimeToNextBoloSysEvent(Double_t aval){fDtNextBolo = aval;}
	void SetTimeToPreviousMuonSysEvent(Double_t aval){fDtPreviousMuon = aval;}
	void SetTimeToNextMuonSysEvent(Double_t aval){fDtNextMuon = aval;}
	
	void SetPreviousBoloGSEventNumber(Int_t aVal) {fPreviousBoloGSEventNumber = aVal;}
	void SetNextBoloGSEventNumber(Int_t aVal) {fNextBoloGSEventNumber = aVal;}
	void SetPreviousMuonGSEventNumber(Int_t aVal) {fPreviousMuonGSEventNumber = aVal;}
	void SetNextMuonGSEventNumber(Int_t aVal) {fNextMuonGSEventNumber = aVal;}
	*/
private:

	//calculated quantities
	/*Double_t fDtPreviousBolo;
	Double_t fDtNextBolo;
	Double_t fDtPreviousMuon;
	Double_t fDtNextMuon;
	
	Int_t fPreviousBoloGSEventNumber;
	Int_t fNextBoloGSEventNumber;
	Int_t fPreviousMuonGSEventNumber;
	Int_t fNextMuonGSEventNumber;
	*/
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLASystemRecordBase &aRec);
  ClassDef(EdwHLASystemRecordBase,1);
};


#endif // __EDWHLASYSTEMRECORDBASE_H__
