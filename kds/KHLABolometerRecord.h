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
	Double32_t GetQvalue(void) const {return fQvalue;}
	Double32_t GetEnergyRecoil(void) const {return fEnergyRecoil;}
	Double32_t GetEnergyIon(void) const {return fEnergyIon;}
	//Double32_t GetEnergyHeat(void) const {return fEnergyHeat;}
	
	Int_t GetEventFlag(void) const {return fEventFlag;}
	Int_t GetChi2Flag(void) const {return fChi2Flag;}
	const TBits* GetIonFlags(void) const {return &fIonFlags;}
	Bool_t TestIonFlag(Int_t index) const;
	
	//Int_t GetTriggerBit1(void) const {return fTriggerBit1;}
	//Int_t GetTriggerBit2(void) const {return fTriggerBit2;}
	Int_t GetVoltageFlag(void) const {return fVoltageFlag;}
	Int_t GetIonPulseTimeOffset(void) const {return fIonPulseTimeOffset;}

	//Double_t GetFiducialVolume(void) const {return fFiducialVolume;}
	
	//Int_t GetSambaRecordNum() const {return fSambaRecordNum;}
	KHLASambaRecord* GetSambaRecord(void) const {return (KHLASambaRecord*)fSambaRecord.GetObject();}
	//TRefArray* GetPulseRecordsArray(void) const {return fPulseRecords;}
	KHLABoloPulseRecord* GetPulseRecord(Int_t i) const;
	
	//vector<Int_t> GetPulseRecordNums() const {return fPulseRecordNums;}
	//Int_t GetPulseRecordNumAt(UInt_t index) const;
	//UInt_t GetNumPulseRecords(void) const {return fPulseRecordNums.size();}
	Int_t GetNumPulseRecords(void) const {return (fPulseRecords.GetEntriesFast());}
	
	/* Double32_t GetEnergyCollectrode(Int_t i) const;
	Double32_t GetEnergyVeto(Int_t i) const;
	Double32_t GetEnergyGuard(Int_t i) const;
	Double32_t GetEnergyBaselineCollectrode(Int_t i) const;
	Double32_t GetEnergyBaselineVeto(Int_t i) const;
	Double32_t GetEnergyBaselineGuard(Int_t i) const;
	Double32_t GetBaselineNoiseCollectrode(Int_t i) const;
	Double32_t GetBaselineNoiseVeto(Int_t i) const;
	Double32_t GetBaselineNoiseGuard(Int_t i) const;
	 */
	Double32_t GetEnergyIonFiducial(void) const {return fEnergyIonFiducial;}
	Double32_t GetEnergySumIonChannels(void) const {return fEnergyIonSum;}
	Double32_t GetBaselineIonFiducial(void) const {return fBaselineNoiseIonFiducial;}
	Double32_t GetBaselineNoiseIonFiducial(void) const {return GetBaselineIonFiducial();} //returns GetBaselineIonFidcuial()
	/* Double32_t GetEnergyBaselineHeat(void) const {return fEnergyBaselineHeat;}
		Double32_t GetBaselineNoiseHeat(void) const {return fBaselineNoiseHeat;}
		Double32_t GetBaselineHeat(void) const {return GetBaselineNoiseHeat();} //returns GetBaselineNoiseHeat()
	 */	
	
	Bool_t TestCutsBit(Int_t i) const;
	TBits* GetCuts(void) {return &fCuts;}
	
	//setters
	void SetQvalue(Double32_t aWord) {fQvalue = aWord;}
	void SetEnergyRecoil(Double32_t aWord) {fEnergyRecoil = aWord;}
	void SetEnergyIon(Double32_t aWord) {fEnergyIon = aWord;}
	//void SetEnergyHeat(Double32_t aWord) {fEnergyHeat = aWord;}
	
	void SetEventFlag(Int_t aWord) {fEventFlag = aWord;}
	void SetChi2Flag(Int_t aWord) {fChi2Flag = aWord;}
	Bool_t SetIonFlags(Int_t *anArray,Int_t aSize);
	void SetIonFlag(Int_t index, Bool_t value = true) {fIonFlags.SetBitNumber(index, value);}
	//void SetTriggerBit1(Int_t aWord) {fTriggerBit1 = aWord;}
	//void SetTriggerBit2(Int_t aWord) {fTriggerBit2 = aWord;}
	void SetVoltageFlag(Int_t aWord) {fVoltageFlag = aWord;}
	void SetIonPulseTimeOffset(Int_t aWord) {fIonPulseTimeOffset = aWord;}
	
	//void SetFiducialVolume(Double_t aWord) {fFiducialVolume = aWord;}
	
	//void SetSambaRecordNum(Int_t aNum){fSambaRecordNum = aNum;}
	void SetSambaRecord(KHLASambaRecord *aRec) {fSambaRecord = aRec;	}
	//void SetPulseRecordNum(Int_t aNum, UInt_t index);
	//void AddPulseRecordNum(Int_t aNum){fPulseRecordNums.push_back(aNum);}
	void AddPulseRecord(KHLABoloPulseRecord* aPulseRecord);
	/* void SetEnergyCollectrode(Int_t i, Double32_t aVal);
		void SetEnergyVeto(Int_t i, Double32_t aVal);
		void SetEnergyGuard(Int_t i, Double32_t aVal);
		void SetEnergyBaselineCollectrode(Int_t i, Double32_t aVal);
		void SetEnergyBaselineVeto(Int_t i, Double32_t aVal);
		void SetEnergyBaselineGuard(Int_t i, Double32_t aVal);
		void SetBaselineNoiseCollectrode(Int_t i, Double32_t aVal);
		void SetBaselineNoiseVeto(Int_t i, Double32_t aVal);
		void SetBaselineNoiseGuard(Int_t i, Double32_t aVal);
	 */
	void SetEnergyIonFiducial(Double32_t aVal)  { fEnergyIonFiducial = aVal;}
	void SetEnergySumIonChannels(Double32_t aVal)  { fEnergyIonSum = aVal;}
	void SetBaselineIonFiducial(Double32_t aVal)  { fBaselineNoiseIonFiducial = aVal;}
	void SetBaselineNoiseIonFiducial(Double32_t aVal)  { SetBaselineIonFiducial(aVal);}

	/* void SetEnergyBaselineHeat(Double32_t aVal)  { fEnergyBaselineHeat = aVal;}
		void SetBaselineNoiseHeat(Double32_t aVal)  { fBaselineNoiseHeat = aVal;}
		void SetBaselineHeat(Double32_t aVal){ SetBaselineNoiseHeat(aVal);}  //keep this method to support older code
	 */	
	void SetCutsBit(Int_t i, Bool_t aVal = true) {fCuts.SetBitNumber(i, aVal);}
	void SetCuts(const TBits *mCuts);
	
