/*
 *  fillEventTest.cc
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/18/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include "TFile.h"
#include "EdwHLAEvent.h"
#include "EdwTestEventClass.h"
#include "EdwHLAMuonModuleSubRecord.h"
#include "EdwHLASingleBoloSubRecord.h"
#include "TTree.h"
#include <iostream>
#include "TRandom.h"

using namespace std;

int main(int argc, char* argv[]) 
{

	cout << "Welcome to Fill Event" << endl;
	
	TFile f("new.root","recreate");
	cout << "Open Root File" << endl;
	
	EdwHLAEvent *mEv = new EdwHLAEvent();
	cout << "Created EdwHLAEvent" << endl;
	
	TTree *t = new TTree("EdwT","EdwEventTree");
	cout << "Created TTree" << endl;
	
	t->Branch("EdwHLAEventBranch","EdwHLAEvent",&mEv,32000,99); 
	//t->BranchRef(); //creates an extra Branch table filled with Branches that have TRefs or TRefArrays
	cout << "Set Branch new.root and filling events" << endl;
	
	gRandom->SetSeed(0);
	Int_t numEvents = 70000 + (Int_t)20000*gRandom->Rndm();
	cout <<"Will create " << numEvents << " events" << endl;
	Bool_t bIsBoloEvent, bIsMuonEvent; 
	for(Int_t i = 0; i < numEvents; i++){
		bIsBoloEvent = bIsMuonEvent = false;
		if((i % 2000) == 0) 
			cout << i << " events filled" << endl;
		mEv->SetRunNumber(i);
		//mEv->SetDataCleaningWord(4);
		mEv->SetEventTime(3.023432*i);
		mEv->SetGSEventNumber(3*i+1994);
		//5% chance of a Muon event... which is probably way too huge.
		if(gRandom->Rndm() < 0.05){
			Int_t nMod = 1 + (Int_t)(3*gRandom->Rndm());
			mEv->GetHLAMuonVetoSysRecord()->SetIsMuonSystemOn(true);
			mEv->GetHLAMuonVetoSysRecord()->SetRunNumber(63);
			mEv->GetHLAMuonVetoSysRecord()->SetRunStartTime(1257638400);  //Nov 8, 2009 at 00:00
			mEv->GetHLAMuonVetoSysRecord()->SetRunEndTime(1270512000);  //April 6, 2010 at 00:00
			
			for(Int_t i = 0; i < nMod; i++) {
				bIsMuonEvent = true;
				EdwHLAMuonModuleSubRecord* mMuonModule = mEv->AddMuonModule();
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
				EdwHLASingleBoloSubRecord* mBolo2 = mEv->AddSingleBolo();
				
				mBolo2->SetQvalue(gRandom->Rndm());
				mBolo2->SetEnergyRecoil(100.0*gRandom->Rndm());
				mBolo2->SetEnergyIon(100.0*gRandom->Rndm());
				mBolo2->SetEnergyHeat(100.0*gRandom->Rndm());
				
			}
		}
		t->Fill();
		//must now clear the event so that the TClonesArrays are cleared
		//and their objects are deleted. 
		if(bIsBoloEvent || bIsMuonEvent) mEv->Clear();  
	}
	cout << "Done filling events." << endl;
	t->Write();
	cout << "Write Tree" << endl;
	f.Close();
	cout << "Closing File" << endl;
	delete mEv;
	cout << "Clean up EdwHLAEvent object " << endl;
	/*
	cout << "Welcome to Fill test Event" << endl;
	TFile ff("testnew32.root","recreate");
	cout << "Open Root File" << endl;
	EdwTestEventClass *mTEv = new EdwTestEventClass();
	cout << "Created EdwTestEventClass" << endl;
	TTree *tt = new TTree("EdwTest","EdwEventTreeTest");
	cout << "Created TTree" << endl;
	tt->Branch("EdwTestEventBranch","EdwTestEventClass",&mTEv,32000,99); 
	cout << "Set Branch and about to fill events" << endl;
	for(Int_t i = 0; i < 100000; i++){
		mTEv->SetRunNumber(i);
		mTEv->SetDataCleaning(4);
		mTEv->SetEventTime(10.023432*i);
		mTEv->SetDataCleaning(0x2);
		mTEv->SetGSEventNumber(3*i+1994);
		tt->Fill();
	}
	cout << "Done filling events." << endl;
	tt->Write();
	cout << "Write Tree" << endl;
	ff.Close();
	cout << "Closing File" << endl;
	delete mTEv;
	cout << "Clean up EdwTestEvent object " << endl;
	 */
}
