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

  Int_t GetPulseType(void) const {return fPulseType;}
	Int_t GetChannelNumber(void) const {return fChannelNumber;}
	
	Bool_t IsCollectrode(void) const {	return (fPulseType == kCollectrodeType) ? true : false; 	}
	Bool_t IsVeto(void) const {	return (fPulseType == kVetoType) ? true : false; 	}
	Bool_t IsGuard(void) const {	return (fPulseType == kGuardType) ? true : false; 	}
	Bool_t IsHeat(void) const {	return (fPulseType == kHeatType) ? true : false; 	}
	
  static Int_t GetCollectrodeType(void) {return kCollectrodeType;  }
  static Int_t GetVetoType(void) {return kVetoType;  }
  static Int_t GetGuardType(void) {return kGuardType;  }
  static Int_t GetHeatType(void) {return kHeatType;  }
  
	Double32_t GetEnergy(void) const {return fEnergy;}
	Double32_t GetEnergyBaseline(void) const {return fEnergyBaseline;}
	Double32_t GetBaselineNoise(void) const {return fBaselineNoise;}
	
	void SetEnergy(Double32_t aVal){fEnergy = aVal;}
	void SetEnergyBaseline(Double32_t aVal){fEnergyBaseline = aVal;}
	void SetBaselineNoise(Double32_t aVal){fBaselineNoise = aVal;}
	
  void SetChannelNumber(Int_t aNum) {fChannelNumber = aNum;}
	
	void SetPulseIsCollectrodeType(void) {fPulseType = kCollectrodeType;}
	void SetPulseIsVetoType(void) {fPulseType = kVetoType;}
	void SetPulseIsGuardType(void) {fPulseType = kGuardType;}
	void SetPulseIsHeatType(void) {fPulseType = kHeatType;}

  
	KHLABolometerRecord* GetBolometerRecord(void) const {return (KHLABolometerRecord*)fBolometerRecord.GetObject();	}
	void SetBolometerRecord(KHLABolometerRecord *aRec) {fBolometerRecord = aRec;	}
	
  enum kPulseChannelType {
    kCollectrodeType = 1,
    kVetoType = 2,
    kGuardType = 3,
    kHeatType = 4
  };
  
private:

	TRef fBolometerRecord;  //the value of this is the pointer to KHLABolometerRecord object that this pulse belongs to.

	Double32_t fEnergy;  //the energy of the pulse in keV.
	Double32_t fEnergyBaseline; //the energy of the pulse in keV in the baseline region. 
	Double32_t fBaselineNoise; //the deviation of the distribution of baseline energy values in this data period in keV.
	
	Int_t fChannelNumber;  //the channel number of the pulse data (1 or 2) 
	Int_t fPulseType;   //a number indicating the pulse type (heat = 4, collectrode = 1, veto = 2, guard = 3)
	

  
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABoloPulseRecord &aRec);
	
  ClassDef(KHLABoloPulseRecord,3);
};


#endif // __KHLABOLOPULSERECORD_H__
