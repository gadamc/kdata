/*
 *  EdwHLAEvent.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 2/26/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __EDWHLAEVENT_H__
#define __EDWHLAEVENT_H__

#include "EdwEventBase.h"

//system record forward declarations
#include "EdwHLABoloSysRecord.h"
#include "EdwHLAMuonVetoSysRecord.h"
#include "TRef.h"
//sub record forward declarrations
class TClonesArray; 
class EdwHLASambaSubRecord;
class EdwHLASingleBoloSubRecord;
class EdwHLABoloPulseSubRecord;
class EdwHLAMuonModuleSubRecord;
class EdwRawEvent;
//class EdwHLANCSysRecord;
class TBits;

const Char_t kHLAEventName[] = "HLA";
const Char_t kHLAClassName[] = "EdwHLAEvent";

class EdwHLAEvent : public EdwEventBase {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	EdwHLAEvent(void);
	EdwHLAEvent(const EdwHLAEvent &anEvent);
	virtual ~EdwHLAEvent(void);
	EdwHLAEvent& operator=(const EdwEventBase &anEvent);
	EdwHLAEvent& operator=(const EdwRawEvent &anEvent);
	EdwHLAEvent& operator=(const EdwHLAEvent &anEvent);
	virtual void Clear(Option_t *option = "C");
	Bool_t IsSame(const EdwHLAEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwHLAEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const EdwHLAEvent &anEvent) const { return !(*this==anEvent); }
	virtual void Compact(void);
	
	//Easy Access Methods
	//These methods access commonly requested quantities without
	//having to dig down into the branch structure.
	Double_t GetSumBoloEnergyRecoil(void);
	Double_t GetSumBoloEnergyHeat(void);
	Double_t GetSumBoloEnergyIon(void);
	//Int_t GetNumberBoloHits(void);
	
	//Getters and Setters to member variables
	Bool_t TestDataCleaningBit(Int_t i) const {return fDataCleaningWord.TestBitNumber(i);}
	//Bool_t TestDataCleaningWord(Int_t aWord); //this does a bit-wise comparison
	
	Int_t GetNumSambas(void) const {return fNumSamba;}
	Int_t GetNumBolos(void) const {return fNumBolo;}
	Int_t GetNumBoloPulses(void) const {return fNumBoloPulse;} 
	Int_t GetNumMuonModules(void) const {return fNumMuonModule;} 
	
	//system record pointers
	EdwHLABoloSysRecord* GetBoloSystemRecord(void) {return &fBoloSystem;}
	EdwHLAMuonVetoSysRecord* GetMuonVetoSystemRecord(void) {return &fMuonSystem;}
	//EdwHLANCSysRecord* GetHLANCSysRecord(void) {return &fEdwHLANCSysRecord;}
	
	EdwHLASambaSubRecord* GetSamba(Int_t i) const;
	EdwHLASingleBoloSubRecord* GetBolo(Int_t i) const;
	EdwHLABoloPulseSubRecord* GetBoloPulse(Int_t i) const;
	EdwHLAMuonModuleSubRecord* GetMuonModule(Int_t i) const;
	
	//pointers to TClonesArrays of SubRecord objects. 
	//unless you need some specific informaton from the TClonesArray
	//its best to use the methods above to get pointers
	//directly to the subRecords
	TClonesArray* GetSambaSubRecords(void) const {return fSamba;}
	TClonesArray* GetBoloSubRecords(void) const {return fBolo;}
	TClonesArray* GetBoloPulseSubRecords(void) const {return fBoloPulse;}
	TClonesArray* GetMuonModuleSubRecords(void) const {return fMuonModule;}
	
	//Adders
	EdwHLASambaSubRecord* AddSamba();
	EdwHLASingleBoloSubRecord* AddBolo();
	EdwHLABoloPulseSubRecord* AddBoloPulse();
	EdwHLAMuonModuleSubRecord* AddMuonModule();
	
	//Remove
	//RemoveSamba(Int_t i);
	//RemoveBolo(Int_t i);
	//RemoveBoloPulse(Int_t i);
	//RemoveMuonModule(Int_t i);
	
	//Setters to local members
	void SetDataCleaningBit(Int_t i, Bool_t val = true) {fDataCleaningWord.SetBitNumber(i,val);}
		
	//Only use these if you know what you're doing. 
	void SetNumSamba(Int_t aNum){fNumSamba = aNum;} //Only use these if you know what you're doing. 
	void SetNumBolo(Int_t aNum){fNumBolo = aNum;} //Only use these if you know what you're doing. 
	void SetNumBoloPulse(Int_t aNum){fNumBoloPulse = aNum;} //Only use these if you know what you're doing. 
	void SetNumMuonModule(Int_t aNum){fNumMuonModule = aNum;} //Only use these if you know what you're doing. 
	
	//Print data members //Probably it would be much easier to implement it in EventBase and just overload it, but I don't really know how to do it and I only really nead it for testing.
	void myPrint();
	
private: 

	TBits fDataCleaningWord;  //each bit in this word represents the pass/fail of this event to specific data cleaning cuts

	EdwHLABoloSysRecord fBoloSystem;  
	EdwHLAMuonVetoSysRecord fMuonSystem; 
	
	//Array of SubRecords containing event information
	Int_t fNumSamba; //the number of Samba Sub Records in this event
	Int_t fNumBolo; //the number of Single Bolo Sub Records in this event
	Int_t fNumBoloPulse; //the number of Bolo Pulse Sub Records in this event
	Int_t fNumMuonModule; //the number of Muon Module Sub Records in this event. 
	
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
	
	//private methods
	template<class T> T* AddSubRecord(TClonesArray *mArray, Int_t &mCount);
	void ClearArray(Option_t *anOpt, TClonesArray *mArray, Int_t &mCount);
	void CreateArrays(void);
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwHLAEvent &anEvent);
	void CopyClonesArrays(const EdwHLAEvent &anEvent);
	void CopyFromRawEvent(const EdwRawEvent &anEvent);
	
	ClassDef(EdwHLAEvent ,1);
};


#endif // __EDWHLAEVENT_H__
