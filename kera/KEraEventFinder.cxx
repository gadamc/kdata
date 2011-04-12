//_____________________________________________
//
// KEraRawEventReader.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 9/1/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The basic purpose of this class is to find the ERA EdwEvent record
// that corresponds to a particular KBolometerRecord.
// This class will search in local directories, which you can specify,
// and will rsync to a remote server (default is ccali.in2p3.fr, which 
// is defined in the KFileTransfer class), also searching in directory 
// locations you can specify (default is /sps/edelweis/kdata/data/rootevts/, 
// as specified by KFileTransfer). 
// The user can also specify a list of local and remote directories
// to search for events. Use the AddPathToSearch method. 
//
// Because the ERA outputs do not specifiy exactly which file an event
// can be found, this class searches the ERA raw events for the file. Soon
// this should be fixed and it will provide the exact file to find the event
//
// In addition, this class contains some rudimentary display capabilities
// via the DisplayEvent and DisplayPower methods. 
//
// This class's behavior initializes differently depending upon which 
// constructor you use. If you use the default constructor with no
// user name, it will assume that you are using this program on ccali
// and will just look in the directories that you tell it. If you provide
// a username, it will assume this is your ccali username and then proceed
// to search the ccali default location for an event if it doesn't find 
// the event in you local directories. 
// 
// A pointer to the EdwEvent object can be obtained via the TransferEvent method
// or the GetEvent method (which gives the pointer to most recently transferred event).
// So, if you call TransferEvent() and then call EdwEvent *e = GetEvent(), the same event will be
// returned as if you just called EdwEvent *e = TransferEvent().
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
  //constructor with no user defined. 
  //Using this constructor, it is assumed
  //that you are searching for ERA events on the local computer, such as ccali.
  //Use the other contructor if you want this object automatically set up
  //to search for files on ccali.
  
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
  //constructor with aUser. aUser should be your username to an account
  //on ccali where the raw data files are found.
  
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
  //init all data members
  
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
    cout << "KEraEventFinder::GetNextFileName. filename: " << fileName << endl;
  }
  else
    cout << "KEraEventFinder::GetNextFileName. You must first set the Samba Record (or the Bolo Record with a KHLABolometerRecord*)" << endl;
  return fileName;
}

EdwEvent* KEraEventFinder::TransferEvent(void)
{
  //This searches for the next event and then returns a pointer
  //when/if it finds the corresponding event.
  
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
  //this searches the target transfer directory for the era raw event file
  
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
  //search through the list of directories for the file. returns true
  //if the event is found.
  
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

Bool_t KEraEventFinder::GetEventFile(const char* filePath, const char* fileName, Bool_t searchLocal) 
{
  //gets a particular file from the local directory or from the remote directory
  //and returns true if it finds the file or successfully transfer it.
  
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
  //opens the fileName that is located in filePath. returns true if sucesseful
  
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
  //returns a pointer to the event found in the local KEraRawEventReader object,
  //fReader. If fReader is zero, then this returns zero.
  //you do not own the memory pointed to by this pointer.
  
  if(fReader != 0) 
    return fReader->GetEvent();

  else return 0;
}



Bool_t KEraEventFinder::SetBolo(KBolometerRecord* aRec)
{
  //Set the bolometer record that defines which raw EdwEvent 
  //the class will search for.
  
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
  //Displays the event in two TCanvas objects. First, it will search for 
  //the EdwEvent that corresponds to aRec and then if it finds it, 
  //will display the event.
  
  if(SetBolo(aRec))
    DisplayEvent();
}

void KEraEventFinder::DisplayEvent(void)
{
  //displays the most recently found event 
  
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
  //same as DisplayEvent(KBolometerRecord *aRec) except that it performs a FFT on the waveform
  //and shows the power spectrum on a log-log plot for each waveform in 
  //the event. 
  
  if(SetBolo(aRec))
    DisplayPower();
}

void KEraEventFinder::DisplayPower(void)
{
  //same as DisplayEvent(void) except that it performs a FFT on the waveform
  //and shows the power spectrum on a log-log plot for each waveform in 
  //the most recently found event. 
  
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
  //sets the username
  
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
