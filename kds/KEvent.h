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


//this is a fantastically horrible place to put these... 
//maybe I should put them in a namespace
//KEvent namespace...?
const Int_t kBoloTriggerType = 0x1;
const Int_t kMuonVetoTriggerType = 0x2;
const Int_t kNCTriggerType = 0x4;  //whenever (if?) we add the Neutron Counter data

class KEvent : public TObject {
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O and rootcint to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	KEvent(void);
	KEvent(const KEvent &anEvent);
	virtual ~KEvent(void);
	virtual KEvent& operator=(const KEvent &anEvent);
	Bool_t IsSame(const KEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const KEvent &anEvent) const;
	Bool_t operator!=(const KEvent &anEvent) const { return !(*this==anEvent); }	
	virtual void Clear(Option_t *option = "C");
	virtual void Compact(void) {/*nothing to compact */};
	
	//Calculated Quantities
	//Double_t GetTimeStamp(void);
	
	//Getters
	Int_t GetRunNumber(void) const {return fRunNumber;}
	Double_t GetEventTriggerTime(void) const {return fEventTriggerTime;}
	Long64_t GetStamp(void) const {return fStamp;}
	Double_t GetRunStartTime(void) const {return fRunStartTime;}
	Double_t GetRunEndTime(void) const {return fRunEndTime;}
	//Double_t GetTimeSinceLastEvent(void) {return fTimeSinceLastEvent;}
	Int_t GetTriggerType(void) const {return fTriggerType;}
	//Int_t GetDataType(void) const {return fDataType;}
	
	//Int_t GetDetectorStatusWord(void) const {return fDetectorStatusWord;}
	//Int_t GetBlindnessWord(void) const {return fBlindnessWord;}
	UInt_t GetGSEventNumber(void) const {return fGSEventNumber;}
	Bool_t IsBlind(void) const;
	Double_t GetSecPerStamp(void) const; 
	Double_t GetStampTime(void) const; 

	//Setters
	void SetRunNumber(Int_t aNum) {fRunNumber = aNum;}
	void SetEventTriggerTime(Double_t aNum) {fEventTriggerTime = aNum;}
	void SetStamp(Long64_t aStamp) {fStamp = aStamp;}
	void SetRunStartTime(Double_t aNum) {fRunStartTime = aNum;}
	void SetRunEndTime(Double_t aNum) {fRunEndTime = aNum;}
	//void SetTimeSinceLastEvent(Double_t aNum) {fTimeSinceLastEvent = aNum;}
	//void SetDataType(Int_t aNum) {fDataType = aNum;}
	
	//void SetDetectorStatusWord(Int_t aNum) {fDetectorStatusWord = aNum;}
	//void SetBlindnessWord(Int_t aNum) {fBlindnessWord = aNum;}
	void SetGSEventNumber(UInt_t aNum) {fGSEventNumber = aNum;}
	
	
	void myPrintB() const;
	 
protected:
	void AddTriggerType(Int_t aNum) {fTriggerType |= aNum;}  
	
private:
	void SetTriggerType(Int_t aNum) {fTriggerType = aNum;}
	
	Int_t fRunNumber; //big Run number. the small run number is found in the Bolometer data record
	Double_t fRunStartTime;  //begining of the Big Run 
	Double_t fRunEndTime; //end of the Big Run
	
	//these will change for every new event in the run. 
	Double_t fEventTriggerTime;  //trigger time of the event according to the PC time. This value is ambiguous and redundant! However, the ambiguity/redundancy will go away if/when the data comes from the IPE DAQ.
	Long64_t fStamp;  //the Opera clock value for this event.
	
	//Double_t fTimeSinceLastEvent;
	Int_t fTriggerType; //0x1 for Bolo event, 0x2 for Muon System, 0x4 if NC counter, 0x8 for next system, etc... 
	//Int_t fDataType; //0x1 for raw K data, 0x2 for HLA Event, 0x4 for MC "raw", 0x8 for MC HLA type ?? is this a good or bad thing? what if somebody sets it to 0x5!
	  //this might be extremely useful for opening up an K ROOT file and determining its event type..
	//Int_t fDetectorStatusWord;  //each bit represents a detector status condition. Currently, this is unfilled.
	//Int_t fBlindnessWord;  //each bit represents a blindness flag. Currently, this is unfilled.
	Int_t fGSEventNumber; //Global Software Event Number. starts from 1 with each new Run
	
	//start and top times of runs are not common to all records since
	//records can come from different systems. 
	
	
	
	//private methods
	void InitializeMembers(void);
	void CopyLocalMembers(const KEvent &anEvent);
	
	
	ClassDef(KEvent, 1);
};

#endif // __KEVENT_H__
