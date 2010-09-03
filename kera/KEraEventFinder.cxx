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
#include <string>

ClassImp(KEraEventFinder);


KEraEventFinder::KEraEventFinder(KSambaRecord* aSambaRecord, KBolometerRecord* aBoloRecord,const TString& aUser,const TString& aServer,const TString& aSourceDir,const TString& aTargetDir)
: fSambaRecord(aSambaRecord), fBoloRecord(aBoloRecord) {

    fTrans = new KFileTransfer(aUser,aServer,aSourceDir,aTargetDir);
    fReader =new KEraRawEventReader();
}

TString KEraEventFinder::GetNextFileName() {


    //first file "*_000.root"
    static Int_t count = 0;
    static Char_t subRunName[4] = "000";

    Int_t n = sprintf(subRunName,"%i",count);

    subRunName[n] = '0';

    static TString sambaname = fSambaRecord->GetRunName() + '_' + subRunName + ".root";

    ++count;

    return sambaname;
}

EdwEvent* KEraEventFinder::GetEvent(void) {


    //Get fSambaRecord attributes

    UInt_t kEventNumber = fSambaRecord->GetSambaEventNumber();


    TString sambaName = GetNextFileName();



    //download and open file with fSambaRecords->RunName
    cout << "Transfer " << sambaName << endl;
    fTrans->Transfer(sambaName);
    cout << "Open " << sambaName << endl;
    fReader->Open(sambaName);


    //read last EdwEvent entry in first file
    Int_t numEntries = fReader->GetEntries();
    EdwEvent* e = fReader->GetEvent();
    fReader->GetEntry(numEntries-1);
    UInt_t edwEventNumber = e->Header()->Num();



    while(kEventNumber > edwEventNumber) {

        //open next file
        cout << "Entry not found in " << sambaName << endl;
        sambaName = GetNextFileName();
        fReader->Close();
        cout << "Opening " << sambaName << endl;
        fTrans->Transfer(sambaName);
        fReader->Open(sambaName);

        //read last EdwEvent entry
        numEntries = fReader->GetEntries();
        fReader->GetEntry(numEntries-1);
        edwEventNumber = e->Header()->Num();
    }

    if(kEventNumber == edwEventNumber) //EdwEvent found
    return e;
        else
        {
            Int_t dist = edwEventNumber-kEventNumber; //go back dist entries
            fReader->GetEntry(numEntries-1-dist);
            return e;
        }

}

KEraEventFinder::~KEraEventFinder() {
    delete fTrans;
    delete fReader;

}


