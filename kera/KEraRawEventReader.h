/*
 *  KEraRawEventReader.h
 *  KEraRawEventStructure
 *
 *  Created by Daniel Wegner on 8/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KERARAWEVENTREADER_H__
#define __KERARAWEVENTREADER_H__

#include "KEraRawFileIO.h"
#include "TBranchElement.h"
#include "EdwEvent.h"
#include <string>
using namespace std;

class KEraEventFinder;

class KEraRawEventReader : public KEraRawFileIO {

public:
  //Constructors
    KEraRawEventReader(void);
	KEraRawEventReader(const string fileName, EdwEvent **anEvent = 0);
	void Open(const string fileName, EdwEvent **anEvent = 0);
    virtual ~KEraRawEventReader(void);

	Bool_t Close(Option_t *opt = "");

	EdwEvent* GetEvent(void);
	Int_t GetCurrentEntryNumber(void) const {return fCurrentEntry;}
	Int_t GetNextEntry(void);
	Int_t GetPreviousEntry(void);
	Int_t GetEntry(Int_t anEntry);
	//Int_t GetEntryWithGSEventNumber(Int_t anEntry);
	Int_t GetEntries(void) const {return fEntries;}

	TObject* Get(const string namecycle) const;

	friend class KEraEventFinder;


private:

	Int_t fCurrentEntry;
	Int_t fEntries;
	Bool_t fSambaNumberIndex;

	EdwEvent *fLocalEvent;
  //private methods
  void InitializeMembers(void);
	Bool_t SetBranchAddress(EdwEvent **anEvent = 0);
	//Bool_t OpenFile(const Char_t* fileName);
	Bool_t OpenFile(const string fileName, EdwEvent **anEvent = 0);

	//discourage / do not allow copy and assignment
	KEraRawEventReader(const KEraRawEventReader &aReader);
	KEraRawEventReader& operator=(const KEraRawEventReader &aReader);

  ClassDef(KEraRawEventReader,1);
};


#endif // __KEraRawEventReader_H__

