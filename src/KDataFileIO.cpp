//_____________________________________________
//
// KDataFileIO.cc
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/11/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for the KDataReader and KDataWriter classes. This class
// does the basic tasks of opening the TFile and setting the branch addresses
// The user should not interact directly with this class, except for the public
// methods that KDataReader/Writer inherit. However, using the methods in this 
// class shouldn't be necessary to perform an analysis of the data stored in an
// KDS file. 
//

#include "KDataFileIO.h"
#include <exception>
#include <iostream>
#include <string>
#include "TBranchElement.h"

using namespace std;

ClassImp(KDataFileIO);

KDataFileIO::KDataFileIO(void)
{
  InitializeMembers();

	fFile = 0;
	fTree = 0;
	
}

KDataFileIO::~KDataFileIO(void)
{
  Delete();
}

void KDataFileIO::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	
	//THESE ARE DEFAULT VALUES AND SHOULD NEVER BE CHANGED!
	fTreeName = "t";
	fTreeTitle = "KDSTree";
	fBranchName = "event";

}

void KDataFileIO::Delete(void)
{
	
	Close();
	if(fFile != 0) {
		delete fFile; fFile = 0;
	}
	 
}

void KDataFileIO::CreateTree(void)
{
	
	fFile->cd();
	
	if(fTree != 0) {
		delete fTree; fTree = 0;
	}
		
	fTree = new TTree(fTreeName.c_str(), fTreeTitle.c_str());
	
	if(fTree->IsZombie())
		cout << "KDataFileIO::CreateTree TTree is Zombie" << endl; 	
	else {
		//fTree->BranchRef(); //force the user to turn this on 
		fTree->SetCacheSize(10000000);
		fTree->AddBranchToCache("*");
	}
}

void KDataFileIO::GetTreePointerInFile(void)
{
	
	fFile->cd();
	
	if(fTree != 0) {
		delete fTree; fTree = 0;
	}
	
	fTree = (TTree *)fFile->Get(fTreeName.c_str());

	if(fTree == 0){
		cout << "KDataFileIO - Couldn't Find Tree " << fTreeName.c_str() << " in file " << endl;
		return;
	}
	if(fTree->IsZombie())
		cout << "TTree is Zombie" << endl; 
}


Bool_t KDataFileIO::Close(Option_t *opt)
{
	//every time we close the file, we delete the Event pointer. 
	
	if(fFile == 0){
		fTree = 0; //the tree should be have been deleted. 
		return true;
	}
	
	fFile->Close(opt);
	
	if (fFile->IsOpen()) {
		return false;
		//return false if we couldn't close the file.
	}
	else {
		delete fFile; fFile = 0;
		fTree = 0; //the tree is deleted by the TFile
		return true;
	}
	
}


TFile* KDataFileIO::OpenFileForWriting(const Char_t* name, const Char_t* option, 
													const Char_t* title)
{
	 
	fFile = new TFile(name, option, title); 
	//MUST do this after initializing the TFile
	CreateTree();
	
	return fFile;
}

TFile* KDataFileIO::OpenFileForReading(const Char_t* name)
{
	
	fFile = new TFile(name, "read");
	
	GetTreePointerInFile();

	return fFile;
}

Int_t KDataFileIO::GetEntries(void) const 
{
	if(fTree!=0) return fTree->GetEntries();
	else return -1;
}

const char* KDataFileIO::GetFileName(void) const
{
	if (fFile != 0) {
		return fFile->GetName();
	}
	else return "";
		
}

const char* KDataFileIO::GetEventClassName(void) const
{
	if(fTree != 0){
		TBranchElement *branch = (TBranchElement *)fTree->GetBranch(fBranchName.c_str());
		if(branch != 0)
			return branch->GetClassName();
		
		else return "";
	}
	else return "";
}


