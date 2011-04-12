/*
 *  KHLASambaRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLASAMBARECORD_H__
#define __KHLASAMBARECORD_H__

#include "KSambaRecord.h"
//#include "TObject.h"

class KHLASambaRecord : public KSambaRecord
{
	
public:
  //Constructors
  KHLASambaRecord(void); //MUST have a default constructor
	KHLASambaRecord(const KHLASambaRecord &aRec);
	KHLASambaRecord& operator=(const KHLASambaRecord &aRec);
  virtual ~KHLASambaRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KHLASambaRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLASambaRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLASambaRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLASambaRecord &aRec);
  ClassDef(KHLASambaRecord,1);
};


#endif // __KHLASAMBARECORD_H__
