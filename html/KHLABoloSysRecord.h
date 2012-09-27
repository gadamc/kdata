/*
 *  KHLABoloSysRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLABOLOSYSRECORD_H__
#define __KHLABOLOSYSRECORD_H__

#include "KBoloSystemRecord.h"

class KHLABoloSysRecord  : public KBoloSystemRecord
{  

public:
  //Constructors
  KHLABoloSysRecord(void);
  virtual ~KHLABoloSysRecord(void);
	KHLABoloSysRecord(const KHLABoloSysRecord &aSysRec);
	KHLABoloSysRecord& operator=(const KHLABoloSysRecord &aSysRec);
	virtual void Clear(Option_t *option = "C");
	Bool_t IsSame(const KHLABoloSysRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLABoloSysRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLABoloSysRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
private:
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABoloSysRecord &aSysRec);
  ClassDef(KHLABoloSysRecord,2);
};


#endif // __KHLABOLOSYSRECORD_H__
