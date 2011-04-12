//_____________________________________________
//
// KDataWriter.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 4/30/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class writes out a KData file. The basic use is, after creating
// an instance of this class, get a pointer to an event object, set the 
// various data parameters in the KEvent and then call KDataWriter::Fill
// to add the event to an output tree. This file creates an KData file with
// trees filled with KHLAEvents, KRawEvents and KHLaMCEvents.
//
// This class automatically assume that we're going to use TRefs, by calling
// TTree::BranchRef when opening the file. 
// But, the user of this object will have to make sure they are dealing
// with the object count properly. See the TRef documentation. 
//

#include "KDataWriter.h"
#include "KEventFactory.h"
#include "KEvent.h"
#include "KRawEvent.h"
#include "KHLAEvent.h"
#include "KHLaMCEvent.h"
#include <typeinfo>
#include <exception> 
#include <iostream>
#include "TClass.h"
#include "TCut.h"

using namespace std;

ClassImp(KDataWriter);

KDataWriter::KDataWriter(void)
{
  fEventBranch = 0;
	fLocalEvent = 0;
	bIsReady = false;
}

KDataWriter::KDataWriter(const Char_t* name, const Char_t* eventType, 
												 const Char_t* mode)
{	
  //standard constructor
	//name = file name 
  //eventType = HLA, Raw, or HLaMC, plus any other objects based on KEvent
  //mode = recreate, update
	
	fEventBranch = 0;
	fLocalEvent = 0;
	bIsReady = false;
	OpenFile(name, eventType, mode);
}

KDataWriter::KDataWriter(const Char_t* name, KEvent** event, 
												 const Char_t* mode)
{
	//standard constructor
	//name = file name 
  //KEvent** = the address of a pointer to an event object that inherits from KEvent
  //mode = recreate (default), update
	
	fEventBranch = 0;
	fLocalEvent = 0;
	bIsReady = false;
	OpenFile(name, event, mode);	
}

KDataWriter::KDataWriter(const KDataWriter &/*aWriter*/)
: KDataFileIO()
{

}

KDataWriter& KDataWriter::operator=(const KDataWriter &/*aWriter*/)
{
  
	return *this;
}

KDataWriter::~KDataWriter(void)
{
	Close();
}
 
Bool_t KDataWriter::OpenFile(const Char_t* fileName, const Char_t* eventType,
															 const Char_t* mode)
{
	//Open fileName for writing. eventType = HLA, Raw, HLaMC, etc..
  //mode = recreate (default), or update
  
	if(fLocalEvent == 0)
		fLocalEvent = KEventFactory::NewEvent(eventType);
	
	return OpenFile(fileName, &fLocalEvent, mode);
	
}


Bool_t KDataWriter::OpenFile(const Char_t* fileName, KEvent **anEvent,
															 const Char_t* mode)
{
  //Open fileName for writing. KEvent** = address of the pointer
  //to an object that inherits from KEvent
  //mode = recreate (default), or update
  
	if(bIsReady)
	{
		cout << "KDataWriter::OpenFile. You already have a file opened. Close it first." << endl;
		return false;
	}
	
	Close();   //hmmm... shouldn't this just go up there and close the old file if a new file is asked to be open?
	
	TFile *mFile = OpenFileForWriting(fileName, mode);
	
	if(!mFile->IsZombie()){
		
		if(SetTreeBranch(anEvent)){
			//cout << "KDataWriter Done Opening File" << endl;
			fTree->BranchRef(); //automatically assume that we're going to use TRefs
			//But, the user of this object will have to make sure they are dealing
			//with the object count properly. See the TRef documentation. 
			bIsReady = true;
			return true;
		}
		else return false;
	}
	else {
		cout << "KDataWriter File is Zombie." << endl;
		return false;
	}
}

	
Bool_t KDataWriter::SetTreeBranch(KEvent **anEvent)
{
	//This method must be called After calling OpenFileForWriting
	//because the order of the creation of the TFile and the TTree 
	//matters to ROOT
	
	if(fTree == 0){
		cout << "KDataWriter SetTreeBranch - fail becaust fTree == 0" << endl;
		return false;
	}

	fEventBranch = 0;
	
	try{
		
		if( dynamic_cast<KHLAEvent*>(*anEvent) != 0) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << &hlaEv << " " << &anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KHLAEvent::GetClassName(), anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<KRawEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KRawEvent::GetClassName(), anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<KHLaMCEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KHLaMCEvent::GetClassName(), anEvent, 512000, 99));
			
		}
		else{
			cout << "KDataWriter::SetTreeBranch(KHLAEvent* ) Unsupported Event Class " << endl;
		}

    //fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), **anEvent.ClassName(), anEvent, 512000, 99));
	}
	catch (exception& e) {

		cout << "Exception: " << e.what() << endl;
		return false;
	}
	
	if(fEventBranch != 0){
		if(fEventBranch->IsZombie()){
			cout << fEventBranch->GetClassName() << endl; 
			cout << "KDataWriter::SetTreeBranch. TBranch is Zombie" << endl;
			return false;
		}
		else return true;
	}
	else {
		return false;
	}

	
}

