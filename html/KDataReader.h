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

class KEvent;

class KDataReader : public KDataFileIO { 

public:
  //Constructors
  //KDataReader(const Char_t* fileName);
  KDataReader(void);
  KDataReader(const Char_t* fileName, KEvent **anEvent = 0, Bool_t useCache = true);
  virtual ~KDataReader(void);

  virtual Bool_t Close(Option_t *opt = "");

  virtual KEvent* GetEvent(void);
  virtual Int_t GetCurrentEntryNumber(void) const {return fCurrentEntry;}
  virtual Int_t GetNextEntry(void);
  virtual Int_t GetPreviousEntry(void);
  virtual Int_t GetEntry(Int_t anEntry);
  virtual Int_t GetEntryWithGSEventNumber(Int_t anEntry);
  virtual Int_t GetEntries(void) const {return fEntries;}


  virtual TObject* Get(const Char_t* namecycle) const;

  virtual Int_t GetNextMuonEntry(Int_t anEntry=-1) ;
  virtual Int_t GetPreviousMuonEntry(Int_t anEntry=-1) ;
  virtual Int_t GetNextBoloEntry(Int_t anEntry=-1) ;
  virtual Int_t GetPreviousBoloEntry(Int_t anEntry=-1) ;

  virtual void SetUseInternalCache(Bool_t bOption = true);

  virtual void ls(Option_t *opt = "") const;
  virtual Bool_t cd(const char *path = 0);
  virtual const char* GetFileName(void) const;
  virtual const char* GetEventClassName(void) const;
  virtual TFile* GetTFile(void) const {return fFile;}
  virtual TTree* GetTTree(void) const {return fTree;}

private:

  Int_t fCurrentEntry; //current entry number in the tree
  Int_t fEntries; //number of entries in the tree
  Bool_t fGSEventIndex; //the global software event number index
  TFile *fFile;
  TTree *fTree;
  KEvent *fLocalEvent;
  //private methods
  
  void InitializeMembers(void);
  Bool_t SetBranchAddress(KEvent **anEvent = 0);
  TFile* OpenFileForReading(const Char_t* name);
  //Bool_t OpenFile(const Char_t* fileName);
  Bool_t OpenFile(const Char_t* fileName, KEvent **anEvent = 0, Bool_t useCache = true);
  void GetTreePointerInFile(void);

 
  ClassDef(KDataReader,2);
};


#endif // __KDataReader_H__
