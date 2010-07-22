/*
 *  EdwDSReader.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 4/30/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWDSREADER_H__
#define __EDWDSREADER_H__

#include "EdwDSFileIO.h"
#include "TBranchElement.h"
#include "EdwHLAEvent.h" // I guess I need this for the GetNextMuonEntry()/ GetNextBoloEntry() methods

class EdwEventBase;

class EdwDSReader : public EdwDSFileIO { 

public:
  //Constructors
	//EdwDSReader(const Char_t* fileName);
	EdwDSReader(const Char_t* fileName, EdwEventBase **anEvent = 0);
  virtual ~EdwDSReader(void);

	Bool_t Close(Option_t *opt = "");
	
	EdwEventBase* GetEvent(void);
	Int_t GetCurrentEntryNumber(void) const {return fCurrentEntry;}
	Int_t GetNextEntry(void);
	Int_t GetPreviousEntry(void);
	Int_t GetEntry(Int_t anEntry);
	Int_t GetEntryWithGSEventNumber(Int_t anEntry);
	Int_t GetEntries(void) const {return fEntries;}
	void ls(Option_t *opt = "") const;
	TObject* Get(const Char_t* namecycle) const;
	
	Int_t GetNextMuonEntry(Int_t anEntry=-1) ;
	Int_t GetPreviousMuonEntry(Int_t anEntry=-1) ;
	Int_t GetNextBoloEntry(Int_t anEntry=-1) ;
	Int_t GetPreviousBoloEntry(Int_t anEntry=-1) ;
	
private:
		
	Int_t fCurrentEntry;
	Int_t fEntries;
	Bool_t fGSEventIndex;
	
	EdwEventBase *fLocalEvent;
  //private methods
  void InitializeMembers(void);
	Bool_t SetBranchAddress(EdwEventBase **anEvent = 0);
	//Bool_t OpenFile(const Char_t* fileName);
	Bool_t OpenFile(const Char_t* fileName, EdwEventBase **anEvent = 0);

	//discourage / do not allow copy and assignment
	EdwDSReader(const EdwDSReader &aReader);
	EdwDSReader& operator=( const EdwDSReader &aReader );
	
  ClassDef(EdwDSReader,1);
};


#endif // __EDWDSREADER_H__
