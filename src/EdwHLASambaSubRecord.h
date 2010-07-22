/*
 *  EdwHLASambaSubRecord.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLASAMBASUBRECORD_H__
#define __EDWHLASAMBASUBRECORD_H__

#include "EdwSambaSubRecordBase.h"
#include "EdwHLASubRecordBase.h"
//#include "TObject.h"

class EdwHLASambaSubRecord : public EdwSambaSubRecordBase,
public EdwHLASubRecordBase {
	
public:
  //Constructors
  EdwHLASambaSubRecord(void); //MUST have a default constructor
	EdwHLASambaSubRecord(const EdwHLASambaSubRecord &aRec);
	EdwHLASambaSubRecord& operator=(const EdwHLASambaSubRecord &aRec);
  virtual ~EdwHLASambaSubRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const EdwHLASambaSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLASambaSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLASambaSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLASambaSubRecord &aRec);
  ClassDef(EdwHLASambaSubRecord,1);
};


#endif // __EDWHLASAMBASUBRECORD_H__
