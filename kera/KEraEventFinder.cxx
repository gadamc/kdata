//_____________________________________________
//
// KEraRawEventReader.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 9/1/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class returns a pointer to an EdwEvent object corresponding to a KBoloMeterRecord's event number
//


#include "KEraEventFinder.h"
#include <typeinfo>
#include <iostream>
#include <fstream>
#include "KEventDisplay.h"
#include "KHLABolometerRecord.h"

ClassImp(KEraEventFinder);

KEraEventFinder::KEraEventFinder(void)
{
  fTrans = new KFileTransfer;
  fReader =new KEraRawEventReader;
  fDisplay = 0;
  Initialize();
  fDirNames.clear();
  AddPathToSearch(fTrans->GetSourcePath().c_str());  //if you're running "locally", you're probably on ccali
  AddPathToSearch(fTargetPath.c_str());  
  fSearchLocally = true;
}

KEraEventFinder::KEraEventFinder(string aUser)
{
  fTrans = new KFileTransfer(aUser);
  fReader =new KEraRawEventReader();
  fDisplay = 0;
  Initialize();
}

KEraEventFinder::KEraEventFinder(const KEraEventFinder& aFinder)
: fSambaRecord(aFinder.fSambaRecord), fBoloRecord(aFinder.fBoloRecord)
{
  //Copy constructor
  fTrans = new KFileTransfer(*aFinder.fTrans);
  fReader = new KEraRawEventReader(*aFinder.fReader);
  fDisplay = 0;
  Initialize();
  
}

KEraEventFinder::KEraEventFinder(KSambaRecord* aSambaRecord, KBolometerRecord* aBoloRecord,
                                 string aUser, string aServer, string aSourceDir, string aTargetDir)
: fSambaRecord(aSambaRecord), fBoloRecord(aBoloRecord)
{
  //constructor to fill all attributes
  
  fTrans = new KFileTransfer(aUser,aServer,aSourceDir,aTargetDir);
  fReader =new KEraRawEventReader();
  fDisplay = 0;
  Initialize();
}

KEraEventFinder::~KEraEventFinder()
{
  //Default destructor
  
  if(fTrans != 0) delete fTrans;
  if(fReader != 0) delete fReader;
  if(fDisplay != 0) delete fDisplay;
}

void KEraEventFinder::Initialize(void)
{
  fDirNames.clear();
  AddPathToSearch(fTrans->GetSourcePath().c_str()); //the default path on ccali
  fSearchLocally = false; //assume that we're searching over the internets. 
  fForceRemoteSearch = false;  //don't force remote search by default.
  fTargetPath = "/tmp/";  //dump all files to /tmp/ and hope that this directory gets cleaned up.
}

string KEraEventFinder::GetNextFileName(const char* name)
{
  //returns the next root filename corresponding to fSambaRecords's event number
  //If you pass in a null pointer (or zero), the method will return the
  //first filename.
  
  TString subRunName = "";
  string fileName = "";
  Int_t theIndex = -1;
  
  if(name == 0) {
    subRunName = "000";
  }
  else{
    TString theName = name;
    if(theName.EndsWith(".root")){
      
      sscanf(name, "%*[a-z-A-Z-0-9]_%03d.root", &theIndex);
      subRunName.Form("%03d",theIndex);
    } 
    
    else {
      cout << "KEraEventFinder::GetNextFileName. file does not end in .root" << endl;
    }		
  }
  if(fSambaRecord != 0){
    string runname = fSambaRecord->GetRunName().c_str();
    fileName =  runname + "_" + subRunName.Data() + ".root";
  }
  else
    cout << "KEraEventFinder::GetNextFileName. You must first set the Samba Record (or the Bolo Record with a KHLABolometerRecord*)" << endl;
  return fileName;
}

