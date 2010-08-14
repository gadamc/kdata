/*
 *  KHLAMuonModuleSubRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLAMUONMODULESUBRECORD_H__
#define __KHLAMUONMODULESUBRECORD_H__

#include "KMuonModuleSubRecord.h"

class KHLAMuonModuleSubRecord : public KMuonModuleSubRecord
{
	
public:
  //Constructors
  KHLAMuonModuleSubRecord(void);
	KHLAMuonModuleSubRecord(const KHLAMuonModuleSubRecord &aRec);
	KHLAMuonModuleSubRecord& operator=(const KHLAMuonModuleSubRecord &aRec);
  virtual ~KHLAMuonModuleSubRecord(void);
	virtual void Clear(Option_t *anOpt);
	Bool_t IsSame(const KHLAMuonModuleSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLAMuonModuleSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLAMuonModuleSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//Danger! don't keep these inline. Need to check i for valid range.
	//Double_t GetEnergy(Int_t i) const {return fEnergy[i];}
	
	//void SetEnergy(Int_t i, Double_t a) {fEnergy[i] = a;}

	
private:
	
	//Double_t fEnergy[kNumPmtsPerMuonModule];  //the energy deposited in both PMTs of the muon module - currently this isn't calculated :(
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLAMuonModuleSubRecord &aRec);
  ClassDef(KHLAMuonModuleSubRecord,1);
};


#endif // __KHLAMUONMODULESUBRECORD_H__
