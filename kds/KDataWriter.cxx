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
	
	Close(); 
	
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
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kHLAClassName, anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<KRawEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kRawClassName, anEvent, 512000, 99));
			
		}
		else if( dynamic_cast<KHLaMCEvent*>(*anEvent) !=0 ) {
			//cout << typeid(*hlaEv).name() << endl;
			//cout << hlaEv << " " << anEvent << endl;
			fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), kHLaMCClassName, anEvent, 512000, 99));
			
		}
		else{
			cout << "KDataWriter::SetTreeBranch(KHLAEvent* ) Unsupported Event Class " << endl;
		}

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
		
		WriteTCuts(); //add the TCuts. 
				
		return fTree->Write(name, option, bufsize);  //add the tree
	}
	else {
		return -1;
	}

}

void KDataWriter::WriteTCuts(void)
{
	if(fFile == 0) return;
	
	if(fFile->IsOpen()){
		fFile->cd();
		/*
		TCut mCut0("fid401","fid401");
		mCut0 = "fDetectorName.fData == \"FID401\"";
		mCut0.Write("", TObject::kWriteDelete);  //kWriteDelete prevents multiple keys
		//being written to the file in case somebody calls Write multiple times
		
		TCut mCut1("fid402","fid402");
		mCut1 = "fDetectorName.fData == \"FID402\"";
		mCut1.Write("", TObject::kWriteDelete);
		
		TCut mCut2("id2","id2");
		mCut2 = "fDetectorName.fData == \"ID2\"";
		mCut2.Write("", TObject::kWriteDelete);
		
		TCut mCut3("id3","id3");
		mCut3 = "fDetectorName.fData == \"ID3\"";
		mCut3.Write("", TObject::kWriteDelete);
		
		TCut mCut4("id4","id4");
		mCut4 = "fDetectorName.fData == \"ID4\"";
		mCut4.Write("", TObject::kWriteDelete);

		TCut mCut5("id5","id5");
		mCut5 = "fDetectorName.fData == \"ID5\"";
		mCut5.Write("", TObject::kWriteDelete);
		
		TCut mCut6("id6","id6");
		mCut6 = "fDetectorName.fData == \"ID6\"";
		mCut6.Write("", TObject::kWriteDelete);
		
		TCut mCut7("id401","id401");
		mCut7 = "fDetectorName.fData == \"ID401\"";
		mCut7.Write("", TObject::kWriteDelete);

		TCut mCut8("id402","id402");
		mCut8 = "fDetectorName.fData == \"ID402\"";
		mCut8.Write("", TObject::kWriteDelete);
		
		TCut mCut9("id403","id403");
		mCut9 = "fDetectorName.fData == \"ID403\"";
		mCut9.Write("", TObject::kWriteDelete);
		
		TCut mCut10("id404","id404");
		mCut10 = "fDetectorName.fData == \"ID404\"";
		mCut10.Write("", TObject::kWriteDelete);
		
		TCut mCut11("id405","id405");
		mCut11 = "fDetectorName.fData == \"ID405\"";
		mCut11.Write("", TObject::kWriteDelete);
		
		TCut mCut12("fid801","fid801");
		mCut12 = "fDetectorName.fData == \"FID801\"";
		mCut12.Write("", TObject::kWriteDelete);

		TCut mCut13("fid802s","fid802s");
		mCut13 = "fDetectorName.fData == \"FID802s\"";
		mCut13.Write("", TObject::kWriteDelete);

		TCut mCut14("fid803","fid803");
		mCut14 = "fDetectorName.fData == \"FID803\"";
		mCut14.Write("", TObject::kWriteDelete);

		TCut mCut15("fid804","fid804");
		mCut15 = "fDetectorName.fData == \"FID804\"";
		mCut15.Write("", TObject::kWriteDelete);
		
		TCut mCut16("fid403","fid403");
		mCut16 = "fDetectorName.fData == \"FID403\"";
		mCut16.Write("", TObject::kWriteDelete);

		TCut mCut17("fid404","fid404");
		mCut17 = "fDetectorName.fData == \"FID404\"";
		mCut17.Write("", TObject::kWriteDelete);
		
		TCut mCut18("id4v2","id4v2");
		mCut18 = "fDetectorName.fData == \"ID4.v2\"";
		mCut18.Write("", TObject::kWriteDelete);
		
		TCut mCut19("id5v2","id5v2");
		mCut19 = "fDetectorName.fData == \"ID5.v2\"";
		mCut19.Write("", TObject::kWriteDelete);
		
		TCut mCut20("id402rel","id402rel");
		mCut20 = "fDetectorName.fData == \"ID402.Rel\"";
		mCut20.Write("", TObject::kWriteDelete);
		
		TCut mCut21("id403rel","id403rel");
		mCut21 = "fDetectorName.fData == \"ID403.Rel\"";
		mCut21.Write("", TObject::kWriteDelete);
		*/
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

