/*
 *  fillEvents.cc
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/6/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "TFile.h"
#include "KHLAEvent.h"
#include "KTestEventClass.h"
#include "KHLAMuonModuleSubRecord.h"
#include "KHLASingleBoloSubRecord.h"
#include "TTree.h"
#include <iostream>
#include <fstream>
#include "TRandom.h"
#include "KDataWriter.h"

using namespace std;


int fillEventExampleFunction(const char* name, const char* log) 
{
	/*
	if(argc < 4) {
    cout << "Incorrect Number of arguments." << endl;
    exit(0);
  }
  */
	
	TString rootFileName = name;  //can specify exactly which argv
	TString logFileName = log;
	
	/*
	Int_t fNumCorrectSwitches = 0;
	//or can scan for switches
  Int_t i = 1;
  while ( i < argc ) {
    if( strcmp(argv[i],"-h") == 0 ) {
      help();
      exit(1);
    }
    else if( strcmp(argv[i],"-f") == 0 ) {
      rootFileName = argv[++i];
			fNumCorrectSwitches++;
    }
    else if( strcmp(argv[i],"-tn") == 0 ) {
      treeName = argv[++i];
			fNumCorrectSwitches++;
    }
    else if( strcmp(argv[i],"-tt") == 0 ) {
      treeTitle = argv[++i];
			fNumCorrectSwitches++;
    }
    else if( strcmp(argv[i],"-l") == 0 ) {
      logFileName = argv[++i];
			fNumCorrectSwitches++;
    }
    else {
      cout << "ERROR when processing command line arguments. " << argv[i] << endl;
      cout << " this option does not exist." << endl;
			return -1;
    }
    i++;
  }
	
	if(fNumCorrectSwitches != 4){
		cout << "didn't find all of the switches " << endl;
		return -1;
	}
	*/
	
	ofstream logFile(logFileName.Data(), ios::out);
	
	cout << "Welcome to Fill Event" << endl;
	logFile << "Welcome to Fill Event" << endl;
	
	/*
	TFile f(rootFileName.Data(),"recreate");
	cout << "Recreated Root File " << rootFileName.Data() << endl;
	logFile << "Recreated Root File " << rootFileName.Data() << endl;
	
	KHLAEvent *mEv = new KHLAEvent();
	cout << "Created KHLAEvent" << endl;
	logFile << "Created KHLAEvent" << endl;
	
	TTree *t = new TTree(treeName.Data(),treeTitle.Data());
	cout << "Created TTree" << endl;
	logFile << "Created TTree" << endl;
	
	t->Branch("KHLAEventBranch","KHLAEvent",&mEv,32000,99); 
	t->BranchRef(); //creates an extra Branch table filled with Branches that have TRefs or TRefArrays
	*/
	
	KDataWriter fOutFile(rootFileName.Data());
	cout << "Recreated Root File " << rootFileName.Data() << endl;
	logFile << "Recreated Root File " << rootFileName.Data() << endl;
	
	KHLAEvent *mEv = dynamic_cast<KHLAEvent*>(fOutFile.GetEvent());
	if(mEv == 0){
		cout << "Unable to Create Event object" << endl;
		return -1;
	}
	
	//standard event information
	mEv->SetRunNumber(12);
	//mEv->SetRunStartTime(
	
	
	gRandom->SetSeed(0);
	Int_t numEvents = 70000 + (Int_t)20000*gRandom->Rndm();
	cout <<"Will create " << numEvents << " events" << endl;
	logFile <<"Will create " << numEvents << " events" << endl;
	Bool_t bIsBoloEvent, bIsMuonEvent; 
	for(Int_t i = 0; i < numEvents; i++){
		
		mEv->Clear("C");  //make sure its cleared
		
		bIsBoloEvent = bIsMuonEvent = false;
		if((i % 2000) == 0) {
			cout << i << " events filled" << endl;
			logFile << i << " events filled" << endl;
		}
		mEv->SetRunNumber(i);
		//mEv->SetDataCleaningWord(4);
		mEv->SetEventTime(3.023432*i);
		mEv->SetGSEventNumber(3*i+1994);
		//5% chance of a Muon event... which is probably way too huge.
		if(gRandom->Rndm() < 0.05){
			Int_t nMod = 1 + (Int_t)(3*gRandom->Rndm());
			mEv->GetMuonVetoSystemRecord()->SetIsMuonSystemOn(true);
			mEv->GetMuonVetoSystemRecord()->SetRunNumber(63);
			mEv->GetMuonVetoSystemRecord()->SetRunStartTime(1257638400);  //Nov 8, 2009 at 00:00
			mEv->GetMuonVetoSystemRecord()->SetRunEndTime(1270512000);  //April 6, 2010 at 00:00
			
			for(Int_t i = 0; i < nMod; i++) {
				bIsMuonEvent = true;
				KHLAMuonModuleSubRecord* mMuonModule = mEv->AddMuonModule();
				mMuonModule->SetModuleNumber(40*gRandom->Rndm());
				mMuonModule->SetAdc(0,(Int_t)(2048*gRandom->Rndm()));
				mMuonModule->SetAdc(1,(Int_t)(2048*gRandom->Rndm()));
				//mMuonModule->SetRelativeEventTime(0,200.0*gRandom->Rndm());
				//mMuonModule->SetRelativeEventTime(1,mMuonModule->GetRelativeEventTime(0) + 200.0*gRandom->Rndm());
				mMuonModule->SetEnergy(0,(Int_t)(2048*gRandom->Rndm()/1.0e6)); //divide by 1 MeV...?
				mMuonModule->SetEnergy(1,(Int_t)(2048*gRandom->Rndm()/1.0e6));
			}
			
		}
		else {
			Int_t nBolo = 1 + (gRandom->Poisson(0.95));
			for (Int_t i = 0; i < nBolo; i++) {
				bIsBoloEvent = true;
				KHLASingleBoloSubRecord* mBolo2 = mEv->AddBolo();
				
				mBolo2->SetQvalue(gRandom->Rndm());
				mBolo2->SetEnergyRecoil(100.0*gRandom->Rndm());
				mBolo2->SetEnergyIon(100.0*gRandom->Rndm());
				mBolo2->SetEnergyHeat(100.0*gRandom->Rndm());
				
			}
		}
		fOutFile.Fill();
		
		
	}
	cout << "Done filling events." << endl;
	logFile << "Done filling events." << endl;
	fOutFile.Write();
	cout << "Write Tree" << endl;
	logFile << "Write Tree" << endl;
	fOutFile.Close();
	cout << "Closing File" << endl;
	logFile << "Closing File" << endl;
	
	
}
