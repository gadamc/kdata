/*
 *  KDataReader.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/30/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KDataReader_H__
#define __KDataReader_H__

#include "KDataFileIO.h"
#include "TBranchElement.h"
#include "KHLAEvent.h" // I guess I need this for the GetNextMuonEntry()/ GetNextBoloEntry() methods

class KEvent;

class KDataReader : public KDataFileIO { 

public:
  //Constructors
	//KDataReader(const Char_t* fileName);
	KDataReader(const Char_t* fileName, KEvent **anEvent = 0);
  virtual ~KDataReader(void);

	Bool_t Close(Option_t *opt = "");
	
	KEvent* GetEvent(void);
	Int_t GetCurrentEntryNumber(void) const {return fCurrentEntry;}
	Int_t GetNextEntry(void);
	Int_t GetPreviousEntry(void);
	Int_t GetEntry(Int_t anEntry);
	Int_t GetEntryWithGSEventNumber(Int_t anEntry);
	Int_t GetEntries(void) const {return fEntries;}
	
	TObject* Get(const Char_t* namecycle) const;
	
	Int_t GetNextMuonEntry(Int_t anEntry=-1) ;
	Int_t GetPreviousMuonEntry(Int_t anEntry=-1) ;
	Int_t GetNextBoloEntry(Int_t anEntry=-1) ;
	Int_t GetPreviousBoloEntry(Int_t anEntry=-1) ;
	
private:
		
	Int_t fCurrentEntry;
	Int_t fEntries;
	Bool_t fGSEventIndex;
	
	KEvent *fLocalEvent;
  //private methods
  void InitializeMembers(void);
	Bool_t SetBranchAddress(KEvent **anEvent = 0);
	//Bool_t OpenFile(const Char_t* fileName);
	Bool_t OpenFile(const Char_t* fileName, KEvent **anEvent = 0);

	//discourage / do not allow copy and assignment
	KDataReader(const KDataReader &aReader);
	KDataReader& operator=( const KDataReader &aReader );
	
  ClassDef(KDataReader,1);
};


#endif // __KDataReader_H__
