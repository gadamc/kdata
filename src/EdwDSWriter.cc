//_____________________________________________
//
// EdwDSWriter.cc
// EdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 4/30/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class writes out an EdwDS file. The basic use is, after creating
// an instance of this class, get a pointer to an event object, set the 
// various data parameters in the EdwDS file and then call EdwDSWriter::Fill
// to add the event to an output tree. This file creates an EdwDS file with
// trees filled with EdwHLAEvents, but you can also select Raw and HLaMC events.
//

#include "EdwDSWriter.h"
#include "EdwEventFactory.h"
#include "EdwEventBase.h"
#include "EdwRawEvent.h"
#include "EdwHLAEvent.h"
#include "EdwHLaMCEvent.h"
//#include "TBranchElement.h"
#include <typeinfo>
#include <exception> 
#include <iostream>
#include "TClass.h"
#include "TCut.h"

using namespace std;

ClassImp(EdwDSWriter);


EdwDSWriter::EdwDSWriter(const Char_t* name, const Char_t* eventType, 
												 const Char_t* mode)
{	
	//TClass::GetClass("TBits")->IgnoreTObjectStreamer();
	
	fEventBranch = 0;
	fLocalEvent = 0;
	bIsReady = false;
	OpenFile(name, eventType, mode);
}

EdwDSWriter::EdwDSWriter(const Char_t* name, EdwEventBase** event, 
												 const Char_t* mode)
{
	//TClass::GetClass("TBits")->IgnoreTObjectStreamer();
	
	fEventBranch = 0;
	fLocalEvent = 0;
	bIsReady = false;
	OpenFile(name, event, mode);	
}

EdwDSWriter::EdwDSWriter(const EdwDSWriter &/*aWriter*/)
: EdwDSFileIO()
{
	//cout << "May Not Copy - Do not want to cause confusion by writing to the same file with different objects." << endl;
	//cout << "Create a new instance of EdwDSWriter. " << endl;
	//cout << "And, you shouldn't have two writers to the same file anyways. " << endl;
}

EdwDSWriter& EdwDSWriter::operator=(const EdwDSWriter &/*aWriter*/)
{
	//cout << "May Not Copy - Do not want to cause confusion by writing to the same file with different objects." << endl;
	//cout << "Create a new instance of EdwDSWriter. " << endl;
	//cout << "And, you shouldn't have two writers to the same file anyways. " << endl;
	return *this;
}

EdwDSWriter::~EdwDSWriter(void)
{
	Close();
}
 
Bool_t EdwDSWriter::OpenFile(const Char_t* fileName, const Char_t* eventType,
															 const Char_t* mode)
{
	
	if(fLocalEvent == 0)
		fLocalEvent = EdwEventFactory::NewEvent(eventType);
	
	return OpenFile(fileName, &fLocalEvent, mode);
	
}


Bool_t EdwDSWriter::OpenFile(const Char_t* fileName, EdwEventBase **anEvent,
															 const Char_t* mode)
{
	if(bIsReady)
	{
		cout << "EdwDSWriter::OpenFile. You already have a file opened. Close it first." << endl;
		return false;
	}
	
	Close(); 
	
	TFile *mFile = OpenFileForWriting(fileName, mode);
	
	if(!mFile->IsZombie()){
		
		if(SetTreeBranch(anEvent)){
			//cout << "EdwDSWriter Done Opening File" << endl;
			bIsReady = true;
			return true;
		}
		else return false;
	}
	else {
		cout << "EdwDSWriter File is Zombie." << endl;
		return false;
	}
}

	
Bool_t EdwDSWriter::SetTreeBranch(EdwEventBase **anEvent)
{
	//This method must be called After calling OpenFileForWriting
	//because the order of the creation of the TFile and the TTree 
	//matters to ROOT
	
	if(fTree == 0){
		cout << "EdwDSWriter SetTreeBranch - fail becaust fTree == 0" << endl;
		return false;
	}

	fEventBranch = 0;
	
	try{
		
		if( dynamic_cast<EdwHLAEvent*>(*anEvent) != 0) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << &hlaEv << " " << &anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kHLAClassName, anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<EdwRawEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kRawClassName, anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<EdwHLaMCEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kHLaMCClassName, anEvent, 512000, 99));
			
		}
		else{
			cout << "EdwDSWriter::SetTreeBranch(EdwHLAEvent* ) Unsupported Event Class " << endl;
		}

	}
	catch (exception& e) {

		cout << "Exception: " << e.what() << endl;
		return false;
	}
	
	if(fEventBranch != 0){
		if(fEventBranch->IsZombie()){
			cout << fEventBranch->GetClassName() << endl; 
			cout << "EdwDSWriter::SetTreeBranch. TBranch is Zombie" << endl;
			return false;
		}
		else return true;
	}
	else {
		return false;
	}

	
}

