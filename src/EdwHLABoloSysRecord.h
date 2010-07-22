/*
 *  EdwHLABoloSysRecord.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLABOLOSYSRECORD_H__
#define __EDWHLABOLOSYSRECORD_H__

#include "EdwBoloSystemRecordBase.h"
#include "EdwHLASystemRecordBase.h"

class EdwHLABoloSysRecord  : public EdwBoloSystemRecordBase, 
public EdwHLASystemRecordBase {  

public:
  //Constructors
  EdwHLABoloSysRecord(void);
  virtual ~EdwHLABoloSysRecord(void);
	EdwHLABoloSysRecord(const EdwHLABoloSysRecord &aSysRec);
	EdwHLABoloSysRecord& operator=(const EdwHLABoloSysRecord &aSysRec);
	virtual void Clear(Option_t *option = "C");
	Bool_t IsSame(const EdwHLABoloSysRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLABoloSysRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLABoloSysRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLABoloSysRecord &aSysRec);
  ClassDef(EdwHLABoloSysRecord,1);
};


#endif // __EDWHLABOLOSYSRECORD_H__
