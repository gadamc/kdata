/*
 *  EdwHLAMuonModuleSubRecord.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLAMUONMODULESUBRECORD_H__
#define __EDWHLAMUONMODULESUBRECORD_H__

#include "EdwMuonModuleSubRecordBase.h"
#include "EdwHLASubRecordBase.h"

class EdwHLAMuonModuleSubRecord : public EdwMuonModuleSubRecordBase,
public EdwHLASubRecordBase {
	
public:
  //Constructors
  EdwHLAMuonModuleSubRecord(void);
	EdwHLAMuonModuleSubRecord(const EdwHLAMuonModuleSubRecord &aRec);
	EdwHLAMuonModuleSubRecord& operator=(const EdwHLAMuonModuleSubRecord &aRec);
  virtual ~EdwHLAMuonModuleSubRecord(void);
	virtual void Clear(Option_t *anOpt);
	Bool_t IsSame(const EdwHLAMuonModuleSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLAMuonModuleSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLAMuonModuleSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//Danger! don't keep these inline. Need to check i for valid range.
	Double_t GetEnergy(Int_t i) const {return fEnergy[i];}
	
	void SetEnergy(Int_t i, Double_t a) {fEnergy[i] = a;}

	
private:
	
	Double_t fEnergy[kNumPmtsPerMuonModule];  //the energy deposited in both PMTs of the muon module - currently this isn't calculated :(
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLAMuonModuleSubRecord &aRec);
  ClassDef(EdwHLAMuonModuleSubRecord,1);
};


#endif // __EDWHLAMUONMODULESUBRECORD_H__
