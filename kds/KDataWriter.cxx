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
#include "KAmpEvent.h"
#include <typeinfo>
#include <exception> 
#include <iostream>
#include "TClass.h"
#include "TCut.h"
#include "TList.h"
#include "KDataProcessingInfo.h"
#include "TSystem.h"
#include "TTimeStamp.h"

using namespace std;

ClassImp(KDataWriter);

KDataWriter::KDataWriter(void)
{
  fEventBranch = 0;
  fLocalEvent = 0;
  fTree = 0;
  fFile = 0;

}

KDataWriter::KDataWriter(const Char_t* name, const Char_t* eventType, 
  const Char_t* mode)
{	
  //standard constructor
  //name = file name 
  //eventType = KHLAEvent, KRawEvent, or HLaMC, plus any other objects based on KEvent.
  //The KEvent-based classes should all have a static method called GetClassName that retuns the name of the class.
  //Pass this result into eventType. For example, KHLAEvent::GetClassName() would create a KData file that holds
  //KHLAEvent objects (this is the default behavior). For raw events, pass KRawEvent::GetClassName(). 
  //This, of course, requires that the KEventFactory knows about the KEvent-based class. 
  //mode = recreate, update

  fEventBranch = 0;
  fLocalEvent = 0;
  fTree = 0;
  fFile = 0;
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
  fTree = 0;
  fFile = 0;
  OpenFile(name, event, mode);	
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

  if(fFile!=0)
  {
    if(fFile->IsOpen()){
      cout << "KDataWriter::OpenFile. You already have a file opened. Close it first." << endl;
      return false;
    }
  }
  //Close();   //hmmm... shouldn't this just go up there and close the old file if a new file is asked to be open?

  OpenFileForWriting(fileName, mode);

  if(!fFile->IsZombie()){

    if(SetTreeBranch(anEvent)){
      //cout << "KDataWriter Done Opening File" << endl;
      fTree->BranchRef(); //automatically assume that we're going to use TRefs
      //But, the user of this object will have to make sure they are dealing
      //with the object count properly. See the TRef documentation. 
      return true;
    }
    else return false;
  }
  else {
    cout << "KDataWriter File is Zombie." << endl;
    return false;
  }
}

TFile* KDataWriter::OpenFileForWriting(const Char_t* name, const Char_t* option, 
  const Char_t* title)
{

  fFile =  TFile::Open(name, option, title); 
  //MUST do this after initializing the TFile
  CreateTree();

  return fFile;
}

void KDataWriter::CreateTree(void)
{

  fFile->cd();
  
  if(fTree != 0) {
    delete fTree; fTree = 0;
  }

  fTree = new TTree(GetTreeName().c_str(), GetTreeTitle().c_str());

  if(fTree->IsZombie())
    cout << "KDataWriter::CreateTree fTree is Zombie" << endl;
  else{
    /*TList *myUserInfo = fTree->GetUserInfo();
    KDataProcessingInfo * mNewProcInfo = new KDataProcessingInfo;
    mNewProcInfo->SetSVNRev( gSystem->GetFromPipe("svnversion $KDATA_ROOT/").Data());
    mNewProcInfo->SetHostName( gSystem->GetFromPipe("hostname").Data());
    mNewProcInfo->SetHostInfo( gSystem->GetFromPipe("uname -a").Data());
    mNewProcInfo->SetUserInfo( (gSystem->GetUserInfo()->fUser).Data() );
    TTimeStamp fNow;
    mNewProcInfo->SetStartTimeOfProcess(fNow);
    if(myUserInfo->GetEntries() > 0){
      mNewProcInfo->SetPrevProcInfo( *(KDataProcessingInfo *)(myUserInfo->At( myUserInfo->GetEntries()-1 )) );
    }
    myUserInfo->Add(mNewProcInfo);*/
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
            //cout << "trying to create a Branch and cast it as TBrachElement*" << endl;
      fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KRawEvent::GetClassName(), anEvent, 512000, 99));
      //cout << "dynamic_cast worked" << endl;
    }
    else if( dynamic_cast<KHLaMCEvent*>(*anEvent) !=0 ) {
      //cout << typeid(*hlaEv).name() << endl;
      //cout << hlaEv << " " << anEvent << endl;
      fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KHLaMCEvent::GetClassName(), anEvent, 512000, 99));

    }
    else if( dynamic_cast<KAmpEvent*>(*anEvent) !=0 ) {
      //cout << typeid(*hlaEv).name() << endl;
      //cout << hlaEv << " " << anEvent << endl;
      fEventBranch = dynamic_cast<TBranchElement*>(fTree->Branch(GetBranchName().c_str(), KAmpEvent::GetClassName(), anEvent, 512000, 99));

    }
    else{
      cout << "KDataWriter::SetTreeBranch. Unsupported Event Class " << endl;
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

  if(fFile==0) return 0;
  if(fFile->IsOpen()==0) return 0;
  
  KEvent *event = 0;
  if(fEventBranch !=0){
    if(!fEventBranch->IsZombie())
    {
      event = (KEvent*)fEventBranch->GetObject();

      if(fLocalEvent != 0){
        if(fLocalEvent != event){
          cout << "Surprisng result. Please don't print this message. If you see this, please report it to Adam Cox immediately. :)" << endl;
        }
      }

    }
  }

  return event;
}


Int_t KDataWriter::Fill(void)
{
  //Fills the data in the current KEvent pointer to the file. 
  //This essentially calls TTree::Fill. 
  if(fFile == 0) return -1;
  
  if(fTree!=0) {
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
  //if(!IsReady()) return -1;

  if(fFile->IsOpen()){
    fFile->cd();
    return fTree->Write(name, option, bufsize);  //add the tree
  }
  else {
    return -1;
  }

}

Bool_t KDataWriter::Close(Option_t * opt)
{
  //closes the file.
  if(fFile==0) return true;
  
  if (!fFile->TestBit(TObject::kNotDeleted)) {
    return true;
  }
  
  fFile->Close(opt);  //does this take care of all of the memory?? or must I delete something???
  if(fFile->IsOpen() == 0){
    fFile = 0;
    fTree = 0;
    if(fLocalEvent){
      delete fLocalEvent;
      fLocalEvent = 0;
    }
    fEventBranch = 0;
    return true;
  }
  else return false;
  
}

const char* KDataWriter::GetEventClassName(void) const
{
  if(fTree != 0){
    TBranchElement *branch = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
    if(branch != 0)
      return branch->GetClassName();

    else return "";
  }
  else return "";
}

const char* KDataWriter::GetFileName(void) const
{
  if (fFile != 0) {
    return fFile->GetName();
  }
  else return "";

}


void KDataWriter::ls(Option_t *anOpt) const
{
  if(fFile != 0){
    fFile->ls(anOpt);
  }

}

Bool_t KDataWriter::cd(const char *path)
{
  if(fFile != 0){
    return fFile->cd(path);
  }
  else return false;
}

Bool_t KDataWriter::IsReady(void) const
{
  if(fFile!=0)
  {
    if(fFile->IsOpen()){
      if(fTree != 0)
        return true;
    }
  }
    
  return false;
}
/*
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

  fTree = (TTree*) treeIn->CloneTree(nEnt, anOpt);

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

  fTree = (TTree*) TTree::MergeTrees(li,anOpt);

  return fTree;
}

*/
