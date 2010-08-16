/*
 *  KHLABoloPulseRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLABOLOPULSERECORD_H__
#define __KHLABOLOPULSERECORD_H__

#include "KBoloPulseRecord.h"


class KHLABoloPulseRecord : public KBoloPulseRecord
{
	
public:
  //Constructors
  KHLABoloPulseRecord(void);
	KHLABoloPulseRecord(const KHLABoloPulseRecord &aRec);
	KHLABoloPulseRecord& operator=(const KHLABoloPulseRecord &aRec);
  virtual ~KHLABoloPulseRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KHLABoloPulseRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLABoloPulseRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLABoloPulseRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);

private:

	//derived quantites such as
	//Double32_t fPeakTime;
	//Double32_t fRiseTime;
	//Double32_t fDecayTime;
	
	//TRef fHLABolometerRecord; //the HLA SingleBolo Sub Record associated with this pulse record
	//TRef fRawPulseSubRecord; //the Raw PulseSub Record associated with this pulse record
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABoloPulseRecord &aRec);
	
  ClassDef(KHLABoloPulseRecord,1);
};


#endif // __KHLABOLOPULSERECORD_H__
