//_____________________________________________
//
// KDataReader.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 4/30/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class reads in a KData file and provides some basic functionality 
// to access the Event data and to loop through the file, event-by-event.
// It will return a pointer to the event object in memory where each 
// event's data is stored. 
//


#include "KDataReader.h"
#include "KEvent.h"
#include "KEventFactory.h"
#include <typeinfo>
#include <iostream>
#include <string>
#include <exception> 

using namespace std;

ClassImp(KDataReader);

KDataReader::KDataReader(void)
{
  InitializeMembers();
}

KDataReader::KDataReader(const Char_t* fileName, KEvent **anEvent, Bool_t useCache)
{
  //standard constructor. You must at least provide a fileName. If you
  //have a particular object in memory where you want the data to be
  //stored, then you should pass in the address of the pointer to that
  //event, anEvent. For example
  //
  //KHLAEvent *e = KEventFactory::NewHLAEvent();
  //KDataReader file("/path/to/a/KHLAEventfile.root", &e);
  //
  //This object can open high level files (which hold KHLAEvents)
  //and raw level files (which hold KRawEvents)

  InitializeMembers();
  if(!OpenFile(fileName, anEvent, useCache)){
    cout << "KDataReader - Could not open " << fileName << endl;
  }
}


KDataReader::~KDataReader(void)
{
  Close();
}

void KDataReader::InitializeMembers(void)
{
  //

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  fCurrentEntry = 0;
  fEntries = 0;
  fGSEventIndex=0;
  fFile = 0;
  fTree = 0;
  fLocalEvent = 0;
  
}


Bool_t KDataReader::OpenFile(const Char_t* fileName, KEvent **anEvent, Bool_t useCache)
{
  //closes any file that is already open and then opens fileName.
  //Also, the pointer to the event returned by GetEvent could change after calling this method,
  //so its best to call GetEvent again before you try to access the event data.
  //returns true if successful. 

  if(fFile != 0){
    Close();  //close the file, if one is already open. 
  }
    
  OpenFileForReading(fileName);
  Bool_t theRet = false;

  if(fFile != 0){
    if(fTree != 0){
      if(SetBranchAddress(anEvent)){
        SetUseInternalCache(useCache);
        if(GetEntry(0) > 0)
          fEntries = fTree->GetEntries();
        theRet = true;
      }
    }
  }

  return theRet;
}

TFile* KDataReader::OpenFileForReading(const Char_t* name)
{

  fFile = TFile::Open(name, "read");
  if (fFile == 0){
    cout << "KDataFileIO - Unable to Open file: " << name << endl;
    return fFile;
  }
  GetTreePointerInFile();

  return fFile;
}

void KDataReader::GetTreePointerInFile(void)
{

  if (fFile == 0){
    cout << "KDataFileIO - File Pointer is NULL" << endl;
    return;
  }

  fFile->cd();
  fTree = (TTree *)fFile->Get(GetTreeName().c_str());

}


Bool_t KDataReader::Close(Option_t *opt)
{
  //close the file with option opt. See TFile::Close(Option_t*)

  if(fFile == 0){
    fTree=0; //the tree should have been deleted
    
    if(fLocalEvent!=0) {
      delete fLocalEvent;
      fLocalEvent = 0;
    }
    
    return true;
  }

  if (!fFile->TestBit(TObject::kNotDeleted)) {
    
    if(fLocalEvent!=0) {
      delete fLocalEvent;
      fLocalEvent = 0;
    }
    
    return true;
  }

  if(fFile->IsOpen())
    fFile->Close(opt);

  if (fFile->IsOpen()) {
    return false;
    //return false if we couldn't close the file.
  }
  else {
    delete fFile; 
    fFile = 0;
    fTree= 0; //the chain is deleted by the TFile
    if(fLocalEvent!=0) {
      delete fLocalEvent;
      fLocalEvent = 0;
    }
  }

  return true;

}

void KDataReader::SetUseInternalCache(Bool_t option)
{
  //This method calls the new TTree::AddBranchToCache method
  //which speeds up reading of the data. However, it does add to the 
  //memory overhead by around 30 Mb. If you do not want to use cacheing,
  //you must declare so in the KDataReader constructor, or when using
  //the KDataReader::OpenFile method. 
  //
  //Currenly, there's no way of telling ROOT to undo this.
  //If you've previously called this method with option = true, then
  //all branches in the tree are added to the cache.
  //

  if(option && fTree != 0){
    fTree->SetCacheSize(20000000);
    fTree->AddBranchToCache("*", true);
  }
  //else 
  //cerr << "KDataReader::SetUseInternalCache. There's no way of undoing this." << endl;
  //cerr << "  If you've previously called this method with option = true, then" << endl;
  //cerr << "  all branches in the tree are added to the cache." << endl; 
  //
}


