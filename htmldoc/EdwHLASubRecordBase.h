/*
 *  EdwHLASubRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLASUBRECORDBASE_H__
#define __EDWHLASUBRECORDBASE_H__

#include "Rtypes.h"

class EdwHLASubRecordBase  { 

public:
  //Constructors
  EdwHLASubRecordBase(void);
	EdwHLASubRecordBase(const EdwHLASubRecordBase &/*aRec*/){ } //nothing to do;
	EdwHLASubRecordBase& operator=(const EdwHLASubRecordBase &/*aRec*/) { return *this;} //nothing to do;
  virtual ~EdwHLASubRecordBase(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const EdwHLASubRecordBase &/*aRec*/, Bool_t /*bPrint = false*/) const { return true; }
	Bool_t operator==(const EdwHLASubRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLASubRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
	
private:
	
	//private methods
  void InitializeMembers(void);
		
  ClassDef(EdwHLASubRecordBase,1);
};


#endif // __EDWHLASUBRECORDBASE_H__
