/*
 *  KDataAnalysis.h
 *  KEdwDataStructure
 *
 *  Created by Adam Cox on 5/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KDATAANALYSIS_H__
#define __KDATAANALYSIS_H__

#include "Rtypes.h"
#include "KEvent.h"
#include "KHLAEvent.h"
#include "KRawEvent.h"
//include all of the System and Sub Records here
//so that when somebody includes the KDataAnalysis.h
//they get all of the classes that make up the event
#include "KHLABoloSysRecord.h"
#include "KHLAMuonVetoSysRecord.h"
#include "KHLASambaRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLABoloPulseRecord.h"
#include "KHLAMuonModuleRecord.h"

class KDataReader;
class KDataWriter;
class KEventCollection;

class KDataAnalysis  { 

public:
  //Constructors
	KDataAnalysis(const Char_t* fileName = 0);
  virtual ~KDataAnalysis(void);
	
	Int_t OpenEdsFile(const Char_t* fileName);
	//static Bool_t IsNorthernMuonModule(KEdwEventBase* e); 
	
	Bool_t OpenOutputKDataFile(const Char_t* fileName);
	Bool_t OpenOutputKDataFile(const Char_t* fileName, const Char_t* aLevel);
	Bool_t OpenOutputKDataFile(const Char_t* fileName, KEvent **anEvent);
	Int_t SaveThisEvent(KEvent* e);
	Int_t SaveThisEvent(void);
	Int_t Write(const Char_t* name = 0, Int_t option = TObject::kWriteDelete, 
										Int_t bufsize = 0);
	void CloseOutput(Option_t *anOpt = "");
	void CloseInput(Option_t *anOpt = "");
	void Close(Option_t *anOpt = "");
	
	Int_t GetCurrentEntryNumber(void) const;
	Int_t GetNextEntry(void) const;
	Int_t GetPreviousEntry(void) const;
	Int_t GetEntry(Int_t anEntry) const;
	Int_t GetEntries(void) const;

	
	const char* GetEventClassName(void) const;
	
	KEvent* GetEvent(void);
	KDataReader* GetFileReader(void) {return fFileReader;}
	KDataWriter* GetFileWriter(void) {return fFileWriter;}

	
	KEventCollection* GetCoincEvent(KEventCollection *mCoincE = 0, Double_t fForwardTime = 5.00000001e-3,
															 Double_t fBackwardTime = 5.00000001e-3); //+- 5 ms inclusive.

	
	//Analysis Methods. Try to make these methods as stand-alone
	//as possible, usually by giving the option to pass in a pointer
	//to the event you want to analyze. If the pointer is NULL, then
	//use the pointer to the event returned by GetEvent()
	Double_t GetBoloPhysicsEventTime(Int_t i, KHLAEvent *ev = 0);
	
private:

	KDataReader *fFileReader;
	KDataWriter *fFileWriter;
	
  //private methods
  void InitializeMembers(void);

  ClassDef(KDataAnalysis,1);
};


#endif // __KDATAANALYSIS_H__
