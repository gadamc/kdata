/*
 *  KBoloSystemRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KBOLOSYSTEMRECORD_H__
#define __KBOLOSYSTEMRECORD_H__

#include "KSystemRecord.h"
#include <string>
#include <cstdlib>

using namespace std;

class KBoloSystemRecord : public KSystemRecord { 

public:
	//Constructors
	KBoloSystemRecord(void);
	KBoloSystemRecord(const KBoloSystemRecord &aSysRec);
	KBoloSystemRecord& operator=(const KBoloSystemRecord &aSysRec);
	virtual ~KBoloSystemRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KBoloSystemRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KBoloSystemRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KBoloSystemRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
		
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KBoloSystemRecord &aRec);

  ClassDef(KBoloSystemRecord,1);
};


#endif // __KBOLOSYSTEMRECORD_H__
