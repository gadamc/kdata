/*
*  KEvent.h
*  KDataStructure
*
*  Created by Adam Cox on 3/3/2010
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/
#ifndef __KEVENT_H__
#define __KEVENT_H__

#include "TObject.h"
//#include "TClonesArray.h"

class KSambaRecord;
class KBolometerRecord;
class KBoloPulseRecord;
class KMuonModuleRecord;
class KMuonVetoSystemRecord;

class KEvent : public TObject {

public:

  KEvent(void);
  KEvent(const KEvent &anEvent);
  virtual ~KEvent(void);
  virtual KEvent& operator=(const KEvent &anEvent);
  Bool_t IsSame(const KEvent &anEvent, Bool_t bPrint = false) const;
  Bool_t operator==(const KEvent &anEvent) const;
  Bool_t operator!=(const KEvent &anEvent) const { return !(*this==anEvent); }  
  virtual void Clear(Option_t *option = "C");
  virtual void Compact(void) {/*nothing to compact */}
  static const char* GetClassName() {return "KEvent"; }

  Long64_t GetEventTriggerStamp(void) const {return fEventTriggerStamp;}
  Long64_t GetStamp(void) const {  
    //this calls GetEventTrigger
    //deprecated kdata version > 4.1
    return GetEventTriggerStamp();  
  }
  Int_t GetTriggerType(void) const {return fTriggerType;}
  //Int_t GetDetectorStatusWord(void) const {return fDetectorStatusWord;}
  //Int_t GetBlindnessWord(void) const {return fBlindnessWord;}
  Bool_t IsBlind(void) const;
  Double_t GetSecPerStamp(void) const; 
  Double_t GetStampTime(void) const; 

  //Setters
  void SetEventTriggerStamp(Long64_t aStamp) {fEventTriggerStamp = aStamp;}
  void SetStamp(Long64_t aStamp) {SetEventTriggerStamp(aStamp);}

  //void SetDetectorStatusWord(Int_t aNum) {fDetectorStatusWord = aNum;}
  //void SetBlindnessWord(Int_t aNum) {fBlindnessWord = aNum;}

  void AddTriggerType(Int_t aNum) {fTriggerType |= aNum;}  

  enum kTriggerTypes {
    kBoloTriggerType = 0x1,
    kMuonVetoTriggerType = 0x2,
    kNCTriggerType = 0x4
  };


  //KEvent is an abstract base class. Derived classes must write
  //their own implementations of the following methods.
  virtual Int_t AddSubRecords(const KEvent &anEvent) = 0;
  virtual Bool_t AddMuonModuleSubRecord(const KMuonModuleRecord &inMuonModule) = 0;
  virtual Bool_t AddBoloSubRecord(const KBolometerRecord &inBolo) = 0;
  virtual void CreateArrays(void) = 0;

  virtual KSambaRecord* GetSamba(Int_t i) const = 0;
  virtual KBolometerRecord* GetBolo(Int_t i) const = 0;
  virtual KBoloPulseRecord* GetBoloPulse(Int_t i) const = 0;
  virtual KMuonModuleRecord* GetMuonModule(Int_t i) const = 0;
  virtual KBoloPulseRecord* GetBoloPulse(const char* name) const = 0;
  virtual KBolometerRecord* GetBolo(const char* name) const = 0;
  
  virtual Int_t GetNumSambas(void) const = 0;
  virtual Int_t GetNumBolos(void) const = 0;
  virtual Int_t GetNumBoloPulses(void) const = 0;
  virtual Int_t GetNumMuonModules(void) const = 0;
  virtual KMuonVetoSystemRecord* GetMuonVetoSystemRecord(void)  = 0;

private:
  void SetTriggerType(Int_t aNum) {fTriggerType = aNum;}


  //these will change for every new event in the run. 
  Long64_t fEventTriggerStamp;  //the Opera clock value for this event.

  Int_t fTriggerType; //0x1 for Bolo event, 0x2 for Muon System, 0x4 if NC counter, 0x8 for next system, etc... 
  //Int_t fDetectorStatusWord;  //each bit represents a detector status condition. Currently, this is unfilled.
  //Int_t fBlindnessWord;  //each bit represents a blindness flag. Currently, this is unfilled.


  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KEvent &anEvent);


  ClassDef(KEvent, 4);
};

#endif // __KEVENT_H__
