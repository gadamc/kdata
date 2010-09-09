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

KFileTransfer::KFileTransfer(const KFileTransfer& aTrans)
: fUser(aTrans.fUser), fServer(aTrans.fServer), fSourcePath(aTrans.fSourcePath), fTargetPath(aTrans.fTargetPath), fSubPath(aTrans.fSubPath)
{
    //Copy constructor
    try {
        fSystem = dynamic_cast<TUnixSystem*>(gSystem);
    }
    catch(bad_cast) {
        this->fSystem = new TUnixSystem();
    }
}

KFileTransfer::KFileTransfer(string aUser,string aServer,string aSourcePath,string aTargetPath, string aSubPath)
: fUser(aUser), fServer(aServer), fSourcePath(aSourcePath), fTargetPath(aTargetPath), fSubPath(aSubPath)
{
    //Constructor
    try {
        fSystem = dynamic_cast<TUnixSystem*>(gSystem);
    }
    catch(bad_cast) {
        this->fSystem = new TUnixSystem();
    }
}

void KFileTransfer::Transfer(string aFilename)
{
    const char* name = aFilename.c_str();
   // for(int k = 0; k<20; ++k)
   // cout << name[k] << endl;
    string commandline = "rsync -vz " + fUser + "@" + fServer + ":" + fSourcePath + fSubPath + aFilename + " " + fTargetPath;
    cout << commandline.c_str() << endl;
       fSystem->Exec(commandline.c_str());
}

KFileTransfer::~KFileTransfer()
{
    //Destructor
    if(fSystem!=gSystem)
        delete fSystem;
}


bool KFileTransfer::FileExists(string aFilename)
{
    //Checks if file "aFilename" exists
    fstream testfile;
    testfile.open(aFilename.c_str());
    //cout << "file exists" << endl;
    return(testfile.is_open());
}

/*
int GetFileModTime(string aFilename) {
        struct stat results;
    stat(aFilename,&results);
    return results.st_mtime;
}
*/



