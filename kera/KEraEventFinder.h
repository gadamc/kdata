/*
 *  KEraRawEventReader.h
 *  KEraRawEventStructure
 *
 *  Created by Daniel Wegner on 9/1/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KERAEVENTFINDER_H__
#define __KERAEVENTFINDER_H__

#include "Rtypes.h"
#include "KEraRawEventReader.h"
#include "KFileTransfer.h"
#include "KSambaRecord.h"
#include "KBolometerRecord.h"
#include "KHLAEvent.h"
#include "EdwEvent.h"
#include "EdwPulse.h"
#include "TString.h"
#include <string>
#include <list>
#include <vector>

class KEventDisplay; 

using namespace std;

class KEraEventFinder {
  
public:
  //Constructors
  KEraEventFinder(void);
  KEraEventFinder(string aUser);
  KEraEventFinder(const KEraEventFinder& aFinder);
  KEraEventFinder(KSambaRecord* aSambaRecord,KBolometerRecord* aBoloRecord,
                  string aUser, string aServer, string aSourceDir, string aTargetDir);
  virtual ~KEraEventFinder();
  
  EdwEvent* TransferEvent(void);
  EdwEvent* TransferEventOld(void);
  EdwEvent* GetEvent(void);  //returns the event pointer from the KEraRawEventReader class
  
  void AddPathToSearch(const char* aPath){fDirNames.push_back(aPath);}
  void ClearSearchPaths(void){ fDirNames.clear(); }
  
  void DisplayEvent(void);
  void DisplayEvent(KBolometerRecord *aRec);
  
  //getters
  KSambaRecord* GetSamba(void) const { return fSambaRecord; }
  KBolometerRecord* GetBolo(void) const { return fBoloRecord; }
  Bool_t IsSearchLocally(void){return fSearchLocally;  }
  Bool_t IsForceRemoteSearch(void){return fForceRemoteSearch;  }
  KEventDisplay* GetDisplay(void) {return fDisplay;  }
  
  //setters
  void SetSamba(KSambaRecord* aRec) { fSambaRecord = aRec; }
  Bool_t SetBolo(KBolometerRecord *aRec);
  void SetSearchLocally(Bool_t aChoice = true);
  void SetForceRemoteSearch(Bool_t aChoice = true);
  void SetTargetPath(const char* aPath){fTargetPath = aPath;}
  void SetUser(const char* aUser);
  void SetApplyBasicPulseProcessing(Bool_t anOpt = true) { fApplyBasicPulseProcessing = anOpt; }
  void SetAlwaysKeepSearching(Bool_t anOpt = true) { fAlwaysKeepSearching = anOpt; }
  string GetNextFileName(const char* name = 0);
  
private:
  Bool_t fSearchLocally;
  Bool_t fForceRemoteSearch;
  Bool_t fEventHasBeenTransfered;
  
  KFileTransfer* fTrans; //Handles the data transfer
  KEraRawEventReader* fReader; //Reads the files
  KSambaRecord* fSambaRecord; //Contains the samba event number in the KDATA structure
  KBolometerRecord* fBoloRecord; //
  list<string> fDirNames;
  string fTargetPath;
  KEventDisplay *fDisplay;
  Bool_t fApplyBasicPulseProcessing;
  Bool_t fAlwaysKeepSearching;  //if this is set to true, they it won't ask you if you want to download the next file
  
  Bool_t GetEventFile(const char* aPath, const char* aFileName, Bool_t searchLocal);
  Bool_t OpenEventFile(const char* filePath, const char* fileName);
  Bool_t SearchTargetDirectory(string &aStartingFile);
  Bool_t SearchDirectoryList(string &aStartingFile);
  Bool_t DoesCurrentFileHaveEvent(void);


  void Initialize(void);
  
  
  ClassDef(KEraEventFinder,1);
};

#endif
