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
#include "TSystem.h"
#include "TUnixSystem.h"

ClassImp(KEraEventFinder);

KEraEventFinder::KEraEventFinder(void)
{
  fTrans = new KFileTransfer;
  fReader =new KEraRawEventReader;
  fDisplay = 0;
  fSambaRecord = 0;
  fBoloRecord = 0;
  Initialize();
  
  //no username was supplied in this constructor. We set up the object to search locally by default.
  fSearchLocally = true;
  fAlwaysKeepSearching = true; //if we're searching locally, don't bother to ask.
  
}

KEraEventFinder::KEraEventFinder(string aUser)
{
  fTrans = new KFileTransfer(aUser);
  fReader =new KEraRawEventReader();
  fDisplay = 0;
  fSambaRecord = 0;
  fBoloRecord = 0;
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
  fForceRemoteSearch = false;  //don't force remote search by default.
  AddPathToSearch(fTrans->GetSourcePath().c_str()); //the default path on ccali
  AddPathToSearch(fTargetPath.c_str()); //add the "target" directory.
  
  TSystem *fSystem = 0;
  try {
		fSystem = dynamic_cast<TUnixSystem*>(gSystem);
	}
	catch(bad_cast) {
		fSystem = new TUnixSystem();
	}
  
  //try to determine if we're on ccali or if we're remote
  TString hostName = fSystem->Getenv("HOST"); 
  if(hostName.Contains("ccali")){
    fSearchLocally = true;
    fAlwaysKeepSearching = true; //if we're searching locally, don't bother to ask.
  }
  else { 
    fSearchLocally = false; //assume that we're searching over the internets. 
    fAlwaysKeepSearching = false;
    fTargetPath = "/tmp/";  //dump all files to /tmp/USER and hope that this directory gets cleaned up.
    fTargetPath.append(fSystem->Getenv("USER"));
    fTargetPath.append("/");
    fSystem->mkdir(fTargetPath.c_str());
  }

  if(fSystem != gSystem)
		delete fSystem;
  
  fApplyPulseProcessing = true;
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
      subRunName.Form("%03d",++theIndex);
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
  if(fBoloRecord == 0 || fSambaRecord == 0 
     || fTrans == 0 || fReader == 0){
    cout <<"KEraEventFinder::TransferEvent. Not Ready to transfer Event" << endl;
    return 0;
  }
  
  string sambaName = GetNextFileName();
  
  if(!fSearchLocally && !fForceRemoteSearch) {
    if(SearchTargetDirectory(sambaName)){
      fEventHasBeenTransfered = true;
      return GetEvent();
    }
  }
  
  if(SearchDirectoryList(sambaName)){
     //the directory list could be local or remote directories.
    fEventHasBeenTransfered = true;
    return GetEvent();
  } 
  
  return 0;
  
}

Bool_t KEraEventFinder::SearchTargetDirectory(string &sambaName)
{
  //this SHOULD ONLY get called if fSearchLocally == false && ForceRemoteSearch == false
  
   if( fSearchLocally || fForceRemoteSearch )
     return false;
  
  //we do a quick check of the target directory. 
  //search the target directory for the files that should correspond to this event
  //keep searching for appropriate file names until a file is not found.
  Bool_t keepGoing = true;
  
  if(sambaName.size() == 0)
    sambaName = GetNextFileName();
  
  do{
#ifdef _K_DEBUG_ERAEVENTFINDER
    cout << "KEraEventFinder. Locally searching for event in " << fTargetPath  << sambaName << endl;
#endif
    keepGoing = GetEventFile(fTargetPath.c_str(), sambaName.c_str(), true);  //since we're searching the target directory, we are searching locally
    if(keepGoing){
      keepGoing = OpenEventFile(fTargetPath.c_str(), sambaName.c_str());

      if(keepGoing){

        if(DoesCurrentFileHaveEvent()){

          return true;
        }
          
      }
      
      sambaName = GetNextFileName(sambaName.c_str());

    }
      
      
  }while(keepGoing);
#ifdef _K_DEBUG_ERAEVENTFINDER  
  cout << "KEraEventFinder. ... not found." << endl;
#endif
  return false;
}

