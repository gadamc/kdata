/*
 *  KSubRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KSUBRECORD_H__
#define __KSUBRECORD_H__

#include "TObject.h"

class KSubRecord : public TObject { 

public:
  //Constructors
  KSubRecord(void);
	KSubRecord(const KSubRecord &/*aRec*/):TObject() { } //nothing to do. don't copy TObject attributes 
	KSubRecord& operator=(const KSubRecord &/*aRec*/) {return *this;} //nothing to do
  virtual ~KSubRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KSubRecord &/*aRec*/, Bool_t /*bPrint = false*/) const {return true;}
	Bool_t operator==(const KSubRecord &aRec) const {return IsSame(aRec,false);}
	Bool_t operator!=(const KSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
	
	//getters
	
	//setters
private:

	//private methods
	void InitializeMembers(void);
	
  ClassDef(KSubRecord,1);
};


#endif // __KSUBRECORD_H__
