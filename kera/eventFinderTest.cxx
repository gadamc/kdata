/*
 *  eventFinderTest.cxx
 *
 *  Created by Daniel Wegner on 9/2/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "Rtypes.h"
 #include "KEraEventFinder.h"
 #include "KDataReader.h"
 #include "KHLAEvent.h"
 #include "KHLASambaRecord.h"
 #include "KHLABolometerRecord.h"
 #include "TString.h"
 #include "TApplication.h"
 #include <iostream>

 using namespace std;

int main(int argc, char** argv) {
    TApplication* app = new TApplication("test",&argc,argv);


 	TString aUser("gadamc");
    TString aServer("ccali.in2p3.fr");
    TString aSourceDir("/sps/edelweis/EdwRootAna/ID Run12/NeutronF2/rootevts/");
    TString aTargetDir("/var/tmp/.");
    KDataReader* kReader = new KDataReader("/home/enzi/Desktop/BoloData/Kds_FID401FID402_test.root");
    KEvent* kEvent = kReader->GetEvent();
    kReader->GetEntry(0);
    KHLAEvent* kHLAEvent;

	try {
            kHLAEvent = dynamic_cast<KHLAEvent*>(kEvent);
		}
	catch(bad_cast) {
        cerr << "bad cast" << endl;
        return 1;
	}

	KSambaRecord* samba = dynamic_cast<KSambaRecord*>(kHLAEvent->GetSamba(0));
	KBolometerRecord* bolo = dynamic_cast<KBolometerRecord*>(kHLAEvent->GetBolo(0));
	// either KHLA*Record pointers are return by the getters or the KEraEventFinder class expect KHLA*Record pointers

	KEraEventFinder* finder = new KEraEventFinder(samba,bolo,aUser,aServer,aSourceDir,aTargetDir);
	cout << &kHLAEvent << endl;
	EdwEvent* edwEvent = finder->GetEvent();

    app->Run();
    return 0;

}
