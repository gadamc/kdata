//_____________________________________________
//
// KEraRawFileIO.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto: Daniel.Wegner@student.kit.edu> on 8/26/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for the KEraRawEventReader and KEraRawEventWriter classes. This class
// does the basic tasks of opening the TFile and setting the branch addresses
// The user should not interact directly with this class, except for the public
// methods that KDataReader/Writer inherit. However, using the methods in this
// class shouldn't be necessary to perform an analysis of the data stored in an
// KDS file.
//

#include "KEraRawFileIO.h"
#include <exception>
#include <iostream>
#include <string>
#include "TBranchElement.h"

using namespace std;

ClassImp(KEraRawFileIO);

KEraRawFileIO::KEraRawFileIO(void)
{
  InitializeMembers();

	fFile = 0;
	fTree = 0;

}

KEraRawFileIO::~KEraRawFileIO(void)
{
  Delete();
}

void KEraRawFileIO::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

	//THESE ARE DEFAULT VALUES AND SHOULD NEVER BE CHANGED!
	fTreeName = "EdwTree";
	fTreeTitle = "EdwTree";
	fBranchName = "Event";

}

void KEraRawFileIO::Delete(void)
{

	Close();
	if(fFile != 0) {
		delete fFile; fFile = 0;
	}

}

void KEraRawFileIO::CreateTree(void)
{

	fFile->cd();

	if(fTree != 0) {
		delete fTree; fTree = 0;
	}

	fTree = new TTree(fTreeName.c_str(), fTreeTitle.c_str());

	if(fTree->IsZombie())
		cout << "KEraRawFileIO::CreateTree TTree is Zombie" << endl;
	else {
		fTree->SetCacheSize(10000000);
		fTree->AddBranchToCache("*");
	}
}

void KEraRawFileIO::GetTreePointerInFile(void)
{

	fFile->cd();

	if(fTree != 0) {
		delete fTree; fTree = 0;
	}

	fTree = (TTree *)fFile->Get(fTreeName.c_str());

	if(fTree == 0){
		cout << "KEraRawFileIO - Couldn't Find Tree " << fTreeName.c_str() << " in file " << endl;
		return;
	}
	if(fTree->IsZombie())
		cout << "TTree is Zombie" << endl;
}


Bool_t KEraRawFileIO::Close(Option_t *opt)
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


TFile* KEraRawFileIO::OpenFileForWriting(const Char_t* name, const Char_t* option,
													const Char_t* title)
{

	fFile = new TFile(name, option, title);
	//MUST do this after initializing the TFile
	CreateTree();

	return fFile;
}

TFile* KEraRawFileIO::OpenFileForReading(const Char_t* name)
{

	fFile = new TFile(name, "read");

	GetTreePointerInFile();

	return fFile;
}

Int_t KEraRawFileIO::GetEntries(void) const
{
	if(fTree!=0) return fTree->GetEntries();
	else return -1;
}

const char* KEraRawFileIO::GetFileName(void) const
{
	if (fFile != 0) {
		return fFile->GetName();
	}
	else return "";

}

const char* KEraRawFileIO::GetEventClassName(void) const
{
	if(fTree != 0){
		TBranchElement *branch = (TBranchElement *)fTree->GetBranch(fBranchName.c_str());
		if(branch != 0)
			return branch->GetClassName();

		else return "";
	}
	else return "";
}


void KEraRawFileIO::ls(Option_t *anOpt) const
{
	if(fFile != 0){
		fFile->ls(anOpt);
	}

}



