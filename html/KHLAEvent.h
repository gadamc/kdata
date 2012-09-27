/*
*  KHLAEvent.h
*  KDataStructure
*
*  Created by Adam Cox on 2/26/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/
#ifndef __KHLAEVENT_H__
#define __KHLAEVENT_H__

#include "KEvent.h"
//system record forward declarations
#include "KHLABoloSysRecord.h"
#include "KHLAMuonVetoSysRecord.h"
#include "KClonesArray.h"
#include "TRef.h"

//sub record forward declarrations
class KHLASambaRecord;
class KHLABolometerRecord;
class KHLABoloPulseRecord;
class KHLAMuonModuleRecord;
class KRawEvent;
class TClonesArray;

//class TBits;

class KHLAEvent : public KEvent {         

public:
  //Constructors and destructor. Only Derived class and the Factory
  //SHOULD access these methods. 
  //However, in order for ROOT's I/O to work properly
  //the constructor and destructor
  //have to be public. Otherwise, I would have made them protected or private
  KHLAEvent(void);
  KHLAEvent(const KHLAEvent &anEvent);
  virtual ~KHLAEvent(void);
  virtual void Clear(Option_t *option = "C");
  virtual void ClearArrays(Option_t *anOption = "C");
  Bool_t IsSame(const KHLAEvent &anEvent, Bool_t bPrint = false) const;
  Bool_t operator==(const KHLAEvent &anEvent) const { return IsSame(anEvent,false); }
  Bool_t operator!=(const KHLAEvent &anEvent) const { return !(*this==anEvent); }
  virtual void Compact(void);
  static const char* GetClassName() {return "KHLAEvent";}


  virtual KEvent& operator=(const KEvent &anEvent);
  virtual KHLAEvent& operator=(const KHLAEvent &anEvent);

  //Easy Access Methods
  //These methods access commonly requested quantities without
  //having to dig down into the branch structure.
  Double_t GetSumBoloEnergyRecoil(void) const;
  //Double_t GetSumBoloEnergyHeat(void);
  Double_t GetSumBoloEnergyIon(void) const;

  KHLABoloSysRecord* GetBoloSystemRecord(void) {return &fBoloSystem;}

  //pointers to KClonesArrays of SubRecord objects. 
  //unless you need some specific informaton from the KClonesArray
  //its best to use the methods above to get pointers
  //directly to the subRecords
  const KClonesArray* GetSambaRecords(void) const {return static_cast<KClonesArray *>(fSamba);}
  const KClonesArray* GetBoloRecords(void) const {return static_cast<KClonesArray *>(fBolo);}
  const KClonesArray* GetBoloPulseRecords(void) const {return static_cast<KClonesArray *>(fBoloPulse);}
  const KClonesArray* GetMuonModuleRecords(void) const {return static_cast<KClonesArray *>(fMuonModule);}
  
  //more pythonic looking methdos
  const KClonesArray* sambaRecords(void) const {return GetSambaRecords();}
  const KClonesArray* boloRecords(void) const {return GetBoloRecords();}
  const KClonesArray* boloPulseRecords(void) const {return GetBoloPulseRecords();}
  const KClonesArray* muonModuleRecords(void) const {return GetMuonModuleRecords();}
  
  //Adders
  KHLASambaRecord* AddSamba();
  KHLABolometerRecord* AddBolo();
  KHLABoloPulseRecord* AddBoloPulse();
  KHLAMuonModuleRecord* AddMuonModule();

  //Setters to local members
  Int_t GetRunNumber(void) const {return fRunNumber;}
  Double_t GetRunStartTime(void) const {return fRunStartTime;}
  Double_t GetRunEndTime(void) const {return fRunEndTime;}
  UInt_t GetGSEventNumber(void) const {return fGSEventNumber;}

  void SetRunNumber(Int_t aNum) {fRunNumber = aNum;}
  void SetRunStartTime(Double_t aNum) {fRunStartTime = aNum;}
  void SetRunEndTime(Double_t aNum) {fRunEndTime = aNum;}
  void SetGSEventNumber(UInt_t aNum) {fGSEventNumber = aNum;}

  void CopyLocalMembers(const KHLAEvent &anEvent);
  void CopyClonesArrays(const KHLAEvent &anEvent);

  Int_t AddBoloSubRecords(const KHLAEvent &anEvent, Bool_t skim = false);
  Int_t AddMuonModuleSubRecords(const KHLAEvent &anEvent);

  //methods required by the KEvent abstract class.
  virtual Int_t AddSubRecords(const KEvent &anEvent);
  virtual Bool_t AddMuonModuleSubRecord(const KMuonModuleRecord &inMuonModule);
  virtual Bool_t AddBoloSubRecord(const KBolometerRecord &inBolo);

  virtual Int_t GetNumSambas(void) const {return (fSamba) ? fSamba->GetEntriesFast() : 0;}
  virtual Int_t GetNumBolos(void) const {return (fBolo) ? fBolo->GetEntriesFast() : 0;;}
  virtual Int_t GetNumBoloPulses(void) const {return (fBoloPulse) ? fBoloPulse->GetEntriesFast() : 0;} 
  virtual Int_t GetNumMuonModules(void) const {return (fMuonModule) ? fMuonModule->GetEntriesFast() : 0;} 

  virtual KMuonVetoSystemRecord* GetMuonVetoSystemRecord(void)  {return static_cast<KMuonVetoSystemRecord *>(&fMuonSystem);}

  virtual KSambaRecord* GetSamba(Int_t i) const;
  virtual KBolometerRecord* GetBolo(Int_t i) const;
  virtual KBoloPulseRecord* GetBoloPulse(Int_t i) const;
  virtual KMuonModuleRecord* GetMuonModule(Int_t i) const;
  virtual KBolometerRecord* GetBolo(const char* name) const;
  virtual KBoloPulseRecord* GetBoloPulse(const char* name) const;

  //soon to be deprecated methods
  Double_t GetEventTriggerTime(void) const;

private: 

  KHLABoloSysRecord fBoloSystem;  //the Bolometer System Record..??
  KHLAMuonVetoSysRecord fMuonSystem; //Muon Veto System Record


  TClonesArray *fSamba; //->  pointer to the array of Samba Sub Records
  TClonesArray *fBolo; //->  pointer to the array of Single Bolo Sub Records
  TClonesArray *fBoloPulse;  //-> pointer to the array of Bolo Pulse Sub Records
  TClonesArray *fMuonModule; //-> pointer to the array of Muon Module Sub Records

  /*NOTE - I am using short names here in order to improve the readability
  of the TTree when looking at the file from the TBrowser or from
  the TTree::Show TTree::Scan methods. 

ALSO, the names of the system record class and the KClonesArrays
should be EXACTLY the same as the names used in ALL Other Event Levels, 
such as the Raw level and the MC level. This makes it easy for porting
analysis code for each level... 
Hmm... there is probably a better way to do this by putting these in the KEvent class...

In principle, everybody doing analysis should use each Class's Member 
Functions, rather than directly reading from the TTree. This ensures 
that the data in the TTrees are properly encapsulated and the user 
doesn't NEED to know the structure or the names of the underlying 
TTree.... This is especially important if the underlying data 
structure changes! By using member functions in a ROOT script or 
compiled code, you're no longer dependent upon the exact naming 
structure in your analysis. 
But, since many people are still uncomfortable with C++
and many people are still doing their 
analysis by direct interaction with the data Tree, we will support 
them as much as possible by using these easy-to-read names that 
are the same accross the different "levels".

*/

  //a user can determine the number of these records in the event.
  //1) - from a root script that calls Event::GetNumSamba, etc...
  //2) - from the TTree selections of
  //         @fSamba.GetEntriesFast()>0
  //         @fBolo.GetEntriesFast()>0
  //         @fBoloPulse.GetEntriesFast()>0
  //         @fMuModlue.GetEntriesFast()>0
  // For example root [0] TFile f("KDataRunFile.root");
  //             root [1] t->Scan("*","@fBolo.GetEntriesFast()==1");
  // selects all of the events where there is only one bolometer
  // with a record in the event. This doesn't guarantee
  // that only one Bolometer in the event has a real physics event
  // it only tells you the number of records in the data structure


Int_t fRunNumber; //big Run number. the small run number is found in the Bolometer data record
Double_t fRunStartTime;  //begining of the Big Run 
Double_t fRunEndTime; //end of the Big Run
Int_t fGSEventNumber; //Global Software Event Number. starts from 1 with each new Run

  //private methods
void CreateArrays(void);
void InitializeMembers(void);


  template<class T> T* AddSubRecord(TClonesArray *mArray);
void DeleteArray(Option_t *anOpt, TClonesArray *mArray);
void ClearArray(Option_t *anOpt, TClonesArray *mArray);


UInt_t GetLargestUniqueIDNumber(void);


ClassDef(KHLAEvent ,4);
};

template<class T> T* KHLAEvent::AddSubRecord(TClonesArray *mArray)
{
  return static_cast<T* >(static_cast<KClonesArray *>(mArray)->GetNewOrCleanedObject(mArray->GetEntriesFast() ) );
}

#endif // __KHLAEVENT_H__
