/*
 *  KHLABolometerRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLABOLOMETERRECORD_H__
#define __KHLABOLOMETERRECORD_H__

#include "KBolometerRecord.h"
#include "TRef.h"
#include "TRefArray.h"
#include "TBits.h"
#include <vector>
#include "KHLASambaRecord.h"

using namespace std;

class KHLABoloPulseRecord;

//#include "TObject.h"

class KHLABolometerRecord : public KBolometerRecord
{

public:
  //Constructors
  KHLABolometerRecord(void);  //MUST have a default constructor
	KHLABolometerRecord(const KHLABolometerRecord &aRec);
	KHLABolometerRecord& operator=(const KHLABolometerRecord &aRec);
  virtual ~KHLABolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KHLABolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KHLABolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KHLABolometerRecord &aRec) const { return !(*this==aRec); }
	void Compact(void); //make the class as small as possible.

	//getters
	Double32_t GetQvalue(void) const {return fQvalue;} //calculated by ERA. depends on event type.
	Double32_t GetEnergyRecoil(void) const {return fEnergyRecoil;} //calculated by ERA. Depends upon event type
	Double32_t GetEnergyIon(void) const {return fEnergyIon;}  //calculated by ERA. Depends upon event type!


	Int_t GetEventFlag(void) const {return fEventFlag;}
  string GetEventCategory(void) const;
	Int_t GetChi2Flag(void) const {return fChi2Flag;}
	const TBits* GetIonFlags(void) const {return &fIonFlags;}
	Bool_t TestIonFlag(Int_t index) const;

	Int_t GetVoltageFlag(void) const {return fVoltageFlag;}
	Int_t GetIonPulseTimeOffset(void) const {return fIonPulseTimeOffset;}


	//Int_t GetSambaRecordNum() const {return fSambaRecordNum;}
	KHLASambaRecord* GetSambaRecord(void) const {return (KHLASambaRecord*)fSambaRecord.GetObject();}
	KHLABoloPulseRecord* GetPulseRecord(Int_t i) const;
  KHLABoloPulseRecord* GetHeatPulse(Int_t aChannel) const;
  KHLABoloPulseRecord* GetCollectrodePulse(Int_t aChannel) const;
  KHLABoloPulseRecord* GetVetoPulse(Int_t aChannel) const;
  KHLABoloPulseRecord* GetGuardPulse(Int_t aChannel) const;
  
	Int_t GetNumPulseRecords(void) const {return (fPulseRecords.GetEntriesFast());}


	Double32_t GetEnergyCollectrode(Int_t aChannel) const;
	Double32_t GetEnergyVeto(Int_t aChannel) const;
	Double32_t GetEnergyGuard(Int_t aChannel) const;
  Double32_t GetEnergyHeat(Int_t aChannel) const;

	Double32_t GetEnergyBaselineCollectrode(Int_t aChannel) const;
	Double32_t GetEnergyBaselineVeto(Int_t aChannel) const;
	Double32_t GetEnergyBaselineGuard(Int_t aChannel) const;
  Double32_t GetEnergyBaselineHeat(Int_t aChannel) const;

	Double32_t GetBaselineNoiseCollectrode(Int_t aChannel) const;
	Double32_t GetBaselineNoiseVeto(Int_t aChannel) const;
	Double32_t GetBaselineNoiseGuard(Int_t aChannel) const;
  Double32_t GetBaselineNoiseHeat(Int_t aChannel) const;


	Double32_t GetEnergyIonFiducial(void) const {return fEnergyIonFiducial;}
	Double32_t GetEnergySumIonChannels(void) const {return fEnergyIonSum;}
	Double32_t GetBaselineIonFiducial(void) const {return fBaselineNoiseIonFiducial;}
	Double32_t GetBaselineNoiseIonFiducial(void) const {return GetBaselineIonFiducial();} //returns GetBaselineIonFidcuial()


	Bool_t TestCutsBit(Int_t i) const;
	TBits* GetCuts(void) {return &fCuts;}

	//setters
	void SetQvalue(Double32_t aWord) {fQvalue = aWord;}
	void SetEnergyRecoil(Double32_t aWord) {fEnergyRecoil = aWord;}
	void SetEnergyIon(Double32_t aWord) {fEnergyIon = aWord;}

	void SetEventFlag(Int_t aWord) {fEventFlag = aWord;}
	void SetChi2Flag(Int_t aWord) {fChi2Flag = aWord;}
	Bool_t SetIonFlags(Int_t *anArray,Int_t aSize);
	void SetIonFlag(Int_t index, Bool_t value = true) {fIonFlags.SetBitNumber(index, value);}
	void SetVoltageFlag(Int_t aWord) {fVoltageFlag = aWord;}
	void SetIonPulseTimeOffset(Int_t aWord) {fIonPulseTimeOffset = aWord;}

	//void SetFiducialVolume(Double_t aWord) {fFiducialVolume = aWord;}

	void SetSambaRecord(KHLASambaRecord *aRec) {fSambaRecord = aRec;	}
	void AddPulseRecord(KHLABoloPulseRecord* aPulseRecord);

	void SetEnergyIonFiducial(Double32_t aVal)  { fEnergyIonFiducial = aVal;}
	void SetEnergySumIonChannels(Double32_t aVal)  { fEnergyIonSum = aVal;}
	void SetBaselineIonFiducial(Double32_t aVal)  { fBaselineNoiseIonFiducial = aVal;}
	void SetBaselineNoiseIonFiducial(Double32_t aVal)  { SetBaselineIonFiducial(aVal);}

	void SetCutsBit(Int_t i, Bool_t aVal = true) {fCuts.SetBitNumber(i, aVal);}
	void SetCuts(const TBits *mCuts);

private:

	Double32_t fQvalue; //calculation based upon event category (heat, fiducial, surface, etc...)
	Double32_t fEnergyRecoil; //calculation based upon event category
	Double32_t fEnergyIon; //calculation based upon event category.

	Int_t fEventFlag;  //0 = noise, 1 = heat only, 2 = fiducial, 3 = surface, 4 = pure guard, 5 = other.. calculated by ERA
	Int_t fChi2Flag; //set to 1 if the chi2 of all relevant channesl is okay (different from cuts.CutChi2)
	TBits fIonFlags; //||  //don't split
	//Int_t fTriggerBit1; //Samba's Liste1 event trigger bit map
	//Int_t fTriggerBit2; //Samba's Liste2 event trigger bit map
	Int_t fVoltageFlag; //Set to the voltage configuration value, as found in era/Scripts/params_id_run12.C .
	Int_t fIonPulseTimeOffset; //the time offset of the peak of the ion pulse relative to the peak of the heat pulse, in units of stamp (10 microseonds)

	TRef fSambaRecord;  //holds a pointer to the associated Samba record
	TRefArray fPulseRecords; //holds an array of pointers to the BoloPulse sub records associated with this bolometer event

	Double32_t fEnergyIonFiducial;  //energy of the fiducial signal (the weighted average of the fEnergyCollectrode)
	Double32_t fEnergyIonSum; //the sum of energy on all ionization channels that have a signal (calucaled by ERA)
	Double32_t fBaselineNoiseIonFiducial; //baseline noise amplitude of the fiducial signal (the weighted average of the fEnergyCollectrode)

	//cuts
	TBits fCuts;  //|| //set the TestCutsBitNumber method for a list of which bit corresponds to which cut.

	//TRefArray fRawPulseSubRecords; //an array of the Raw PulseSub Records associated with this bolo record

	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABolometerRecord &aRec);
	KHLABoloPulseRecord * GetPulseRecord(Int_t channel, Int_t aType) const;
  Double32_t GetEnergy(Int_t aChannel, Int_t aType) const;
  Double32_t GetEnergyBaseline(Int_t aChannel, Int_t aType) const;
  Double32_t GetBaselineNoise(Int_t aChannel, Int_t aType) const;

  ClassDef(KHLABolometerRecord,2);
};


#endif // __KHLABOLOMETERRECORD_H__
