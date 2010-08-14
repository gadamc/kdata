//_____________________________________________
//
// KDataReader.cc
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 4/30/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class reads in an KDS file and provides some basic functionality 
// to access the Event data and to loop through the file, event-by-event.
//


#include "KDataReader.h"
#include "KEvent.h"
#include "KEventFactory.h"
#include <typeinfo>
#include <iostream>
#include "TClass.h"
#include <string>

using namespace std;

ClassImp(KDataReader);

KDataReader::KDataReader(const Char_t* fileName, KEvent **anEvent)
{
	//TClass::GetClass("TBits")->IgnoreTObjectStreamer(),
	
	fLocalEvent = 0;
	InitializeMembers();
  if(!OpenFile(fileName, anEvent)){
		cout << "KDataReader - Could not open " << fileName << endl;
	}
}


KDataReader::KDataReader(const KDataReader &/*aWriter*/)
:KDataFileIO()
{
	//cout << "May Not Copy" << endl;
	//cout << "Create a new instance of KDataReader. " << endl;
	//cout << "And, you shouldn't have two readers to the same file anyways. " << endl;
}

KDataReader& KDataReader::operator=( const KDataReader &/*aWriter*/)
{
	//cout << "May Not Copy" << endl;
	//cout << "Create a new instance of KTreeRedaer. " << endl;
	//cout << "And, you shouldn't have two readers to the same file anyways. " << endl;
	return *this;
}

KDataReader::~KDataReader(void)
{
  Close();
}

void KDataReader::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fCurrentEntry = 0;
	fEntries = 0;
	fGSEventIndex=0;
}


Bool_t  KDataReader::OpenFile(const Char_t* fileName, KEvent **anEvent)
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

Bool_t KDataReader::Close(Option_t *opt)
{
	return this->KDataFileIO::Close(opt);
	if(fLocalEvent!=0) {
		KEventFactory::DeleteEvent(fLocalEvent);
		fLocalEvent = 0;
	}
		
}

Bool_t KDataReader::SetBranchAddress(KEvent **anEvent)
{
	if(fTree == 0) return false;
	
	if(anEvent == 0){ 
		TBranchElement *fBranch = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
		if(fBranch == 0) return false;
		
		fLocalEvent = KEventFactory::NewEvent(fBranch->GetClassName());
		if(fLocalEvent != 0)
			fTree->SetBranchAddress(GetBranchName().c_str(), &fLocalEvent);
		else 
			return false; 
	}
	else 
		fTree->SetBranchAddress(GetBranchName().c_str(), anEvent);
	
	return true;
	
}

KEvent* KDataReader::GetEvent(void)
{
	KEvent* event = 0;
	if(fTree !=0){
		if(!fTree->IsZombie())
		{
			TBranchElement *fB = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
			event = (KEvent *)fB->GetObject();
		}
	}
	
	return event;
}

Int_t KDataReader::GetNextEntry(void)
{
	return GetEntry(fCurrentEntry+1);
}

Int_t KDataReader::GetPreviousEntry(void)
{
	return  GetEntry(fCurrentEntry-1);
}

Int_t KDataReader::GetEntry(Int_t anEntry)
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

Int_t KDataReader::GetEntryWithGSEventNumber(Int_t anEntry)
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

void KDataReader::ls(Option_t *anOpt) const
{
	if(fFile != 0){
		fFile->ls(anOpt);
	}
	
}

TObject* KDataReader::Get(const Char_t* namecycle) const
{
	if(fFile != 0){
		return fFile->Get(namecycle);
	}
	else return 0;
}

//anEntry has to be the entry number for the allBolosVetoFile it is the same as GSEventNumber, but not for skimmed files
Int_t KDataReader::GetNextMuonEntry(Int_t anEntry) 
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

Int_t KDataReader::GetPreviousMuonEntry(Int_t anEntry) 
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

Int_t KDataReader::GetNextBoloEntry(Int_t anEntry) 
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

Int_t KDataReader::GetPreviousBoloEntry(Int_t anEntry) 
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