private:
	
	Double32_t fQvalue; //calculation based upon event category (heat, fiducial, surface, etc...)
	Double32_t fEnergyRecoil; //calculation based upon event category
	Double32_t fEnergyIon; //calculation based upon event category.
	//Double32_t fEnergyHeat; //independent of event category. based upon the heat pulse height.
	
	Int_t fEventFlag;  //0 = noise, 1 = heat only, 2 = fiducial, 3 = surface, 4 = pure guard, 5 = other.. calculated by ERA
	Int_t fChi2Flag; //set to 1 if the chi2 of all relevant channesl is okay (different from cuts.CutChi2)
	TBits fIonFlags; 
	//Int_t fTriggerBit1; //Samba's Liste1 event trigger bit map
	//Int_t fTriggerBit2; //Samba's Liste2 event trigger bit map
	Int_t fVoltageFlag; //Set to one if the polarity on this bolometer is reveresed. 
	Int_t fIonPulseTimeOffset; //the time offset of the peak of the ion pulse relative to the peak of the heat pulse, in units of stamp (10 microseonds)
	
	//Double_t fFiducialVolume;  //the value of the fiducial volume, in grams, for this detector.
	
	//Int_t fSambaRecordNum;  //This number is index of the TClonesArray holding the Samba Record for this bolometer record.
	TRef fSambaRecord;  
	
	//vector<Int_t> fPulseRecordNums;
	TRefArray fPulseRecords;
	
	//adding data from eion, eheat and cuts files from the ERA output
	//many of these values might want to be stored in boloPulse Sub record
	//at a later time. This is where they should be, logically. But, given
	//the need for analysis now, we put these data here without thinking about it
	//the FID detectors, for example, do not have a guard channel. 
	
	//eion
	/* Double32_t fEnergyCollectrode[2];  //energy of pulse on the two collectrodes (or fiducial electrodes)
		Double32_t fEnergyVeto[2]; //energy of pulse on the two veto electrodes
		Double32_t fEnergyGuard[2]; //energy of pulse on the two guard channels.
		Double32_t fEnergyBaselineCollectrode[2];  //fitted value of the energy along the baseline (at "zero time") on each collectrode
		Double32_t fEnergyBaselineVeto[2]; //fitted value of the energy along the baseline at zero time on each veto electrode
		Double32_t fEnergyBaselineGuard[2]; //fitted value of the energy along the baseline at zero time on each guard channel
		Double32_t fBaselineNoiseCollectrode[2]; //baseline noise amplitude on the collectrodes (in what units...)
		Double32_t fBaselineNoiseVeto[2]; //baseline noise amplitude on the veto channels
		Double32_t fBaselineNoiseGuard[2]; //baseline noise amplitude on the guard channels
	 */
	Double32_t fEnergyIonFiducial;  //energy of the fiducial signal (the weighted average of the fEnergyCollectrode)
	Double32_t fEnergyIonSum; //the sum of energy on all ionization channels that have a signal (calucaled by ERA)
	Double32_t fBaselineNoiseIonFiducial; //baseline noise amplitude of the fiducial signal (the weighted average of the fEnergyCollectrode)
	//Double_t fPolarFlag;  //same as fVoltageFlag. 
	
	//eheat
	/*
	Double32_t fEnergyBaselineHeat; //fitted value of the energy along the baseline (at "zero time") on the heat channel
	Double32_t fBaselineNoiseHeat; //baseline noise amplitude of the heat signal. Call GetBaselineNoiseHeat() to get this value.
	*/
	
	//cuts
	TBits fCuts;  //set the TestCutsBitNumber method for a list of which bit corresponds to which cut.
	
	//TRefArray fHLAPulseSubRecords; //an array of the HLA PulseSub Records associated with this bolo record
	//TRefArray fRawPulseSubRecords; //an array of the Raw PulseSub Records associated with this bolo record
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLABolometerRecord &aRec);
	
  ClassDef(KHLABolometerRecord,2);
};


#endif // __KHLABOLOMETERRECORD_H__
