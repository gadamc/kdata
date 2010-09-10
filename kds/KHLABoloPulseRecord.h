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
#include "KHLABolometerRecord.h"
#include "TRef.h"

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

	Double32_t GetEnergy(void) const {return fEnergy;}
	Double32_t GetEnergyBaseline(void) const {return fEnergyBaseline;}
	Double32_t GetBaselineNoise(void) const {return fBaselineNoise;}
	
	void SetEnergy(Double32_t aVal){fEnergy = aVal;}
	void SetEnergyBaseline(Double32_t aVal){fEnergyBaseline = aVal;}
	void SetBaselineNoise(Double32_t aVal){fBaselineNoise = aVal;}
	
	KHLABolometerRecord* GetBolometerRecord(void) const {return (KHLABolometerRecord*)fBolometerRecord.GetObject();	}
	void SetBolometerRecord(KHLABolometerRecord *aRec) {fBolometerRecord = aRec;	}
	
private:

	TRef fBolometerRecord;  //the value of this is the entry number of the TClonesArray that holds the KHLABolometerRecord object that this pulse belongs to.

	Double32_t fEnergy;  //the energy of the pulse in keV.
	Double32_t fEnergyBaseline; //the energy of the pulse in keV in the baseline region. 
	Double32_t fBaselineNoise; //the rms of the baseline in keV.
	
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABoloPulseRecord &aRec);
	
  ClassDef(KHLABoloPulseRecord,2);
};


#endif // __KHLABOLOPULSERECORD_H__
