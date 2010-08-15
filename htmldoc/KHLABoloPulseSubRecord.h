/*
 *  KHLABoloPulseSubRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLABOLOPULSESUBRECORD_H__
#define __KHLABOLOPULSESUBRECORD_H__

#include "KBoloPulseSubRecord.h"


class KHLABoloPulseSubRecord : public KBoloPulseSubRecord
{
	
public:
  //Constructors
  KHLABoloPulseSubRecord(void);
	KHLABoloPulseSubRecord(const KHLABoloPulseSubRecord &aRec);
	KHLABoloPulseSubRecord& operator=(const KHLABoloPulseSubRecord &aRec);
  virtual ~KHLABoloPulseSubRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KHLABoloPulseSubRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLABoloPulseSubRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLABoloPulseSubRecord &aRec) const { return !(*this==aRec); }
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
	void CopyLocalMembers(const KHLABoloPulseSubRecord &aRec);
	
  ClassDef(KHLABoloPulseSubRecord,1);
};


#endif // __KHLABOLOPULSESUBRECORD_H__
