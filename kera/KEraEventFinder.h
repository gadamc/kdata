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
#include <string>
#include "TString.h"

class KEraEventFinder {

    public:
        KEraEventFinder(KSambaRecord* aSambaRecord,KBolometerRecord* aBoloRecord, const TString& aUser,const TString& aServer,const TString& aSourceDir,const TString& aTargetDir);
        EdwEvent* GetEvent(void);
        ~KEraEventFinder();


    private:
        TString GetNextFileName(void);

        KFileTransfer* fTrans;
        KEraRawEventReader* fReader;
        KSambaRecord* fSambaRecord;
        KBolometerRecord* fBoloRecord;


        ClassDef(KEraEventFinder,1);
};

#endif
