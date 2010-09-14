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

#include "KHLABolometerRecord.h"

ClassImp(KEraEventFinder);

KEraEventFinder::KEraEventFinder(string aUser)
{
	fTrans = new KFileTransfer(aUser);
	fReader =new KEraRawEventReader();
  Initialize();
}

KEraEventFinder::KEraEventFinder(const KEraEventFinder& aFinder)
: fSambaRecord(aFinder.fSambaRecord), fBoloRecord(aFinder.fBoloRecord)
{
  //Copy constructor
  fTrans = new KFileTransfer(*aFinder.fTrans);
  fReader = new KEraRawEventReader(*aFinder.fReader);
  Initialize();
  
}

KEraEventFinder::KEraEventFinder(KSambaRecord* aSambaRecord, KBolometerRecord* aBoloRecord,
																 string aUser, string aServer, string aSourceDir, string aTargetDir)
: fSambaRecord(aSambaRecord), fBoloRecord(aBoloRecord)
{
  //constructor to fill all attributes
  
  fTrans = new KFileTransfer(aUser,aServer,aSourceDir,aTargetDir);
  fReader =new KEraRawEventReader();
  Initialize();
}

KEraEventFinder::~KEraEventFinder()
{
  //Default destructor
  
  if(fTrans != 0) delete fTrans;
  if(fReader != 0) delete fReader;
}

void KEraEventFinder::Initialize(void)
{
  fDirNames.clear();
  AddPathToSearch("/sps/edelweis/kdata/data/rootevts/"); //the default path
  fSearchLocally = false; //assume that we're searching over the internets. 
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
  cout << "kEventNumber: " << kEventNumber << endl;  
  
  //char subPath[40];
  list<string>::iterator it = fDirNames.begin();
  if(it == fDirNames.end())
  {
    cout << "KEraEventFinder::TransferEvent. No search directories have been specified." << endl;
    return 0;
  }
  
  for(it = fDirNames.begin(); it!= fDirNames.end(); ++it) {
    
    string sambaName = GetNextFileName();

    if(GetEventFile(it->c_str(), sambaName.c_str())){
      if(!OpenEventFile(fTargetPath.c_str(), sambaName.c_str())){
        cout  << "KEraEventFinder::TransferEvent. Failed Opening the File! Not an ERA Event File?" << endl;
        return 0;
      }
    }
    else //if GetEventFile fails, then use the list of directories to search for other places.
      continue;
    
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
        if(!OpenEventFile(fTargetPath.c_str(), sambaName.c_str())){
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
    else
    {
      Int_t dist = edwEventNumber-kEventNumber; //go back dist entries
      fReader->GetEntry(numEntries-1-dist);
      edwEventNumber = e->Header()->Num();
      if(kEventNumber != edwEventNumber)
      {
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
      return e;
    }
  }
  
  cout << "KEraEventFinder::TransferEvent. Entry not found";
  return 0;
}

Bool_t  KEraEventFinder::GetEventFile(const char* filePath, const char* fileName) 
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



void KEraEventFinder::SetBolo(KBolometerRecord* aRec) 
{ 
  fBoloRecord = aRec;
  
  //check to see if this pointer is really a KHLABolometerRecord
  //If so, then check to see if it points to a valid SambaRecord
  try {
    KHLABolometerRecord *bolo = dynamic_cast<KHLABolometerRecord*>(fBoloRecord);
    if(bolo->GetSambaRecord() != 0)
      SetSamba(bolo->GetSambaRecord());
  }
  catch(bad_cast) {
    //do nothing.
  }
  
}