Bool_t KEraEventFinder::SearchDirectoryList(string &sambaName)
{
  //search through the list of directories for the file
  list<string>::iterator it = fDirNames.begin();
  if(it == fDirNames.end())
  {
    cout << "KEraEventFinder::SearchDirectoryList. No search directories have been specified." << endl;
    return 0;
  }
 
  if(sambaName.size() == 0)
    sambaName = GetNextFileName();
  
  string startingSambaName = sambaName;
  
  for(it = fDirNames.begin(); it!= fDirNames.end(); ++it){
    
    sambaName = startingSambaName;
    
    string filePath;
    if(fSearchLocally)
      filePath = it->c_str();
    else 
      filePath = fTargetPath;
    
    Bool_t keepGoing = true;
    
    do{
      
      //if we find the file, open it up, and find the file, then we return true;
      //otherwise we search for the next file within this directory on the list.
  
      if(GetEventFile(it->c_str(), sambaName.c_str(), fSearchLocally)){  
        if(!OpenEventFile(filePath.c_str(), sambaName.c_str())){  
          cout  << "KEraEventFinder::TransferEvent. Failed Opening the File! Not an ERA Event File?" << endl;
        }
        else if (DoesCurrentFileHaveEvent()){
          return true;
        }
        
        sambaName = GetNextFileName(sambaName.c_str());
        
        if(!fAlwaysKeepSearching) {
          cout << endl;
          cout << "Do you want to search for the event in the next file " << sambaName << " in this directory? " << endl;
          cout << "     You are currently searching ";
          if (fSearchLocally) cout << "locally" << endl; 
          else cout << "remotely " << endl;
          
          if(!fSearchLocally && fTrans != 0) 
            cout << "     on server: " << fTrans->GetUser() << "@" << fTrans->GetServer() << endl;
          cout << "     in directory " << it->c_str() << endl << endl;
          cout << endl;
          cout << "     You can automatically answer \'yes\' by calling KEraEventFinder::SetAlwaysKeepSearching(true)" << endl;
          cout << endl;
          cout << "     Answer \'next\' if you want to search in the next directory in your list."<< endl;
          cout << "     Answer \'quit\' if you want to stop searching altogether. " << endl;
          cout << endl;
          
          ++it;
          if(it != fDirNames.end()) {
            cout << "     If you answer 'next' the search will continue in the next directory on your list starting with the first file name." << endl;
            cout << "     The next directory in your list is: " << it->c_str() << endl;
          }
          else{
            cout << "     This is the last directory in your list. If you answer 'next' the search will stop." << endl;
          }
           --it;
          
          cout << endl;
          cout << "     Any other answer will be considered to mean \'yes, continue searching in this directory\': ";
         
          char theOpt[10];
          streamsize oldSize = cin.width(10);
          cin >> theOpt;
          string anOption = theOpt;
          if(anOption == "next")
            keepGoing = false;
          if(anOption == "quit")
            return false;
          
          cin.width(oldSize);
        }
      }
      else{
        //We weren't able to find the first file in the list in this directory. We
        //assume that this directory won't have any of the files.
        keepGoing = false;
        //continue;  //go to the next directory. 
      }
    } while (keepGoing);
  }
  
  return false;
}

Bool_t KEraEventFinder::DoesCurrentFileHaveEvent(void)
{
  if(fReader == 0 || fSambaRecord == 0){
    cout << "KEraEventFinder::DoesCurrentFileHaveEvent. KEraRawEventReader or KSambaRecord is NULL." << endl;
    return 0;
  }
  //check to see if the file opened by our KEraRawEventReader 
  //contains the event we're looking for.
  
  EdwEvent* e = fReader->GetEvent();
  fReader->GetEntry(fReader->GetEntries()-1);
  UInt_t kSambaEventNumber = fSambaRecord->GetSambaEventNumber();
  UInt_t edwEventNumber = e->Header()->Num();
#ifdef _K_DEBUG_ERAEVENTFINDER
  cout << "Searching for event number " << kSambaEventNumber << endl;
  cout << " The event number of the last entry in this file is " << edwEventNumber << endl;
#endif
  
  if( kSambaEventNumber > edwEventNumber ){
    return false;
  }
  
  else if(kSambaEventNumber  == edwEventNumber) //EdwEvent found
    return true;

  else {

    //cout << "Jumping back to event" << endl;
    UInt_t dist = edwEventNumber - kSambaEventNumber; //go back dist entries
#ifdef _K_DEBUG_ERAEVENTFINDER
    cout << " Jumping back a distance " << dist << endl;
#endif
    
    fReader->GetEntry(fReader->GetEntries()-1-dist);
    edwEventNumber = e->Header()->Num();
    
    if(kSambaEventNumber != edwEventNumber){  //if its STILL not the right event, then we search this file.
#ifdef _K_DEBUG_ERAEVENTFINDER
      cout << "The ERA Raw file doesn't seem to be continuous in Samba Event Numbers... searching " << endl;
#endif
      //perform a search!???
      for(Int_t i = 0; i < fReader->GetEntries(); i++){
        fReader->GetEntry(i);
        if(kSambaEventNumber == e->Header()->Num()){
#ifdef _K_DEBUG_ERAEVENTFINDER
          cout << "Event Found in entry " << i << endl;
#endif
          return true;
        }
      }
      return false;
    }
    else if (kSambaEventNumber == edwEventNumber)
      return true; //hurray!
  }
  
  return false;
}