EdwEvent* KEraEventFinder::TransferEvent(void)
{
  //Returns a pointer to an EdwEvent object with fSambaRecord's event number
  
  if(fBoloRecord == 0 || fSambaRecord == 0 
     || fTrans == 0 || fReader == 0){
    cout <<"KEraEventFinder::TransferEvent. Not Ready to transfer Event" << endl;
    return 0;
  }
  
  //Get fSambaRecord attributes
  UInt_t kEventNumber = fSambaRecord->GetSambaEventNumber();
  //cout << "kEventNumber: " << kEventNumber << endl;  
  
  //char subPath[40];
  list<string>::iterator it = fDirNames.begin();
  if(it == fDirNames.end())
  {
    cout << "KEraEventFinder::TransferEvent. No search directories have been specified." << endl;
    return 0;
  }
  
  //if we are not forced to do a remote search and we 
  //aren't already set to search locally, we insert a quick
  //search to the local target directory to see if the file
  //already exists. 
  list<string> tempDirNames = fDirNames;
  Bool_t bSaveSearchLocalOption = fSearchLocally;
  if(!fSearchLocally && !fForceRemoteSearch) {
    tempDirNames.push_front(fTargetPath.c_str());
    fSearchLocally = true;
  } 
  it = tempDirNames.begin();
  
  for(it = tempDirNames.begin(); it!= tempDirNames.end(); ++it) {
    //cout << "Searching for file in " << it->c_str() << endl;
    string sambaName = GetNextFileName();
    
    string theFilePath;
    if(fSearchLocally) theFilePath = it->c_str();
    else theFilePath = fTargetPath.c_str();
    
    if(GetEventFile(it->c_str(), sambaName.c_str())){
      if(!OpenEventFile(theFilePath.c_str(), sambaName.c_str())){
        cout  << "KEraEventFinder::TransferEvent. Failed Opening the File! Not an ERA Event File?" << endl;
        return 0;
      }
      fSearchLocally = bSaveSearchLocalOption;
    }
    else {
      fSearchLocally = bSaveSearchLocalOption; //reset the search local option to its original value.
      continue; //search all of the directories in the list.
    }
    
    //cout << "Found File" << endl;
    
    //read last EdwEvent entry in first file
    Int_t numEntries = fReader->GetEntries();
    //cout << "with " << numEntries << " entries" << endl;
    EdwEvent* e = fReader->GetEvent();
    fReader->GetEntry(numEntries-1);
    UInt_t edwEventNumber = e->Header()->Num();
    
    while(kEventNumber > edwEventNumber) {
      
      //open next file
      cout << "KEraEventFinder::TransferEvent. Entry not found in " << sambaName << endl;
      cout << "                                will try next file." << endl;
      sambaName = GetNextFileName(sambaName.c_str());
      
      if(GetEventFile(it->c_str(), sambaName.c_str())) {
        if(!OpenEventFile(theFilePath.c_str(), sambaName.c_str())){
          cout  << "KEraEventFinder::TransferEvent. Failed Opening the File! Not an ERA Event File?" << endl;
          return 0;
        }
      }
      else 
        break;
      
      //read last EdwEvent entry
      numEntries = fReader->GetEntries();
      //cout << "with " << numEntries << " entries" << endl;
      fReader->GetEntry(numEntries-1);
      e = fReader->GetEvent();
      edwEventNumber = e->Header()->Num();
    }
    
    if(kEventNumber == edwEventNumber) //EdwEvent found
      return e;
    else {
      //cout << "Jumping back to event" << endl;
      Int_t dist = edwEventNumber-kEventNumber; //go back dist entries
      fReader->GetEntry(numEntries-1-dist);
      edwEventNumber = e->Header()->Num();
      
      if(kEventNumber != edwEventNumber){
        cout << "The file doesn't seem to be continuous in Event Numbers... searching " << endl;
        //perform a search!???
        for(Int_t i = 0; i < fReader->GetEntries(); i++){
          fReader->GetEntry(i);
          if(kEventNumber == e->Header()->Num()){
            cout << "Event Found." <<endl;
            break;
          }
        }
      }
      
      fEventHasBeenTransfered = true;
      return e;
    }
  }
  
  cout << "KEraEventFinder::TransferEvent. Entry not found" << endl;
  return 0;
}

Bool_t KEraEventFinder::GetEventFile(const char* filePath, const char* fileName) 
{
  if(!fSearchLocally){
    
    fTrans->SetSourcePath(filePath);
    fTrans->SetTargetPath(fTargetPath);
    //download and open file with fSambaRecords->RunName
    cout << "Transfer " << fileName << endl;
    fTrans->Transfer(fileName);
    
    //cout << "Opening " << fTrans->GetTargetPath() + sambaName << endl;
    return fTrans->FileExists(fTrans->GetTargetPath() + fileName);
  }
  else {
    
    string fullPath = filePath;
    fullPath += fileName;
    ifstream checkFile(fullPath.c_str(), ifstream::binary | ifstream::in);
    if(checkFile.is_open()){
      checkFile.close();
      return true;
    }
    else return false;
    
  }
  
}

Bool_t KEraEventFinder::OpenEventFile(const char* filePath, const char* fileName)
{
  string file = filePath;
  file += fileName;
  if(fReader != 0) 
    return fReader->Open(file);
  else return false;
}

EdwEvent* KEraEventFinder::GetEvent(void)
{
  if(fReader != 0) 
    return fReader->GetEvent();
  
  else return 0;
}



Bool_t KEraEventFinder::SetBolo(KBolometerRecord* aRec) 
{ 
  if(aRec == 0) {
    cout << "KEraEventFinder::SetBolo. BolometerRecord is NULL." << endl;
    return false; 
  }
  
  fBoloRecord = aRec;
  fEventHasBeenTransfered  = false;
  //check to see if this pointer is really a KHLABolometerRecord
  //If so, then check to see if it points to a valid SambaRecord
  try {
    KHLABolometerRecord *bolo = dynamic_cast<KHLABolometerRecord*>(fBoloRecord);
    if(bolo->GetSambaRecord() != 0)
      SetSamba(bolo->GetSambaRecord());
    return true;
  }
  catch(bad_cast) {
    //do nothing.
    return false;
  }
  
}

void KEraEventFinder::DisplayEvent(KBolometerRecord *aRec)
{
  if(SetBolo(aRec))
    DisplayEvent();
}

void KEraEventFinder::DisplayEvent(void)
{
  if(fBoloRecord == 0){
    cout << "KEraEventFinder::DisplayEvent. No Event to Display. You must first SetBolo." << endl;
  }
  try {
    if(!fEventHasBeenTransfered)
      if(TransferEvent() == 0){
        cout << "KEraEventFinder::DisplayEvent. Unable to Transfer Event." << endl;
        return;
      }
    
    KHLABolometerRecord *bolo = dynamic_cast<KHLABolometerRecord*>(fBoloRecord);
    
    if(fDisplay == 0)
      fDisplay = new KEventDisplay;
    
    fDisplay->SetEvent(GetEvent(), bolo);
    fDisplay->DisplayEvent();
  }
  catch(bad_cast) {
    cout << "Your Bolometer Record is not a HLABolometerRecord. I do not yet know how to display this kind of event! Please program me." << endl;
  }
  
}