KEvent* KDataWriter::GetEvent(void)
{
  //Once you've opened your KDataFile, use this method to get a 
  //pointer to the event object. This method returns a KEvent*, but
  //you will probably want to cast that pointer into the appropriate type
  //either a KRawEvent or KHLAEvent. For example, let's say you're 
  //doing analysis with a KHLAEvent, you would do the following
  //
  //KDataWriter myFile("/path/to/myKHLADataFile.root");
  //KHLAEvent *e = (KHLAEvent *)myFile.GetEvent();
  //
  
	if(!bIsReady) return 0;
	
	KEvent *event = 0;
	if(fEventBranch !=0){
		if(!fEventBranch->IsZombie())
		{
			event = (KEvent*)fEventBranch->GetObject();
			
			if(fLocalEvent != 0){
				if(fLocalEvent != event){
					cout << "Surprisng result. Please don't print this message. :)" << endl;
				}
			}
			
		}
	}
	
	return event;
}

Bool_t KDataWriter::IsReady(void) const
{
  //returns true if the file is open and ready to start writing data.
  
	return bIsReady;
}

Int_t KDataWriter::Fill(void)
{
  //Fills the data in the current KEvent pointer to the file. 
  //This essentially calls TTree::Fill. 
  
	if(IsReady()) {			
		fFile = fTree->GetCurrentFile(); //new files will automatically be created
		//if the file exceeds fgMaxTreeSize, which is 100 Gigabytes by default
		//therefore, we must make sure we point to the right file. 
		//see the documentation in TTree::ChangeFile(TFile *).
		KEvent *e = GetEvent();
		if(e!=0) e->Compact();  //if this were to fail, then something would be terribly wrong
		
		fFile->cd();
		return fTree->Fill();
	}
	else 
		return -1;
}

Int_t KDataWriter::Write(const Char_t *name, Int_t option, Int_t bufsize)
{
  //Final write of the TTree to the output file name
	//The options are the same as those options in TObject::Write
	
	if(fFile == 0) return -1;
	if(!IsReady()) return -1;
	
	if(fFile->IsOpen()){
		fFile->cd();
		
				
		return fTree->Write(name, option, bufsize);  //add the tree
	}
	else {
		return -1;
	}

}

Bool_t KDataWriter::Close(Option_t *opt)
{
  //closes the file.
  
	//cout << "KDataWriter Close"<< endl;
	if(bIsReady == false)
		return true; //we've already closed the file. 
	
	fEventBranch = 0;
	if(fLocalEvent != 0){
		if(KEventFactory::DeleteEvent(fLocalEvent))
			fLocalEvent = 0;

	}
	bIsReady = !KDataFileIO::Close(opt);
	
	return !bIsReady;
}


TTree* KDataWriter::CloneTree(TTree *treeIn, Long64_t nEnt, Option_t* anOpt)
{
  //Clones the treeIn and sets the resulting pointer from treeIn->CloneTree
  //to the local TTree pointer, fTree.
  
	if(fTree != 0)
		delete fTree; fTree = 0;
	
	if(fLocalEvent != 0)
		KEventFactory::DeleteEvent(fLocalEvent); fLocalEvent = 0;
	
	//if we clone the tree from treeIn, then we don't use
	//the local event object memory space. So, we should delete it. 
	
	fTree = treeIn->CloneTree(nEnt, anOpt);
	
	return fTree;
}

TTree *KDataWriter::ConcatenateTrees(TList* li, Option_t *anOpt)
{
  //Calls the static function TTree::MergeTree(TList*). The returned
  //pointer to TTree is set to the local fTree and returned by this method.
  
	if(fTree != 0)
		delete fTree; fTree = 0;
	
	if(fLocalEvent != 0)
		KEventFactory::DeleteEvent(fLocalEvent); fLocalEvent = 0;
	
	fTree = TTree::MergeTrees(li,anOpt);
	
	return fTree;
}

