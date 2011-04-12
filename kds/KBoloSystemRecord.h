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
	
	//getters
	//Double_t GetTimeSinceLastBoloSysEvent(void) const;
	//const char* GetRunName(void) const {return fRunName.c_str();}
	//Int_t GetRunYear(void) const {return fRunName[0]-'a' + 2000;}
	//Int_t GetRunMonth(void) const {return fRunName[1]-'a' + 1;}
	//Int_t GetRunDay(void) const {return atoi(fRunName.substr(2,2).c_str());}
	
	//setters
	//void SetTimeSinceLastBoloSysEvent(Double_t aNum);
	//void SetRunName(string aWord) {fRunName=aWord.substr(0,4);}  //only save the first four characters
	//of the run name since the remaining characters are mac/hour specific.

	
private:
	
	//string fRunName;  //the Bolo System Run Name (kc21, or jg14, etc...) This doesn't really make sense
	
		
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KBoloSystemRecord &aRec);

  ClassDef(KBoloSystemRecord,1);
};


#endif // __KBOLOSYSTEMRECORD_H__
