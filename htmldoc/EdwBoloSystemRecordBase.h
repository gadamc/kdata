/*
 *  EdwBoloSystemRecordBase.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWBOLOSYSTEMRECORDBASE_H__
#define __EDWBOLOSYSTEMRECORDBASE_H__

#include "EdwSystemRecordBase.h"
#include <string>
#include <cstdlib>

using namespace std;

class EdwBoloSystemRecordBase : public EdwSystemRecordBase { 

public:
	//Constructors
	EdwBoloSystemRecordBase(void);
	EdwBoloSystemRecordBase(const EdwBoloSystemRecordBase &aSysRec);
	EdwBoloSystemRecordBase& operator=(const EdwBoloSystemRecordBase &aSysRec);
	virtual ~EdwBoloSystemRecordBase(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwBoloSystemRecordBase &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwBoloSystemRecordBase &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwBoloSystemRecordBase &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//getters
	//Double_t GetTimeSinceLastBoloSysEvent(void) const;
	string GetRunName(void) const {return fRunName;}
	Int_t GetRunYear(void) const {return fRunName[0]-'a' + 2000;}
	Int_t GetRunMonth(void) const {return fRunName[1]-'a' + 1;}
	Int_t GetRunDay(void) const {return atoi(fRunName.substr(2,2).c_str());}
	
	//setters
	//void SetTimeSinceLastBoloSysEvent(Double_t aNum);
	void SetRunName(string aWord) {fRunName=aWord.substr(0,4);}  //only save the first four characters
	//of the run name since the remaining characters are mac/hour specific.

	
private:
	
	string fRunName;  //the Bolo System Run Name (kc21, or jg14, etc...)
	
		
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwBoloSystemRecordBase &aRec);

  ClassDef(EdwBoloSystemRecordBase,1);
};


#endif // __EDWBOLOSYSTEMRECORDBASE_H__
