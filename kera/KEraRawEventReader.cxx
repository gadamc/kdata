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
#include "TClass.h"
#include <string>

using namespace std;

ClassImp(KEraRawEventReader);

KEraRawEventReader::KEraRawEventReader(const Char_t* fileName, EdwEvent **anEvent)
{
	//TClass::GetClass("TBits")->IgnoreTObjectStreamer(),

	fLocalEvent = 0;
	InitializeMembers();
  if(!OpenFile(fileName, anEvent)){
		cout << "KEraRawEventReader - Could not open " << fileName << endl;
	}

}


KEraRawEventReader::KEraRawEventReader(const KEraRawEventReader& aWriter)
:KEraRawFileIO()
{
	//cout << "May Not Copy" << endl;
	//cout << "Create a new instance of KEraRawEventReader. " << endl;
	//cout << "And, you shouldn't have two readers to the same file anyways. " << endl;
}

KEraRawEventReader& KEraRawEventReader::operator=( const KEraRawEventReader& aWriter)
{
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
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fCurrentEntry = 0;
	fEntries = 0;
//	fGSEventIndex=0;
}


Bool_t  KEraRawEventReader::OpenFile(const Char_t* fileName, EdwEvent **anEvent)
{
	Close();  //close the file, if one is already open.
	fFile = OpenFileForReading(fileName);
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

Bool_t KEraRawEventReader::Close(Option_t *opt)
{
	return this->KEraRawFileIO::Close(opt);
	if(fLocalEvent!=0) {
	    delete fLocalEvent;
		//KEventFactory::DeleteEvent(fLocalEvent);
		fLocalEvent = 0;
	}

}

Bool_t KEraRawEventReader::SetBranchAddress(EdwEvent **anEvent)
{
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


/*
Int_t KEraRawEventReader::GetEntryWithGSEventNumber(Int_t anEntry)
{
	if(fGSEventIndex==0){
		cout << "Building index..." << endl;
		fTree->BuildIndex("fGSEventNumber");
		fGSEventIndex=1;
	}

	//will load (if possible) Entry cooresponding to GSEventNumber'anEntry' and
	//if successful, will set the fCurrentEntry value to
	//anEntry. Otherwise returnvalue should be -1.

	Int_t theRet = -1;
	if(fTree == 0) {
		fCurrentEntry = 0;
		return -1;
	}
	else {
		theRet = fTree->GetEntryWithIndex(anEntry);
		if(theRet > 0)
			fCurrentEntry = fTree->GetEntryNumberWithIndex(anEntry);

		return theRet;
	}
}

*/

TObject* KEraRawEventReader::Get(const Char_t* namecycle) const
{
	if(fFile != 0){
		return fFile->Get(namecycle);
	}
	else return 0;
}

//anEntry has to be the entry number for the allBolosVetoFile it is the same as GSEventNumber, but not for skimmed files

/*
Int_t KEraRawEventReader::GetNextMuonEntry(Int_t anEntry)
{
	Int_t size=0;
	KHLAEvent* e=0;
	e = dynamic_cast<KHLAEvent*>(GetEvent());
	if(e==0)return -1;

	if(anEntry<0){
		size=GetNextEntry();
	}
	else{
		size=GetEntry(anEntry+1);
	}
	while(size>0 && e->GetNumMuonModules()==0){
	size=GetNextEntry();
	}
	return size;
}

Int_t KEraRawEventReader::GetPreviousMuonEntry(Int_t anEntry)
{
	Int_t size=0;
	KHLAEvent* e=0;
	e = dynamic_cast<KHLAEvent*>(GetEvent());
	if(e==0)return -1;
	if(anEntry<0)
		size=GetPreviousEntry();
	else
		size=GetEntry(anEntry-1);
	while(size>0 && e->GetNumMuonModules()==0){
	size=GetPreviousEntry();
	}
	return size;
}

Int_t KEraRawEventReader::GetNextBoloEntry(Int_t anEntry)
{
	Int_t size=0;
	KHLAEvent* e=0;
	e = dynamic_cast<KHLAEvent*>(GetEvent());
	if(e==0)return -1;
	if(anEntry<0)
		size=GetNextEntry();
	else
		size=GetEntry(anEntry+1);
	while(size>0 && e->GetNumBolos()==0){
	size=GetNextEntry();
	}
	return size;
}

Int_t KEraRawEventReader::GetPreviousBoloEntry(Int_t anEntry)
{
	Int_t size=0;
	KHLAEvent* e=0;
	e = dynamic_cast<KHLAEvent*>(GetEvent());
	if(e==0)return -1;
	if(anEntry<0)
		size=GetPreviousEntry();
	else
		size=GetEntry(anEntry-1);
	while(size>0 && e->GetNumBolos()==0){
	size=GetPreviousEntry();
	}
	return size;
}
*/

