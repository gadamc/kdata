/*
 *  EdwAnalysis.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 5/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWANALYSIS_H__
#define __EDWANALYSIS_H__

#include "Rtypes.h"
#include "EdwEventBase.h"
#include "EdwHLAEvent.h"
#include "EdwRawEvent.h"
//include all of the System and Sub Records here
//so that when somebody includes the EdwAnalysis.h
//they get all of the classes that make up the event
#include "EdwHLABoloSysRecord.h"
#include "EdwHLAMuonVetoSysRecord.h"
#include "EdwHLASambaSubRecord.h"
#include "EdwHLASingleBoloSubRecord.h"
#include "EdwHLABoloPulseSubRecord.h"
#include "EdwHLAMuonModuleSubRecord.h"

class EdwDSReader;
class EdwDSWriter;
class EdwEventCollection;

class EdwAnalysis  { 

public:
  //Constructors
	EdwAnalysis(const Char_t* fileName = 0);
  virtual ~EdwAnalysis(void);
	
	Int_t OpenEdsFile(const Char_t* fileName);
	//static Bool_t IsNorthernMuonModule(EdwEventBase* e); 
	
	Bool_t OpenOutputEdsFile(const Char_t* fileName);
	Bool_t OpenOutputEdsFile(const Char_t* fileName, const Char_t* aLevel);
	Bool_t OpenOutputEdsFile(const Char_t* fileName, EdwEventBase **anEvent);
	Int_t SaveThisEvent(EdwEventBase* e);
	Int_t SaveThisEvent(void);
	Int_t WriteOutput(const Char_t* name = 0, Int_t option = TObject::kWriteDelete, 
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
	
	EdwEventBase* GetEvent(void);
	EdwDSReader* GetFileReader(void) {return fFileReader;}
	EdwDSWriter* GetFileWriter(void) {return fFileWriter;}

	
	EdwEventCollection* GetCoincEvent(EdwEventCollection *mCoincE = 0, Double_t fForwardTime = 5.00000001e-3,
															 Double_t fBackwardTime = 5.00000001e-3); //+- 5 ms inclusive.

	
	//Analysis Methods. Try to make these methods as stand-alone
	//as possible, usually by giving the option to pass in a pointer
	//to the event you want to analyze. If the pointer is NULL, then
	//use the pointer to the event returned by GetEvent()
	Double_t GetBoloPhysicsEventTime(Int_t i, EdwHLAEvent *ev = 0);
	
private:

	EdwDSReader *fFileReader;
	EdwDSWriter *fFileWriter;
	
  //private methods
  void InitializeMembers(void);

  ClassDef(EdwAnalysis,1);
};


#endif // __EDWANALYSIS_H__
