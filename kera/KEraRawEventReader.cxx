//_____________________________________________
//
// KEraRawEventReader.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 8/26/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class reads in an Edw file and provides some basic functionality
// to access the Event data and to loop through the file, event-by-event.
//


#include "KEraRawEventReader.h"
#include <typeinfo>
#include <iostream>
#include <string>

using namespace std;


ClassImp(KEraRawEventReader);

KEraRawEventReader::KEraRawEventReader() 
{
  //empty constructor. Use KEraRawEventReader::Open to open a file
  
  fLocalEvent = 0;
	InitializeMembers();
}

KEraRawEventReader::KEraRawEventReader(const string fileName, EdwEvent **anEvent)
{
  //standard constructor. You only need to provide the fileName. Memory
  //will be allocated to hold the EdwEvent if anEvent == 0. However,
  //if you need/want to allocate the EdwEvent memory yourself, you can
  //pass an address to the pointer here. 
  //
  
	fLocalEvent = 0;
	InitializeMembers();
  if(!OpenFile(fileName, anEvent)){
		cout << "KEraRawEventReader - Could not open " << fileName << endl;
	}
  
}


KEraRawEventReader::KEraRawEventReader(const KEraRawEventReader& aWriter)
:KEraRawFileIO()
{
  //
  
	//cout << "May Not Copy" << endl;
	//cout << "Create a new instance of KEraRawEventReader. " << endl;
	//cout << "And, you shouldn't have two readers to the same file anyways. " << endl;
}

KEraRawEventReader& KEraRawEventReader::operator=( const KEraRawEventReader& aWriter)
{
  //
  
	//cout << "May Not Copy" << endl;
	//cout << "Create a new instance of KTreeRedaer. " << endl;
	//cout << "And, you shouldn't have two readers to the same file anyways. " << endl;
	return *this;
}

KEraRawEventReader::~KEraRawEventReader(void)
{
  
  Close();
}

void KEraRawEventReader::InitializeMembers(void)
{
  //init local members
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fCurrentEntry = 0;
	fEntries = 0;
  fSambaNumberIndex=0;
}


Bool_t  KEraRawEventReader::OpenFile(const string fileName, EdwEvent **anEvent)
{
  //opens fileName and sets the top-level branch address to the address of anEvent.
  //If you pass in anEvent == 0, then memory will be allocated for you. You can get 
  //a pointer to EdwEvent via the GetEvent() method. 
  //This will close any files that are already open. 
  
  
	Close();  //close the file, if one is already open.
	fFile = OpenFileForReading(fileName.c_str());
	Bool_t theRet = false;
  
	if(fFile != 0){
		if(fTree != 0){
			if(SetBranchAddress(anEvent)){
				if(GetEntry(0) > 0)
					fEntries = fTree->GetEntries();
        theRet = true;
			}
		}
	}
  
	return theRet;
}


Bool_t  KEraRawEventReader::Open(const string fileName, EdwEvent **anEvent) 
{
  //same as OpenFile
  
  if(!OpenFile(fileName, anEvent)){
		cout << "KEraRawEventReader - Could not open " << fileName << endl;
    return false;
	}
  else return true;
}


Bool_t KEraRawEventReader::Close(Option_t *opt)
{
  if(fLocalEvent!=0) {
    delete fLocalEvent;
		//KEventFactory::DeleteEvent(fLocalEvent);
		fLocalEvent = 0;
	}
	return this->KEraRawFileIO::Close(opt);
}

Bool_t KEraRawEventReader::SetBranchAddress(EdwEvent **anEvent)
{
  //sets the top-level event branch adress to anEvent.
  
	if(fTree == 0) return false;
  
	if(anEvent == 0){
		TBranchElement *fBranch = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
		if(fBranch == 0) return false;
    
		fLocalEvent = new EdwEvent();
		if(fLocalEvent != 0)
			fTree->SetBranchAddress(GetBranchName().c_str(), &fLocalEvent);
		else
			return false;
	}
	else
		fTree->SetBranchAddress(GetBranchName().c_str(), anEvent);
  
	return true;
  
}

EdwEvent* KEraRawEventReader::GetEvent(void)
{
  //returns a pointer to EdwEvent, where the values for each EdwEvent
  //in the file will be found. Each time GetEntry (or GetNextEntry() )
  //is called, ROOT fills the EdwEvent with the values for that event.
  
	EdwEvent* event = 0;
	if(fTree !=0){
		if(!fTree->IsZombie())
		{
			TBranchElement *fB = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
			event = (EdwEvent *)fB->GetObject();
		}
	}
  
	return event;
}

Int_t KEraRawEventReader::GetNextEntry(void)
{
	return GetEntry(fCurrentEntry+1);
}

Int_t KEraRawEventReader::GetPreviousEntry(void)
{
	return  GetEntry(fCurrentEntry-1);
}

Int_t KEraRawEventReader::GetEntry(Int_t anEntry)
{
	//will load Entry number 'anEntry' and
	//if successful, will set the fCurrentEntry value to
	//anEntry.
  
	Int_t theRet = -1;
	if(fTree == 0) {
		fCurrentEntry = 0;
		return -1;
	}
	else {
		theRet = fTree->GetEntry(anEntry);
		if(theRet > 0)
			fCurrentEntry = anEntry;
    
		return theRet;
	}
}


TObject* KEraRawEventReader::Get(const string namecycle) const
{
  //Same as TFile::Get
  
	if(fFile != 0){
		return fFile->Get(namecycle.c_str());
	}
	else return 0;
}

