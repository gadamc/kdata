/*
 *  EdwHLAMuonVetoSysRecord.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLAMUONVETOSYSRECORD_H__
#define __EDWHLAMUONVETOSYSRECORD_H__

#include "EdwMuonVetoSystemRecordBase.h"
#include "EdwHLASystemRecordBase.h"

class EdwHLAMuonVetoSysRecord : public EdwMuonVetoSystemRecordBase, 
public EdwHLASystemRecordBase { 

public:
  //Constructors
  EdwHLAMuonVetoSysRecord(void);
	EdwHLAMuonVetoSysRecord(const EdwHLAMuonVetoSysRecord &aSysRec);
	EdwHLAMuonVetoSysRecord& operator=(const EdwHLAMuonVetoSysRecord &aSysRec);
	virtual void Clear(Option_t *option = "C"); 
	virtual ~EdwHLAMuonVetoSysRecord(void);
	Bool_t IsSame(const EdwHLAMuonVetoSysRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLAMuonVetoSysRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLAMuonVetoSysRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLAMuonVetoSysRecord &aSysRec);
  ClassDef(EdwHLAMuonVetoSysRecord,1);
};


#endif // __EDWHLAMUONVETOSYSRECORD_H__
