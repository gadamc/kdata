/*
 *  KHLASambaSubRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLASAMBASUBRECORD_H__
#define __KHLASAMBASUBRECORD_H__

#include "KSambaSubRecord.h"
//#include "TObject.h"

class KHLASambaSubRecord : public KSambaSubRecord
{
	
public:
  //Constructors
  KHLASambaSubRecord(void); //MUST have a default constructor
	KHLASambaSubRecord(const KHLASambaSubRecord &aRec);
	KHLASambaSubRecord& operator=(const KHLASambaSubRecord &aRec);
  virtual ~KHLASambaSubRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KHLASambaSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLASambaSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLASambaSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLASambaSubRecord &aRec);
  ClassDef(KHLASambaSubRecord,1);
};


#endif // __KHLASAMBASUBRECORD_H__
