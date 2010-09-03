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

using namespace std;

ClassImp(KFileTransfer);

KFileTransfer::KFileTransfer(const TString& aUser,const TString& aServer,const TString& aSourcePath,const TString& aTargetPath)
: fUser(aUser), fServer(aServer), fSourcePath(aSourcePath), fTargetPath(aTargetPath) {

    fSystem = new TUnixSystem();
    //fSystem = dynamic_cast<TUnixSystem*>(gSystem);
}

void KFileTransfer::Transfer(const TString& aFilename) {
    TString commandline = "rsync -vz " + fUser + '@' + fServer + ':' + fSourcePath + aFilename + ' ' + fTargetPath;
    cout << commandline << endl;
    fSystem->Exec(commandline);
}

KFileTransfer::~KFileTransfer() {
    delete fSystem;
}



