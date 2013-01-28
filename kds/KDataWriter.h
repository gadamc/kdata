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
  virtual Bool_t OpenFile(const Char_t* fileName, const Char_t* eventType = KHLAEvent::GetClassName(),
    const Char_t* mode = "recreate");
  virtual ~KDataWriter(void);

  //Use these constructors/methods if you own the KEvent object
  //and you are responsible for cleaning up.
  KDataWriter(const Char_t* fileName, KEvent **anEvent,  
    const Char_t* mode = "recreate"); 
  virtual Bool_t OpenFile(const Char_t* fileName, KEvent **anEvent,
    const Char_t* mode = "recreate");

  virtual KEvent* GetEvent(void);
  virtual void NewEvent(void);
  virtual Int_t Fill(void); 
  virtual Int_t Write(const Char_t* name = 0, Int_t option = TObject::kWriteDelete, Int_t bufsize = 0);
  virtual Bool_t Close(Option_t *opt = "");
  virtual TFile* GetTFile(void) const {return fFile;}
  virtual TTree* GetTTree(void) const {return fTree;}
  virtual void ls(Option_t *opt = "") const;
  virtual Bool_t cd(const char *path = 0);
  virtual const char* GetFileName(void) const;
  virtual const char* GetEventClassName(void) const;
  virtual Bool_t IsReady(void) const;
  virtual Int_t GetEntries(void) const{if(IsReady()) return fTree->GetEntries(); else return -1;};
 
  virtual Int_t GetBufferSize(void){return fBufferSize;}
  virtual void SetBufferSize(Int_t aSize){fBufferSize = aSize;}

  virtual Int_t GetSplitLevel(void){return fSplitLevel;}
  virtual void SetSplitLevel(Int_t aSize){fSplitLevel = aSize;}
private:

  //Bool_t bIsReady; //true if ready, false if not.
  TBranchElement *fEventBranch;
  KEvent *fLocalEvent;
  TFile *fFile;
  TTree *fTree;
  
  Int_t fBufferSize;
  Int_t fObjectNumber;
  Int_t fSplitLevel;
  Bool_t SetTreeBranch(KEvent** event);
 
  TFile* OpenFileForWriting(const Char_t* name, const Char_t* option = "recreate", 
    const Char_t* title = "");
  void CreateTree(void);
  
  ClassDef(KDataWriter,2);
};


#endif // __KDataWriter_H__
