/*
 *  EdwSubRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWSUBRECORDBASE_H__
#define __EDWSUBRECORDBASE_H__

#include "TObject.h"

class EdwSubRecordBase : public TObject { 

public:
  //Constructors
  EdwSubRecordBase(void);
	EdwSubRecordBase(const EdwSubRecordBase &/*aRec*/):TObject() { } //nothing to do. don't copy TObject attributes 
	EdwSubRecordBase& operator=(const EdwSubRecordBase &/*aRec*/) {return *this;} //nothing to do
  virtual ~EdwSubRecordBase(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwSubRecordBase &/*aRec*/, Bool_t /*bPrint = false*/) const {return true;}
	Bool_t operator==(const EdwSubRecordBase &aRec) const {return IsSame(aRec,false);}
	Bool_t operator!=(const EdwSubRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
	
	//getters
	
	//setters
private:

	//private methods
	void InitializeMembers(void);
	
  ClassDef(EdwSubRecordBase,1);
};


#endif // __EDWSUBRECORDBASE_H__
