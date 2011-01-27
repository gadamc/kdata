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
#include "TRef.h"

//sub record forward declarrations
class KHLASambaRecord;
class KHLABolometerRecord;
class KHLABoloPulseRecord;
class KHLAMuonModuleRecord;
class KRawEvent;
class TClonesArray;

//class TBits;

const Char_t kHLAEventName[] = "HLA";
const Char_t kHLAClassName[] = "KHLAEvent";

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

	KHLAEvent& operator=(const KEvent &anEvent);
	KHLAEvent& operator=(const KRawEvent &anEvent);
	KHLAEvent& operator=(const KHLAEvent &anEvent);
	Int_t AddSubRecords(const KHLAEvent &ev, Bool_t skimNoise = false);
		
  Bool_t AddMuonModuleSubRecord(const KHLAMuonModuleRecord &inMuonModule);
  Bool_t AddBoloSubRecord(const KHLABolometerRecord &inBolo);
  
	//Easy Access Methods
	//These methods access commonly requested quantities without
	//having to dig down into the branch structure.
	Double_t GetSumBoloEnergyRecoil(void) const;
	//Double_t GetSumBoloEnergyHeat(void);
	Double_t GetSumBoloEnergyIon(void) const;
  Bool_t IsGoodMuonVetoData(void) const;
	
	//Getters and Setters to member variables
	
	Int_t GetNumSambas(void) const {return fNumSamba;}
	Int_t GetNumBolos(void) const {return fNumBolo;}
	Int_t GetNumBoloPulses(void) const {return fNumBoloPulse;} 
	Int_t GetNumMuonModules(void) const {return fNumMuonModule;} 
	
	//system record pointers
	KHLABoloSysRecord* GetBoloSystemRecord(void) {return &fBoloSystem;}
	KHLAMuonVetoSysRecord* GetMuonVetoSystemRecord(void) {return &fMuonSystem;}
	
	KHLASambaRecord* GetSamba(Int_t i) const;
	KHLABolometerRecord* GetBolo(Int_t i) const;
	KHLABoloPulseRecord* GetBoloPulse(Int_t i) const;
	KHLAMuonModuleRecord* GetMuonModule(Int_t i) const;
	
	//pointers to TClonesArrays of SubRecord objects. 
	//unless you need some specific informaton from the TClonesArray
	//its best to use the methods above to get pointers
	//directly to the subRecords
	TClonesArray* GetSambaRecords(void) const {return fSamba;}
	TClonesArray* GetBoloRecords(void) const {return fBolo;}
	TClonesArray* GetBoloPulseRecords(void) const {return fBoloPulse;}
	TClonesArray* GetMuonModuleRecords(void) const {return fMuonModule;}
	
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

  
	//Only use these if you know what you're doing. 
	//void SetNumSamba(Int_t aNum){fNumSamba = aNum;} //Only use these if you know what you're doing. 
	//void SetNumBolo(Int_t aNum){fNumBolo = aNum;} //Only use these if you know what you're doing. 
	//void SetNumBoloPulse(Int_t aNum){fNumBoloPulse = aNum;} //Only use these if you know what you're doing. 
	//void SetNumMuonModule(Int_t aNum){fNumMuonModule = aNum;} //Only use these if you know what you're doing. 
		

private: 
  
	KHLABoloSysRecord fBoloSystem;  //the Bolometer System Record
	KHLAMuonVetoSysRecord fMuonSystem; //Muon Veto System Record
	
	//Array of SubRecords containing event information
	Int_t fNumSamba; //the number of Samba Sub Records in this event
	Int_t fNumBolo; //the number of Single Bolo Sub Records in this event
	Int_t fNumBoloPulse; //the number of Bolo Pulse Sub Records in this event
	Int_t fNumMuonModule; //the number of Muon Module Sub Records in this event. 
	
	//bool fAssignmentOptionNoTref;
	//do we need to store these values in the tree? how much disk space would
	//we save? There are two ways
	//a user can determine the number of these records in the event.
	//1) - from a root script that calls Event::GetNumSamba, etc...
	//2) - from the TTree selections of
	//         @fSamba.size()>0
	//         @fBolo.size()>0
	//         @fBoloPulse.size()>0
	//         @fMuModlue.size()>0
	// For example root [0] TFile f("EDSRunFile.root");
	//             root [1] t->Scan("*","@fBolo.size()==1");
	// selects all of the events where there is only one bolometer
	// with a record in the event. Now.. this doesn't (YET?) guarantee
	// that only one Bolometer in the event has a real physics event
	// it only tells you the number of records in the data structure
	
	TClonesArray *fSamba; //->  pointer to the array of Samba Sub Records
	TClonesArray *fBolo; //->  pointer to the array of Single Bolo Sub Records
	TClonesArray *fBoloPulse;  //-> pointer to the array of Bolo Pulse Sub Records
	TClonesArray *fMuonModule; //-> pointer to the array of Muon Module Sub Records
		
	/*NOTE - I am using short names here in order to improve the readability
	 of the TTree when looking at the file from the TBrowser or from
	 the TTree::Show TTree::Scan methods. 
	 
	 ALSO, the names of the system record class and the TClonesArrays
	 should be EXACTLY the same as the names used in ALL Other Event Levels, 
	 such as the Raw level and the MC level. This makes it easy for porting
	 analysis code for each level... 
	 
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
	
  
  Int_t fRunNumber; //big Run number. the small run number is found in the Bolometer data record
	Double_t fRunStartTime;  //begining of the Big Run 
	Double_t fRunEndTime; //end of the Big Run
  Int_t fGSEventNumber; //Global Software Event Number. starts from 1 with each new Run
  
	//private methods
	void CreateArrays(void);
	void InitializeMembers(void);
	void CopyLocalMembers(const KHLAEvent &anEvent);
	void CopyClonesArrays(const KHLAEvent &anEvent);
	void CopyFromRawEvent(const KRawEvent &anEvent);
	
  template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
  void DeleteArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
  void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);

  
	UInt_t GetLargestUniqueIDNumber(void);

	
	ClassDef(KHLAEvent ,2);
};


#endif // __KHLAEVENT_H__
