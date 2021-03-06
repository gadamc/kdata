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

#include "TObject.h"
#include "TRef.h"

#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include <string>

#define KPULSEANARECORD_EXTRA_SIZE 15

class KPulseAnalysisRecord : public TObject { 

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
  virtual const char* GetName(void) const { return fName.c_str();}
  virtual Double32_t GetPeakPosition(void) const { return fPeakPosition;}
  virtual Short_t GetUnit(void) const {return fUnit;}
  virtual Double32_t GetChiSq(void) const { return fChiSq;}
  virtual Double32_t GetBaselineAmplitudeWidth(void) const { return fBaselineAmplitudeWidth;}
  virtual Double32_t GetRisetime(void) const { return fRiseTime;}
  virtual Double32_t GetPulseWidth(void) const { return fPulseWidth;}
  virtual Double32_t GetSlopeRemoved(void) const {return fSlopeRemoved;}
  virtual Double32_t GetBaselineRemoved(void) const {return fBaselineRemoved;}
  virtual Double32_t GetExtra(UInt_t i) const;
  
  virtual void SetAmp(Double32_t aVal) {  fAmp = aVal;}
  virtual void SetName(const char* atype) {  fName = atype;}
  virtual void SetPeakPosition(Double32_t aval) { fPeakPosition = aval;}
  virtual void SetUnit(Short_t aval) { fUnit = aval;}
  virtual void SetChiSq(Double32_t aval) {  fChiSq =  aval;}
  virtual void SetBaselineAmplitudeWidth(Double32_t aval) {  fBaselineAmplitudeWidth =  aval;}
  virtual void SetRisetime(Double32_t aval) {  fRiseTime =  aval;}
  virtual void SetPulseWidth(Double32_t aval) {  fPulseWidth =  aval;}
  virtual void SetSlopeRemoved(Double32_t aval) {  fSlopeRemoved =  aval;}
  virtual void SetBaselineRemoved(Double32_t aval) {  fBaselineRemoved =  aval;}
  virtual void SetExtra(Double32_t aVal, UInt_t index);
  
  KAmpBolometerRecord* GetBolometerRecord(void) const {return (KAmpBolometerRecord*)fBolometerRecord.GetObject();}
  KAmpBoloPulseRecord* GetBoloPulseRecord(void) const {return (KAmpBoloPulseRecord*)fBoloPulseRecord.GetObject();}
  
  void SetBolometerRecord(KAmpBolometerRecord *aRec) {fBolometerRecord = aRec;}
  void SetBoloPulseRecord(KAmpBoloPulseRecord *aRec) {fBoloPulseRecord = aRec;}
  
  
private:
	//private methods
  
  Double32_t fAmp;  //size of the estimated pulse height
  std::string fName;  //the name of the method used to estimate the pulse characteristics. This should be a unique name. This name will be used in the database to store a document that describes this estimator.
  Double32_t fPeakPosition;  //in units of the pulse trace sample index.
  Short_t fUnit;  //0 = raw, 1 = keVee, 2 = keV
  Double32_t fChiSq; //goodness-of-fit measure
  Double32_t fBaselineAmplitudeWidth; //energy resolution at 0 keV. use this to hold the FWHM
  Double32_t fRiseTime;   //in units of seconds
  
  TRef fBolometerRecord;  //points to the appropriate bolometer record within this event
  TRef fBoloPulseRecord;  //points to the appropriate bolo pulse record
  
  //always add new member variables to the end of this list - this improves backwards compatibility
  Double32_t fPulseWidth;  //in units of seconds
  Double32_t fBaselineRemoved; //amplitude of the baseline that was corrected in the analysis
  Double32_t fSlopeRemoved; //amplitude of the slope that was removed (=0 exactly if linear removal was not performed)
  
  Double32_t fExtra[KPULSEANARECORD_EXTRA_SIZE];  //this array holds 15 floating-point numbers for the different calculations to fill as desired.
                                                  //this is a static sized array so that they are accessible from TTree::Draw and Scan
  

  //WARNING: just remember, that if you modifiy this class and add or remove variables, each time you 
  //make a change, you must increase the version number of this class
  //found in the ClassDef macro below before you record data to disk with that change. 
  //This is necessary for backwards compatibility. 
  
  
  void InitializeMembers(void);
  void CopyLocalMembers(const KPulseAnalysisRecord &aRec);
  
  ClassDef(KPulseAnalysisRecord,5);
};


#endif // __KPULSEANALYSISRECORD_H__