Bool_t KDataReader::SetBranchAddress(KEvent **anEvent)
{
  //set the branch address to anEvent.

  if(fTree == 0) return false;

  if(anEvent == 0){ 
    TBranchElement *fBranch = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
    if(fBranch == 0) return false;

    fLocalEvent = KEventFactory::NewEvent(fBranch->GetClassName());
    if(fLocalEvent != 0) {
      fTree->SetBranchAddress(GetBranchName().c_str(), &fLocalEvent);
    }
    else 
      return false; 
  }
  else 
    fTree->SetBranchAddress(GetBranchName().c_str(), anEvent);

  return true;

}

KEvent* KDataReader::GetEvent(void)
{
  //Once you've opened your KDataFile, use this method to get a 
  //pointer to the event object. This method returns a KEvent*, but
  //you will probably want to cast that pointer into the appropriate type
  //either a KRawEvent or KHLAEvent. For example, let's say you're 
  //doing analysis with a KHLAEvent, you would do the following
  //
  //KDataReader myFile("/path/to/myKHLADataFile.root");
  //KHLAEvent *e = (KHLAEvent *)myFile.GetEvent();
  //


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
  //Loads the event with data from the next entry in the tree
  //returns the size of the event in bytes.

  return GetEntry(fCurrentEntry+1);
}

Int_t KDataReader::GetPreviousEntry(void)
{
  //Loads the event with data from the previous entry in the tree
  //returns the size of the event in bytes.

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
    fTree->GetCurrentFile()->cd();  //make sure we are in the right directory
    theRet = fTree->GetEntry(anEntry);
    if(theRet > 0) 
      fCurrentEntry = anEntry;

    return theRet;
  }
}

Int_t KDataReader::GetEntryWithGSEventNumber(Int_t anEntry)
{
  //will load (if possible) Entry cooresponding to GSEventNumber'anEntry' and 
  //if successful, will set the fCurrentEntry value to
  //anEntry. Otherwise returnvalue should be -1.

  if(fGSEventIndex==0){
    cout << "Building index..." << endl;
    fTree->BuildIndex("fGSEventNumber");
    fGSEventIndex=1;
  }

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

TObject* KDataReader::Get(const Char_t* namecycle) const
{
  //this simply calls TFile::Get and returns an object found in the file.
  if(fFile != 0){
    return fFile->Get(namecycle);
  }
  else return 0;
}

const char* KDataReader::GetEventClassName(void) const
{
  if(fTree != 0){
    TBranchElement *branch = (TBranchElement *)fTree->GetBranch(GetBranchName().c_str());
    if(branch != 0)
      return branch->GetClassName();

    else return "";
  }
  else return "";
}

const char* KDataReader::GetFileName(void) const
{
  if (fFile != 0) {
    return fFile->GetName();
  }
  else return "";

}

void KDataReader::ls(Option_t *anOpt) const
{
  if(fFile != 0){
    fFile->ls(anOpt);
  }

}

Bool_t KDataReader::cd(const char *path)
{
  if(fFile != 0){
    return fFile->cd(path);
  }
  else return false;
}

Int_t KDataReader::GetNextMuonEntry(Int_t anEntry) 
{
  //Loads the event with data from the next entry in the tree that
  //has at least one muon module subrecord in the event.
  //returns the size of the event in bytes.
  //returns zero when it reaches the end of the file, if no other
  //events are found.
  //if anEntry !=0, it will search for the next muon entry after anEntry
  //


  Int_t size=0;
  KEvent* e = GetEvent();
  
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
  //Loads the event with data from the previous entry in the tree that
  //has at least one muon module subrecord in the event.
  //returns the size of the event in bytes.
  //returns zero when it reaches the beginning of the file, if no other
  //events are found.
  //if anEntry !=0, it will search for the previous muon entry before anEntry
  //



  Int_t size=0;
  KEvent* e = GetEvent();
  
  if(e==0) return -1;
  
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
  //Loads the event with data from the next entry in the tree that
  //has at least one bolometer subrecord in the event.
  //returns the size of the event in bytes.
  //returns zero when it reaches the end of the file, if no other
  //events are found.
  //if anEntry !=0, it will search for the next bolometer entry after anEntry
  //
  

  Int_t size=0;
  KEvent* e = GetEvent();
  
  if(e==0) return -1;
  
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
  //Loads the event with data from the previous entry in the tree that
  //has at least one bolometer subrecord in the event.
  //returns the size of the event in bytes.
  //returns zero when it reaches the beginning of the file, if no other
  //events are found.
  //if anEntry !=0, it will search for the previous bolometer entry before anEntry
  //

  Int_t size=0;
  KEvent* e = GetEvent();
  
  if(e==0) return -1;
  
  if(anEntry<0)
    size=GetPreviousEntry();
  else
    size=GetEntry(anEntry-1);
  
  while(size>0 && e->GetNumBolos()==0){
    size=GetPreviousEntry();
  }
  
  return size;
}
