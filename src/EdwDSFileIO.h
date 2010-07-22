/*
 *  EdwDSFileIO.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 5/11/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWDSFILEIO_H__
#define __EDWDSFILEIO_H__

#include "Rtypes.h"
#include "TTree.h"
#include <string>
#include "TFile.h"

using namespace std;

class EdwDSFileIO  { 

protected:
	//Constructors -- This class is the base for the EdwTreeWriter and EdwTreeReader classes
  EdwDSFileIO(void);
  virtual ~EdwDSFileIO(void);
	virtual void Delete(void);
	TFile* OpenFileForWriting(const Char_t* name, const Char_t* option = "recreate", 
														const Char_t* title = "");
	TFile* OpenFileForReading(const Char_t* name);
	//give the derived classes direct access to these
	TFile *fFile;
	TTree *fTree;

public:

	string GetTreeName(void) const {return fTreeName;}
	string GetTreeTitle(void) const {return fTreeTitle;}
	string GetBranchName(void) const {return fBranchName;}
	TFile* GetTFile(void) const {return fFile;}
	TTree* GetTTree(void) const {return fTree;}
	virtual Bool_t Close(Option_t *opt = "");
	virtual Int_t GetEntries(void) const;
	const char* GetFileName(void) const;
	const char* GetEventClassName(void) const;
	
private:
	string fTreeName;
	string fTreeTitle;
	string fBranchName;

	
  //private methods
  void InitializeMembers(void);
	void CreateTree(void);
  void GetTreePointerInFile(void);
	ClassDef(EdwDSFileIO,1);
};


#endif // __EDWDSFILEIO_H__
