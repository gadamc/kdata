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
#include "TString.h"
#include <string>
using namespace std;

class KEraEventFinder {

    public:
        //Constructors
        KEraEventFinder(const KEraEventFinder& aFinder);
        KEraEventFinder(KSambaRecord* aSambaRecord,KBolometerRecord* aBoloRecord,string aUser,string aServer,string aSourceDir,string aTargetDir,string aSubDir = "");
        EdwEvent* GetEvent(void);
        virtual ~KEraEventFinder();

        //getters
        KSambaRecord* GetSamba(void) const { return fSambaRecord; }
        KBolometerRecord* GetBolo(void) const { return fBoloRecord; }

        //setters
        void SetSamba(KSambaRecord* aRec) { fSambaRecord = aRec; }
        void SetBolo(KBolometerRecord* aRec) { fBoloRecord = aRec; }

    private:
        string GetNextFileName(bool reset = false);

        KFileTransfer* fTrans; //Handles the data transfer
        KEraRawEventReader* fReader; //Reads the files
        KSambaRecord* fSambaRecord; //Contains the samba event number in the KDATA structure
        KBolometerRecord* fBoloRecord; //




        ClassDef(KEraEventFinder,1);
};

#endif
