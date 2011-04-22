/*
 *  KSystemRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KSYSTEMRECORD_H__
#define __KSYSTEMRECORD_H__

#include "Rtypes.h"

class KSystemRecord  { 

public:
	//Constructors
	KSystemRecord(void);
	KSystemRecord(const KSystemRecord &aRec);
	KSystemRecord& operator=(const KSystemRecord &aRec);
	virtual ~KSystemRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KSystemRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KSystemRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KSystemRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void) {/*nothing to compact */};
  
  Bool_t IsSystemOn(void) const {return fIsSystemOn;}	
	void SetIsSystemOn(Bool_t a) {fIsSystemOn = a;}
	
private:
		
	Bool_t fIsSystemOn; //set to true if the system is on.
			
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KSystemRecord &aRec);

	ClassDef(KSystemRecord,1);
};


#endif // __KSYSTEMRECORD_H__
