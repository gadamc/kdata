//_____________________________________________
//
// KFileTransfer.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 8/26/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
//

#include "KFileTransfer.h"
#include "TUnixSystem.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

ClassImp(KFileTransfer);

KFileTransfer::KFileTransfer(void)
{
	SetDefaultValues();
  try {
		fSystem = dynamic_cast<TUnixSystem*>(gSystem);
	}
	catch(bad_cast) {
		this->fSystem = new TUnixSystem();
	}
}

KFileTransfer::KFileTransfer(string aUser): fUser(aUser)
{
	SetDefaultValues();
  try {
		fSystem = dynamic_cast<TUnixSystem*>(gSystem);
	}
	catch(bad_cast) {
		this->fSystem = new TUnixSystem();
	}
}


KFileTransfer::KFileTransfer(string aUser, string aServer, string aSourcePath,
                             string aTargetPath)
: fUser(aUser), fServer(aServer), fSourcePath(aSourcePath), fTargetPath(aTargetPath)
{
	//Constructor
	try {
		fSystem = dynamic_cast<TUnixSystem*>(gSystem);
	}
	catch(bad_cast) {
		this->fSystem = new TUnixSystem();
	}
}

KFileTransfer::KFileTransfer(const KFileTransfer& aTrans)
: fUser(aTrans.fUser), fServer(aTrans.fServer), fSourcePath(aTrans.fSourcePath), 
fTargetPath(aTrans.fTargetPath)
{
	//Copy constructor
	try {
		fSystem = dynamic_cast<TUnixSystem*>(gSystem);
	}
	catch(bad_cast) {
    this->fSystem = new TUnixSystem();
	}
}

KFileTransfer::~KFileTransfer()
{
	//Destructor
	if(fSystem != gSystem)
		delete fSystem;
}

void KFileTransfer::SetDefaultValues(void)
{
  //The default values are set to
  //fServer = "ccali.in2p3.fr";
	//fSourcePath = "/sps/edelweis/kdata/data/rootevts/";
	//fTargetPath = "/tmp/";
	//which assumes that you are looking to find an ERA rootevts file!.

  
	fServer = "ccali.in2p3.fr";
	fSourcePath = "/sps/edelweis/kdata/data/rootevts/";
	fTargetPath = "/tmp/";
  fExtraRsyncOptions = "";
	
}

void KFileTransfer::Transfer(string aFilename)
{
  // for(int k = 0; k<20; ++k)
	// cout << name[k] << endl;
  
  //We MUST ensure that fSourcePath ends with a "/"
  TString aSourcePath = fSourcePath;
  if(!aSourcePath.EndsWith("/"))
    aSourcePath += "/";
  fSourcePath = aSourcePath.Data();
  
  //cout << fSourcePath << endl;
	
  string commandline = "rsync -Lz " + fExtraRsyncOptions + fUser + 
  "@" + fServer + ":" + fSourcePath +  aFilename + " " + fTargetPath  + aFilename;
	
  cout << commandline.c_str() << endl;
	fSystem->Exec(commandline.c_str());

}


bool KFileTransfer::FileExists(string aFileName)
{
  //Checks if file "aFilename" exists.
#ifdef _K_DEBUG_FILETRANSFER
  cout << "Checking for filename: " << aFileName << endl;
#endif
  ifstream testfile(aFileName.c_str(),ifstream::binary | ifstream::in);
  
  if(testfile.is_open()){
#ifdef _K_DEBUG_FILETRANSFER
    cout << "was able to open the file..." << endl;
#endif
    return true;
  }
    
  else return false;
}

/*
 int GetFileModTime(string aFilename) {
 struct stat results;
 stat(aFilename,&results);
 return results.st_mtime;
 }
 */



