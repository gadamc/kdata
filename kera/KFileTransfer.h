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
        KFileTransfer(const TString& aUser,const TString& aServer,const TString& aSourcePath,const TString& aTargetPath);
        void Transfer(const TString& aFileName);
        virtual ~KFileTransfer(void);

    private:
        TString fUser;
        TString fServer;
        TString fSourcePath;
        TString fTargetPath;
        TSystem* fSystem;

    ClassDef(KFileTransfer,1);

 };

 #endif