EdwEventBase* EdwDSWriter::GetEvent(void)
{
	if(!bIsReady) return 0;
	
	EdwEventBase *event = 0;
	if(fEventBranch !=0){
		if(!fEventBranch->IsZombie())
		{
			event = (EdwEventBase*)fEventBranch->GetObject();
			
			if(fLocalEvent != 0){
				if(fLocalEvent != event){
					cout << "Surprisng result. Please don't print this message. :)" << endl;
				}
			}
			
		}
	}
	
	return event;
}

Bool_t EdwDSWriter::IsReady(void) const
{
	return bIsReady;
}

Int_t EdwDSWriter::Fill(void)
{
	if(IsReady()) {			
		fFile = fTree->GetCurrentFile(); //new files will automatically be created
		//if the file exceeds fgMaxTreeSize, which is 100 Gigabytes by default
		//therefore, we must make sure we point to the right file. 
		//see the documentation in TTree::ChangeFile(TFile *).
		EdwEventBase *e = GetEvent();
		if(e!=0) e->Compact();  //if this were to fail, then something would be terribly wrong
		
		fFile->cd();
		return fTree->Fill();
	}
	else 
		return -1;
}

Int_t EdwDSWriter::Write(const Char_t *name, Int_t option, Int_t bufsize)
{
	//The options are the same as those options in TObject::Write
	
	if(fFile == 0) return -1;
	if(!IsReady()) return -1;
	
	if(fFile->IsOpen()){
		fFile->cd();
		
		WriteTCuts(); //add the TCuts. 
				
		return fTree->Write(name, option, bufsize);  //add the tree
	}
	else {
		return -1;
	}

}

void EdwDSWriter::WriteTCuts(void)
{
	if(fFile == 0) return;
	
	if(fFile->IsOpen()){
		fFile->cd();
		
		TCut mCut0("fid401","fid401");
		mCut0 = "fDetectorNumber == 0";
		mCut0.Write("", TObject::kWriteDelete);  //kWriteDelete prevents multiple keys
		//being written to the file in case somebody calls Write multiple times
		//however, this assurances is not applied to the tree. So the user
		//should be aware of this.
		
		TCut mCut1("fid402","fid402");
		mCut1 = "fDetectorNumber == 9";
		mCut1.Write("", TObject::kWriteDelete);
		
		TCut mCut2("id2","id2");
		mCut2 = "fDetectorNumber == 32";
		mCut2.Write("", TObject::kWriteDelete);
		
		TCut mCut3("id3","id3");
		mCut3 = "fDetectorNumber == 11";
		mCut3.Write("", TObject::kWriteDelete);
		
		TCut mCut4("id4","id4");
		mCut4 = "fDetectorNumber == 23";
		mCut4.Write("", TObject::kWriteDelete);

		TCut mCut5("id5","id5");
		mCut5 = "fDetectorNumber == 5";
		mCut5.Write("", TObject::kWriteDelete);
		
		TCut mCut6("id6","id6");
		mCut6 = "fDetectorNumber == 17";
		mCut6.Write("", TObject::kWriteDelete);
		
		TCut mCut7("id401","id401");
		mCut7 = "fDetectorNumber == 14";
		mCut7.Write("", TObject::kWriteDelete);

		TCut mCut8("id402","id402");
		mCut8 = "fDetectorNumber == 26";
		mCut8.Write("", TObject::kWriteDelete);
		
		TCut mCut9("id403","id403");
		mCut9 = "fDetectorNumber == 2";
		mCut9.Write("", TObject::kWriteDelete);
		
		TCut mCut10("id404","id404");
		mCut10 = "fDetectorNumber == 20";
		mCut10.Write("", TObject::kWriteDelete);
		
		TCut mCut11("id405","id405");
		mCut11 = "fDetectorNumber == 29";
		mCut11.Write("", TObject::kWriteDelete);
		
	}
	
}

Bool_t EdwDSWriter::Close(Option_t *opt)
{
	//cout << "EdwDSWriter Close"<< endl;
	if(bIsReady == false)
		return true; //we've already closed the file. 
	
	fEventBranch = 0;
	if(fLocalEvent != 0){
		if(EdwEventFactory::DeleteEvent(fLocalEvent))
			fLocalEvent = 0;
	}
	bIsReady = !EdwDSFileIO::Close(opt);
	
	return !bIsReady;
}


TTree* EdwDSWriter::CloneTree(TTree *treeIn, Long64_t nEnt, Option_t* anOpt)
{
	if(fTree != 0)
		delete fTree; fTree = 0;
	
	if(fLocalEvent != 0)
		EdwEventFactory::DeleteEvent(fLocalEvent); fLocalEvent = 0;
	
	//if we clone the tree from treeIn, then we don't use
	//the local event object memory space. So, we should delete it. 
	
	fTree = treeIn->CloneTree(nEnt, anOpt);
	
	return fTree;
}

TTree *EdwDSWriter::ConcatenateTrees(TList* li, Option_t *anOpt)
{
	if(fTree != 0)
		delete fTree; fTree = 0;
	
	if(fLocalEvent != 0)
		EdwEventFactory::DeleteEvent(fLocalEvent); fLocalEvent = 0;
	
	fTree = TTree::MergeTrees(li,anOpt);
	
	return fTree;
}

