/*
*  KAmpEvent.h
*  KDataStructure
*
*  Created by Adam Cox on 3/24/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/

#ifndef __KAMPEVENT_H__
#define __KAMPEVENT_H__

#include "KEvent.h"
#include "KRawMuonVetoSysRecord.h"
#include "TClonesArray.h"

class KAmpBoloPulseRecord;
class KRawSambaRecord;
class KAmpBolometerRecord;
class KRawMuonModuleRecord;
class KPulseAnalysisRecord;
class KRawBolometerRecord;
class KRawBoloPulseRecord;

class KBoloPulseRecord;
class KSambaRecord;
class KBolometerRecord;
class KMuonModuleRecord;

class KAmpEvent : public KEvent {         

public:
  //Constructors and destructor. Only Derived class and the Factory
  //SHOULD access these methods. 
  //However, in order for ROOT's I/O to work properly
  //the constructor and destructor
  //have to be public. Otherwise, I would have made them protected or private
  KAmpEvent(void);
  KAmpEvent(const KAmpEvent &anEvent);
  virtual ~KAmpEvent(void);
  virtual void Clear(Option_t *opt = "C");
  void ClearArrays(Option_t *opt = "C");
  Bool_t IsSame(const KAmpEvent &anEvent, Bool_t bPrint = false) const;
  Bool_t operator==(const KAmpEvent &anEvent) const { return IsSame(anEvent,false); }
  Bool_t operator!=(const KAmpEvent &anEvent) const { return !(*this==anEvent); }
  virtual void Compact(void);
  static const char* GetClassName() {return "KAmpEvent";}
  void CreateArrays(void);

  virtual KEvent& operator=(const KEvent &anEvent);
  virtual KAmpEvent& operator=(const KAmpEvent &anEvent);

  const TClonesArray* GetSambaRecords(void) const {return fSamba;}
  const TClonesArray* GetBoloRecords(void) const {return fBolo;}
  const TClonesArray* GetBoloPulseRecords(void) const {return fBoloPulse;}
  const TClonesArray* GetMuonModuleRecords(void) const {return fMuonModule;}
  const TClonesArray* GetPulseAnalysisRecord(void) const {return fPulseAna;}
  //more pythonic looking methdos
  const TClonesArray* sambaRecords(void) const {return GetSambaRecords();}
  const TClonesArray* boloRecords(void) const {return GetBoloRecords();}
  const TClonesArray* boloPulseRecords(void) const {return GetBoloPulseRecords();}
  const TClonesArray* muonModuleRecords(void) const {return GetMuonModuleRecords();}
  
  KRawSambaRecord* AddSamba();
  KAmpBolometerRecord* AddBolo();
  KAmpBolometerRecord* AddBolo(KRawBolometerRecord* boloRaw, Bool_t force=false);
  KAmpBoloPulseRecord* AddBoloPulse();
  KAmpBoloPulseRecord* AddBoloPulse(KRawBoloPulseRecord* pRaw, KAmpBolometerRecord* boloAmp, Bool_t force=false);
  KRawMuonModuleRecord* AddMuonModule();
  KPulseAnalysisRecord* AddPulseAnalysisRecord();

  //methods required by the KEvent abstract class.
  virtual Int_t AddSubRecords(const KEvent &anEvent);
  virtual Bool_t AddMuonModuleSubRecord(const KMuonModuleRecord &inMuonModule);
  virtual Bool_t AddBoloSubRecord(const KBolometerRecord &inBolo);

  virtual Int_t GetNumSambas(void) const {return (fSamba) ? fSamba->GetEntriesFast() : 0;}
  virtual Int_t GetNumBolos(void) const {return (fBolo) ? fBolo->GetEntriesFast() : 0;;}
  virtual Int_t GetNumBoloPulses(void) const {return (fBoloPulse) ? fBoloPulse->GetEntriesFast() : 0;} 
  virtual Int_t GetNumMuonModules(void) const {return (fMuonModule) ? fMuonModule->GetEntriesFast() : 0;} 
  virtual Int_t GetNumPulseAnalysisRecords(void) const {return (fPulseAna) ? fPulseAna->GetEntriesFast() : 0;} 

  virtual KMuonVetoSystemRecord* GetMuonVetoSystemRecord(void) {KMuonVetoSystemRecord *s = static_cast<KMuonVetoSystemRecord *>(&fMuonSystem); return s;}

  virtual KSambaRecord* GetSamba(Int_t i) const;
  virtual KBolometerRecord* GetBolo(Int_t i) const;
  virtual KBoloPulseRecord* GetBoloPulse(Int_t i) const;
  virtual KMuonModuleRecord* GetMuonModule(Int_t i) const;
  virtual KPulseAnalysisRecord* GetPulseAnalysisRecord(Int_t i) const;
  virtual KBolometerRecord* GetBolo(const char* name) const;
  virtual KBoloPulseRecord* GetBoloPulse(const char* name) const;
private: 

  KRawMuonVetoSysRecord fMuonSystem; //Muon Veto System Record

  //the '//->' comment tells ROOT that this pointer will never be NULL

  TClonesArray *fSamba; //->  an array of samba records
  TClonesArray *fBolo; //-> an array of bolometer records
  TClonesArray *fBoloPulse; //-> an array of pulse record
  TClonesArray *fMuonModule; //-> an array of muon module records
  TClonesArray *fPulseAna; //-> an array of pulse analysis, each calculated by a different method

  void InitializeMembers(void);
  void CopyLocalMembers(const KAmpEvent &anEvent);
  void CopyClonesArrays(const KAmpEvent &anEvent);

  template<class T> T* AddSubRecord(TClonesArray *mArray);
  void ClearArray(Option_t *anOpt, TClonesArray *mArray);

  UInt_t GetLargestUniqueIDNumber(void);

  ClassDef(KAmpEvent ,1);
};

template<class T> T* KAmpEvent::AddSubRecord(TClonesArray *mArray)
{
  return static_cast<T* >( mArray->ConstructedAt( mArray->GetEntriesFast() ) );
}

#endif // __KAMPEVENT_H__
