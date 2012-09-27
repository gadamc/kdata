/*
 *  KFileTransfer.h
 *
 *  Created by Daniel Wegner on 8/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KFILETRANSFER_H__
#define __KFILETRANSFER_H__

#include "Rtypes.h"
#include "TSystem.h"
#include <string>
#include "TString.h"


using namespace std;

class KFileTransfer {
	
public:
	//Constructors
	KFileTransfer(void);
	KFileTransfer(string aUser);
	KFileTransfer(const KFileTransfer& aTrans);
	KFileTransfer(string aUser, string aServer, string aSourcePath, string aTargetPath);
	void Transfer(string aFileName);
	virtual ~KFileTransfer(void);
	
	//getters
	string GetUser() const { return fUser; }
	string GetServer() const { return fServer; }
	string GetSourcePath() const { return fSourcePath; }
	string GetTargetPath() const { return fTargetPath; }
	string GetExtraRsyncOptions() const { return fExtraRsyncOptions; }
  
	//setters
	void SetUser(string aUser) { fUser = aUser; }
	void SetServer(string aServer) { fServer = aServer; }
	void SetSourcePath(string aSourcePath) { fSourcePath = aSourcePath; }
	void SetTargetPath(string aTargetPath) { fTargetPath = aTargetPath; }
  void SetExtraRsyncOptions(string anOption) { fExtraRsyncOptions = anOption; }
	void SetDefaultValues(void);

	bool FileExists(string aFilename);
	
private:
	string fUser; //User name
	string fServer; //Server name
	string fSourcePath; //Source path
	string fTargetPath; //Target path
	TSystem* fSystem; //Local TSystem object executing rsync commands
	
  string fExtraRsyncOptions; 
  
	ClassDef(KFileTransfer,1);
};

#endif