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
#include <list>

ClassImp(KEraEventFinder);

KEraEventFinder::KEraEventFinder(const KEraEventFinder& aFinder)
: fSambaRecord(aFinder.fSambaRecord), fBoloRecord(aFinder.fBoloRecord)
{
    //Copy constructor
    fTrans = new KFileTransfer(*aFinder.fTrans);
    fReader = new KEraRawEventReader(*aFinder.fReader);

}

KEraEventFinder::KEraEventFinder(KSambaRecord* aSambaRecord, KBolometerRecord* aBoloRecord,string aUser,string aServer,string aSourceDir,string aTargetDir,string aSubDir)
: fSambaRecord(aSambaRecord), fBoloRecord(aBoloRecord)
{
    //constructor to fill all attributes

    list<string> dirNames;
    fTrans = new KFileTransfer(aUser,aServer,aSourceDir,aTargetDir,aSubDir);
    fReader =new KEraRawEventReader();
}

string KEraEventFinder::GetNextFileName(bool reset)
{
    //returns the next root filename corresponding to fSambaRecords's event number

    //first file "*_000.root"
    static Int_t count = 0;
    static string sambaname;
    static Char_t subRunName[4] = "000";
    if(reset)
        count = 0;

    Int_t n = sprintf(subRunName,"%i",count);
    subRunName[n] = '0';
    swap(subRunName[0],subRunName[2]);
    string runname = fSambaRecord->GetRunName().c_str();
    sambaname =  runname + "_" + subRunName + ".root";

    ++count;

    return sambaname;
}

EdwEvent* KEraEventFinder::GetEvent(void)
{
    //Returns a pointer to an EdwEvent object with fSambaRecord's event number

    //search directories
    list<string> dirNames;
    dirNames.push_back("NeutronS");
    dirNames.push_back("BckgdS");
    dirNames.push_back("GammaS");

    //Get fSambaRecord attributes
    UInt_t kEventNumber = fSambaRecord->GetSambaEventNumber();
    Int_t daqNumber = fSambaRecord->GetSambaDAQNumber();
    cout << "kEventNumber: " << kEventNumber << endl;
    cout << "DAQ Number: " << daqNumber << endl;

    char subPath[40];
    for(list<string>::iterator it = dirNames.begin(); it!= dirNames.end(); ++it) {

        sprintf(subPath,"%s%i/rootevts/",it->c_str(),daqNumber);
        fTrans->SetSubPath(subPath);
        string sambaName = GetNextFileName(true);

        //download and open file with fSambaRecords->RunName
        cout << "Transfer " << sambaName << endl;
        fTrans->Transfer(sambaName);
        string target = fTrans->GetTargetPath();
        cout << "Open " << target + sambaName << endl;
        if(fTrans->FileExists(target + sambaName))
            fReader->Open(target + sambaName);
            else
            continue;

        //read last EdwEvent entry in first file
        Int_t numEntries = fReader->GetEntries();
        cout << "with " << numEntries << " entries" << endl;
        EdwEvent* e = fReader->GetEvent();
        fReader->GetEntry(numEntries-1);
        UInt_t edwEventNumber = e->Header()->Num();

        while(kEventNumber > edwEventNumber) {

            //open next file
            cout << "Entry not found in " << sambaName << endl;
            sambaName = GetNextFileName(false);
            cout << "Open " << sambaName << endl;
            fTrans->Transfer(sambaName);
            if(fTrans->FileExists(target + sambaName)) {
                //fReader->Close();
                fReader->Open(target + sambaName);
            }
                else
                    break;

            //read last EdwEvent entry
            numEntries = fReader->GetEntries();
            cout << "with " << numEntries << " entries" << endl;
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
                return e;
            }
        }

        cerr << "Entry not found in all directories";
        return 0;
}

KEraEventFinder::~KEraEventFinder()
{
    //Default destructor

    delete fTrans;
    delete fReader;
}


