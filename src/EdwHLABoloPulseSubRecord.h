/*
 *  EdwHLABoloPulseSubRecord.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWHLABOLOPULSESUBRECORD_H__
#define __EDWHLABOLOPULSESUBRECORD_H__

#include "EdwBoloPulseSubRecordBase.h"
#include "EdwHLASubRecordBase.h"


class EdwHLABoloPulseSubRecord : public EdwBoloPulseSubRecordBase, 
public EdwHLASubRecordBase {
	
public:
  //Constructors
  EdwHLABoloPulseSubRecord(void);
	EdwHLABoloPulseSubRecord(const EdwHLABoloPulseSubRecord &aRec);
	EdwHLABoloPulseSubRecord& operator=(const EdwHLABoloPulseSubRecord &aRec);
  virtual ~EdwHLABoloPulseSubRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const EdwHLABoloPulseSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLABoloPulseSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const EdwHLABoloPulseSubRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);

private:

	//derived quantites such as
	//Double32_t fPeakTime;
	//Double32_t fRiseTime;
	//Double32_t fDecayTime;
	
	//TRef fHLASingleBoloSubRecord; //the HLA SingleBolo Sub Record associated with this pulse record
	//TRef fRawPulseSubRecord; //the Raw PulseSub Record associated with this pulse record
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLABoloPulseSubRecord &aRec);
	
  ClassDef(EdwHLABoloPulseSubRecord,1);
};


#endif // __EDWHLABOLOPULSESUBRECORD_H__
