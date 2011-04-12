/*
 *  KBolometerRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KBOLOMETERRECORD_H__
#define __KBOLOMETERRECORD_H__

#include "KSubRecord.h"
#include <string>

using namespace std;

class KBolometerRecord : public KSubRecord {

public:
  //Constructors
  KBolometerRecord(void);
	KBolometerRecord(const KBolometerRecord &aRec);
	KBolometerRecord& operator=(const KBolometerRecord &aRec);
  virtual ~KBolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KBolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KBolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KBolometerRecord &aRec) const { return !(*this==aRec); }
  
  Bool_t IsDetector(const char* name);
	
  virtual void Compact(void);
	
	//getters
	string GetDetectorName(void) const;
	
	//setters
	void SetDetectorName(const Char_t* aWord); 

private:
		
	string fDetectorName; 
	
	//private methods
  void InitializeMembers(void);
	void CopyLocalMembers(const KBolometerRecord &aRec);
	
  ClassDef(KBolometerRecord,1);
};


#endif // __KBOLOMETERRECORD_H__

