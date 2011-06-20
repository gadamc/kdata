/*
 *  KHLAMuonVetoSysRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLAMUONVETOSYSRECORD_H__
#define __KHLAMUONVETOSYSRECORD_H__

#include "KMuonVetoSystemRecord.h"

class KHLAMuonVetoSysRecord : public KMuonVetoSystemRecord
{ 

public:
  //Constructors
  KHLAMuonVetoSysRecord(void);
	KHLAMuonVetoSysRecord(const KHLAMuonVetoSysRecord &aSysRec);
	KHLAMuonVetoSysRecord& operator=(const KHLAMuonVetoSysRecord &aSysRec);
	virtual void Clear(Option_t *option = "C"); 
	virtual ~KHLAMuonVetoSysRecord(void);
	Bool_t IsSame(const KHLAMuonVetoSysRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLAMuonVetoSysRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLAMuonVetoSysRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLAMuonVetoSysRecord &aSysRec);
  ClassDef(KHLAMuonVetoSysRecord,2);
};


#endif // __KHLAMUONVETOSYSRECORD_H__