EdwEvent* KEraEventFinder::TransferEventOld(void)
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
    
    if(GetEventFile(it->c_str(), sambaName.c_str(), fSearchLocally)){
      if(!OpenEventFile(theFilePath.c_str(), sambaName.c_str())){
        cout  << "KEraEventFinder::TransferEvent. Failed Opening the File! Not an ERA Event File?" << endl;
        return 0;
      }
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
#ifdef _K_DEBUG_ERAEVENTFINDER
      cout << "KEraEventFinder::TransferEvent. Entry not found in " << sambaName << endl;
      cout << "                                will try next file." << endl;
#endif
      sambaName = GetNextFileName(sambaName.c_str());
      
      if(fSearchLocally) theFilePath = it->c_str();
      else theFilePath = fTargetPath.c_str();
      
      if(GetEventFile(it->c_str(), sambaName.c_str(), fSearchLocally)) {
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
#ifdef _K_DEBUG_ERAEVENTFINDER
        cout << "The file doesn't seem to be continuous in Event Numbers... searching " << endl;
#endif
        //perform a search!???
        for(Int_t i = 0; i < fReader->GetEntries(); i++){
          fReader->GetEntry(i);
          if(kEventNumber == e->Header()->Num()){
#ifdef _K_DEBUG_ERAEVENTFINDER
            cout << "Event Found." <<endl;
#endif
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

Bool_t KEraEventFinder::GetEventFile(const char* filePath, const char* fileName, Bool_t searchLocal) 
{
  if(!searchLocal){
    
    fTrans->SetSourcePath(filePath);
    fTrans->SetTargetPath(fTargetPath);
    //download and open file with fSambaRecords->RunName
#ifdef _K_DEBUG_ERAEVENTFINDER
    cout << "Transfer " << fileName << endl;
#endif
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
#ifdef _K_DEBUG_ERAEVENTFINDER
  cout << "KEraEventFinger. Attempting to open event file " << file << endl;
#endif
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
    
    fDisplay->SetApplyPulseProcessing(fApplyPulseProcessing);
    fDisplay->SetEvent(GetEvent(), bolo);
    fDisplay->DisplayEvent();
  }
  catch(bad_cast) {
    cout << "Your Bolometer Record is not a HLABolometerRecord. I do not yet know how to display this kind of event! Please program me." << endl;
  }
  
}

void KEraEventFinder::DisplayPower(KBolometerRecord *aRec)
{
  if(SetBolo(aRec))
    DisplayPower();
}

void KEraEventFinder::DisplayPower(void)
{
  if(fBoloRecord == 0){
    cout << "KEraEventFinder::DisplayPower. No Event to Display. You must first SetBolo." << endl;
  }
  try {
    if(!fEventHasBeenTransfered)
      if(TransferEvent() == 0){
        cout << "KEraEventFinder::DisplayPower. Unable to Transfer Event." << endl;
        return;
      }
    
    KHLABolometerRecord *bolo = dynamic_cast<KHLABolometerRecord*>(fBoloRecord);
    
    if(fDisplay == 0)
      fDisplay = new KEventDisplay;
    
    fDisplay->SetApplyPulseProcessing(true);
    fDisplay->SetEvent(GetEvent(), bolo);
    fDisplay->DisplayPower();
  }
  catch(bad_cast) {
    cout << "Your Bolometer Record is not a HLABolometerRecord. I do not yet know how to display this kind of event! Please program me." << endl;
  }
  
}


void KEraEventFinder::SetUser(const Char_t* aUser)
{
  if(fTrans != 0)
    fTrans->SetUser(aUser);
}

void KEraEventFinder::SetSearchLocally(Bool_t anOpt)
{ 
  //Set this to only search in local directories (as specified by the fDirNames)
  //Also, this will SetAlwaysKeepSearch(true) so that it doesn't prompt the user
  //to continue searching for files. 
  fSearchLocally = anOpt; 
  if(anOpt) 
    SetAlwaysKeepSearching(true);
}

void KEraEventFinder::SetForceRemoteSearch(Bool_t aChoice)
{ 
  //Force the search for files on the remote server. This
  //also sets SetSearchLocally(false).
  
  fForceRemoteSearch = aChoice; 
  if(aChoice) 
    SetSearchLocally(false);
}
