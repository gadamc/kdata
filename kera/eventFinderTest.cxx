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
#include <list>


using namespace std;



int main(int argc, char** argv) {
  /*
  
  string kEventSourceFile("/home/enzi/Desktop/BoloData/Kds_FID401FID402_test.root");
  KDataReader* kReader = new KDataReader(kEventSourceFile.c_str());
  KEvent* kEvent = kReader->GetEvent();
  Int_t kEntries = kReader->GetEntries();
  cout << "Open " << kEventSourceFile << endl;
  cout << "with " << kEntries << " entries" << endl;
  kReader->GetEntry(10000);
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
  
  
  string aUser("gadamc");
  string aServer("ccali.in2p3.fr");
  
  
  
  string aSourceDir("/sps/edelweis/EdwRootAna/ID_Run12/");
  string aTargetDir("/var/tmp/");
  
  KEraEventFinder* finder = new KEraEventFinder(samba,bolo,aUser,aServer,aSourceDir,aTargetDir);
  cout << &kHLAEvent << endl;
  EdwEvent* edwEvent = finder->TransferEvent();
  if(edwEvent != 0) {
    cout << "edwEvent found" << endl;
    
    edwEvent->Header()->Inspect();
  }
  
  
  */
  TApplication* app = new TApplication("test",&argc,argv);

  
  KDataReader f("/Users/adam/analysis/edelweiss/data/boloEds/boloEdsFull/Merge/Kds_Run12_3.0dev_skim.root");
  f.GetNextBoloEntry();
  KHLAEvent *e = (KHLAEvent *)f.GetEvent();
  KEraEventFinder find("gadamc");
  find.DisplayEvent(e->GetBolo(0));
  
  app->Run();
  app->ReturnPressed("");
  return 0;
}
