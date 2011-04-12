/*
 *  KDataWriter.h
 *  KDataStorage
 *
 *  Created by Adam Cox on 4/6/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KDataWriter_H__
#define __KDataWriter_H__

#include "KDataFileIO.h"
#include "TBranchElement.h"
#include "KHLAEvent.h"

class KEvent;
class TList;

class KDataWriter : public KDataFileIO { 

public:
  //Constructors -- Use these preferred ways of using this class
	//In this way, you don't own the KEvent memory and don't have 
	//to worry about cleaning up. ROOT does this for you.
	//But you can use the GetEvent method below to get a pointer
	//to the event class.
  KDataWriter(void);
	KDataWriter(const Char_t* filename, const Char_t* eventType = KHLAEvent::GetClassName(), 
								const Char_t* mode = "recreate");
	Bool_t OpenFile(const Char_t* fileName, const Char_t* eventType = KHLAEvent::GetClassName(),
									const Char_t* mode = "recreate");
	
	
	//Use these constructors/methods if you own the KEvent object
	//and you are responsible for cleaning up.
	KDataWriter(const Char_t* fileName, KEvent **anEvent,  
							const Char_t* mode = "recreate"); 
	Bool_t OpenFile(const Char_t* fileName, KEvent **anEvent,
									const Char_t* mode = "recreate");
	
	virtual ~KDataWriter(void);

	KEvent* GetEvent(void);
	Int_t Fill(void); 
	Int_t Write(const Char_t* name = 0, Int_t option = TObject::kWriteDelete, Int_t bufsize = 0);
	Bool_t Close(Option_t *opt = "");
	Bool_t IsReady(void) const;
	
	//some methods that call TTree methods of the same name.
	TTree* CloneTree(TTree *treeIn, Long64_t nentries = -1, Option_t* option = "");
	TTree* ConcatenateTrees(TList* li, Option_t *anOpt = "");
	
	//some methods that call TFile methods of the same name.
	
	
private:
	
	Bool_t bIsReady; //true if ready, false if not.
	TBranchElement *fEventBranch;
	KEvent *fLocalEvent;
	
  //private methods
  //void Reset(void);
	//void SetRunConstants(Int_t aBigRunNumber = 0);
	Bool_t SetTreeBranch(KEvent** event);
	Bool_t SetTreeBranch(const Char_t* eventType);

	//forbid copy constructor and assignment operator for this class.
	//maybe one day implement a reasonable copy constructor and assignment operator
	//so long as you make sure to create a new file rather than write to an existing open file
	KDataWriter(const KDataWriter &aWriter);
	KDataWriter& operator=( const KDataWriter &aWriter);
	
  ClassDef(KDataWriter,1);
};


#endif // __KDataWriter_H__
