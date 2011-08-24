/*
 *  KPulseAnalysisRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __KPULSEANALYSISRECORD_H__
#define __KPULSEANALYSISRECORD_H__

#include "KSubRecord.h"
#include <string>
#include "TRef.h"

#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"

class KPulseAnalysisRecord : public KSubRecord { 

public:
  //Constructors
  KPulseAnalysisRecord(void);
	KPulseAnalysisRecord(const KPulseAnalysisRecord &aRec);
	KPulseAnalysisRecord& operator=(const KPulseAnalysisRecord &aRec);
  virtual ~KPulseAnalysisRecord(void);
	virtual void Clear(Option_t *anOpt = "C");
	Bool_t IsSame(const KPulseAnalysisRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KPulseAnalysisRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KPulseAnalysisRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
  virtual Double32_t GetAmp(void) const { return fAmp;}
  virtual const char* GetCalculationType(void) const { return fCalcType.c_str();}
  virtual Double32_t GetPeakPosition(void) const { return fPeakPosition;}
  virtual Bool_t IsBaseline(void) const { return fIsBaseline;}
  virtual Short_t GetUnit(void) const {return fUnit;}
  virtual Double32_t GetChiSq(void) const { return fChiSq;}
  virtual Double32_t GetBaselineAmplitudeWidth(void) const { return fBaselineAmplitudeWidth;}
  virtual Double32_t GetRisetime(void) const { return fRiseTime;}
  virtual Double32_t GetPulseWidth(void) const { return fPulseWidth;}

  virtual void SetAmp(Double32_t aVal) {  fAmp = aVal;}
  virtual void SetCalculationType(const char* atype) {  fCalcType = atype;}
  virtual void SetPeakPosition(Double32_t aval) { fPeakPosition = aval;}
  virtual void SetIsBaseline(Bool_t opt = true) {  fIsBaseline = opt;}
  virtual void SetUnit(Short_t aval) { fUnit = aval;}
  virtual void SetChiSq(Double32_t aval) {  fChiSq =  aval;}
  virtual void SetBaselineAmplitudeWidth(Double32_t aval) {  fBaselineAmplitudeWidth =  aval;}
  virtual void SetRisetime(Double32_t aval) {  fRiseTime =  aval;}
  virtual void SetPulseWidth(Double32_t aval) {  fPulseWidth =  aval;}
  
  KAmpBolometerRecord* GetBolometerRecord(void) const {return (KAmpBolometerRecord*)fBolometerRecord.GetObject();}
	KAmpBoloPulseRecord* GetBoloPulseRecord(void) const {return (KAmpBoloPulseRecord*)fBoloPulseRecord.GetObject();}
	
	void SetBolometerRecord(KAmpBolometerRecord *aRec) {fBolometerRecord = aRec;	}
	void SetBoloPulseRecord(KAmpBoloPulseRecord *aRec) {fBoloPulseRecord = aRec;	}
	
	/*
	enum kPulseAmpCalculatorType {
     kButter1 = 1,
     kOptimalFilter = 2,
     kGuardType = 3,
     kHeatType = 4
   };
   */  //lets first try a string to store the type. this is easiest and most flexible.
   
private:
	//private methods
  
  Double32_t fAmp;
  std::string fCalcType;  //the name of the method used to calculate the pulse. such as butter20, or optfilter. keep it short. if this takes up too much data, we could pack these into a numerical code.
  Double32_t fPeakPosition;  //in units of the pulse trace sample index.
  Bool_t fIsBaseline;  //if true, this is the estimated amplitude of the baseline
  Short_t fUnit;  //0 = raw, 1 = keVee, 2 = keVnr
  Double32_t fChiSq; //goodness-of-fit measure
  Double32_t fBaselineAmplitudeWidth;
  Double32_t fRiseTime;  
  
  TRef fBolometerRecord;
  TRef fBoloPulseRecord;
  
  //always add new member variables to the end of this list - this improves backwards compatibility
  Double32_t fPulseWidth;
  
  //pulse width
  //best-fit parameter results
  //shit... there are tons of other things that one may want to record. tons? or just a few?
  //or do we limit ourselves here and say 'anything else that you may want to calculate', you're on your
  //own, but you should try to do it in a "KData" way. At the very least, make a TTree that can be friended
  //to the raw KData. Given the electronics, how many different pulse shape parameters will there be? 
  //Perhaps in Eureca this could change since the TES measure fast signals, which do require a pulse shape
  //analysis
  //
  //or we just grow this class as needed to hold the data. Sure, we may end up storing some bytes of useless data
  //if a particular method doesn't calculate every member variable of this class. But overall, the data stored in 
  //these files is still going to be significantly smaller than the raw pulse records. 
  //
  //it doesn't have to be perfect - it just has to work and provide easy access for analysis groups.
  //
  //WARNING: just remember, that if you modifiy this class and add or remove variables, each time you 
  //make a change, you must increase the version number of this class
  //found in the ClassDef macro below before you record data to disk with that change. 
  //This is necessary for backwards compatibility. 
  
  
	void InitializeMembers(void);
	void CopyLocalMembers(const KPulseAnalysisRecord &aRec);
	
  ClassDef(KPulseAnalysisRecord,1);
};


#endif // __KPULSEANALYSISRECORD_H__

