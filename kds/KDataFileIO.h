/*
*  KDataFileIO.h
*  KDataStructure
*
*  Created by Adam Cox on 5/11/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/

#ifndef __KDataFileIO_H__
#define __KDataFileIO_H__

#include "Rtypes.h"
#include "TChain.h"
#include <string>
#include "TFile.h"


using namespace std;

class KDataFileIO  { 

protected:
  //Constructors -- This class is the base for the KTreeWriter and KTreeReader classes
  KDataFileIO(void);
  virtual ~KDataFileIO(void);
  //virtual void Delete(void);
  // 
  string fTreeName;
  string fTreeTitle;
  string fBranchName;
  //string fChainTitle;
 
  //give the derived classes direct access to these
  //TChain *fChain;

public:

  string GetTreeName(void) const {return fTreeName;}
  string GetTreeTitle(void) const {return fTreeTitle;}
  string GetBranchName(void) const {return fBranchName;}
  //string GetChaneTitle(void) const {return fChainTitle;}
  //TFile* GetTFile(void) const {return fFile;}
  //TTree* GetTTree(void) const {return (TTree *)fChain;}
  //TChain* GetTChain(void) const {return fChain;}
  //virtual Bool_t Close(Option_t *opt = "");
  //virtual Int_t GetEntries(void) const;
  //const char* GetFileName(void) const;
  //const char* GetEventClassName(void) const;
  //void ls(Option_t *opt = "") const;
  //virtual Bool_t cd(const char *path = 0);

private:
  

  //private methods
  void InitializeMembers(void);
  //void CreateTree(void);
  //void GetTreePointerInFile(const char* name);
  ClassDef(KDataFileIO,2);
};


#endif // __KDataFileIO_H__
