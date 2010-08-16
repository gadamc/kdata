/*
 *  fillEvents.cxx
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/6/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "TFile.h"
#include "KHLAEvent.h"
#include "KHLAMuonModuleRecord.h"
#include "KHLABolometerRecord.h"
#include "TTree.h"
#include <iostream>
#include "TRandom.h"
#include "TTimeStamp.h"
#include <unistd.h>
#include "TCut.h"
#include "TClass.h"
#include "TBits.h"
#include "KEventFactory.h"
#include "KDataWriter.h"

//#include "TObjectTable.h"

using namespace std;

int main(int /*argc*/, const char* /*argv[]*/) 
{
	
	cout << "Welcome to Fill Event" << endl;
	
	//We don't need the fUniqueID and fBits words in
	//the TObject for these classes. 
	//TClass::GetClass("TBits")->IgnoreTObjectStreamer();
	//TClass::GetClass("KHLAMuonModuleRecord")->IgnoreTObjectStreamer();
	/*
	TFile f("new.root","recreate");
	cout << "Open Root File" << endl;
	
	KHLAEvent *mEv = KEventFactory::NewHLAEvent();
	//KHLAEvent *mEv = new KHLAEvent;
	cout << "Created KHLAEvent" << endl;
	
	TTree *t = new TTree("t","KEventTree");
	cout << "Created TTree" << endl;
	
	t->Branch("event","KHLAEvent",&mEv,32000,99); 
	
	t->BranchRef(); //creates an extra Branch table filled with Branches that have TRefs or TRefArrays
	cout << "Set Branch new.root and filling events" << endl;
	//t->GetBranch("fKHLABoloSysRecord")->SetName("boloSys");
	//t->GetBranch("fKHLAMuonVetoSysRecord")->SetName("muonSys");
	//standard event information
*/
	
	//mEv->SetRunStartTime(
	//gObjectTable->AddObj((TObject *)mEv);
	
	//gObjectTable->Print();
	
	KDataWriter mOutput("new.root","HLA","recreate");
	KHLAEvent *mEv = dynamic_cast<KHLAEvent*>(mOutput.GetEvent());
		
	/*if(mEv == 0){
		cout << "KDataWriter returned a NULL pointer from GetEvent" << endl;
		return -1;
	}*/
	
	gRandom->SetSeed(0);
	Int_t numEvents = 70000 + (Int_t)2000*gRandom->Rndm();

	cout <<"Will create " << numEvents << " events" << endl;
	Bool_t bIsBoloEvent, bIsMuonEvent; 
	for(Int_t i = 0; i < numEvents; i++){
		bIsBoloEvent = bIsMuonEvent = false;
		mEv->Clear("C"); //clear the event at the beginning, just to be sure. 
		
		if((i % 2000) == 0) 
			cout << i << " events filled" << endl;
		mEv->SetRunNumber(12);
		//mEv->SetDataCleaningWord(4);
		mEv->SetEventTriggerTime(3.023432*i + 1257638400);
		mEv->SetGSEventNumber(i);
		//gObjectTable->AddObj((TObject *)mEv->GetMuonVetoSystemRecord());
		//gObjectTable->AddObj((TObject *)mEv->GetHLABoloSysRecord());
		mEv->GetBoloSystemRecord()->SetRunStartTime(1257638400);
		mEv->GetBoloSystemRecord()->SetRunEndTime(1270512000);
		mEv->GetMuonVetoSystemRecord()->SetIsSystemOn(true);
		mEv->GetMuonVetoSystemRecord()->SetRunNumber(63);
		mEv->GetMuonVetoSystemRecord()->SetRunStartTime(1257638400);  //Nov 8, 2009 at 00:00
		mEv->GetMuonVetoSystemRecord()->SetRunEndTime(1270512000);  //April 6, 2010 at 00:00
		
		
		//5% chance of a Muon event... which is probably way too huge.
		if(gRandom->Rndm() < 0.5){
			Int_t nMod = 1 + (Int_t)(3*gRandom->Rndm());
						
			for(Int_t i = 0; i < nMod; i++) {
				bIsMuonEvent = true;
				KHLAMuonModuleRecord* mMuonModule = mEv->AddMuonModule();
				mMuonModule->SetModuleNumber(40*gRandom->Rndm());
				mMuonModule->SetAdc(0,(Int_t)(290*gRandom->Rndm()));
				mMuonModule->SetAdc(1,(Int_t)(4056*gRandom->Rndm()));
				mMuonModule->SetTdc(0,200.0*gRandom->Rndm());
				mMuonModule->SetTdc(1,mMuonModule->GetTdc(0) + 200.0*gRandom->Rndm());
				mMuonModule->SetEnergy(0,(Int_t)(2048*gRandom->Rndm()/1.0e6)); //divide by 1 MeV...?
				mMuonModule->SetEnergy(1,(Int_t)(2048*gRandom->Rndm()/1.0e6));
			}
			
		}
		else {
						
			Int_t nBolo = 1 + (gRandom->Poisson(0.95));
			for (Int_t i = 0; i < nBolo; i++) {
				bIsBoloEvent = true;
				KHLABolometerRecord* mBolo2 = mEv->AddBolo();
				
				mBolo2->SetQvalue(gRandom->Rndm());
				mBolo2->SetEnergyRecoil(100.0*gRandom->Rndm());
				mBolo2->SetEnergyIon(100.0*gRandom->Rndm());
				mBolo2->SetEnergyHeat(100.0*gRandom->Rndm());
				if(i==0) {
					mBolo2->SetDetectorName("FID401",6);
					mBolo2->SetDetectorNumber(5);
				}
				else {
					mBolo2->SetDetectorName("ID2",3);
					mBolo2->SetDetectorNumber(2);
				}
			}
		 
		}
		/*
		if(i == 1){
			gObjectTable->Print();
		}
		
		if(i == 10000000)
			gObjectTable->Print();
		
		if(i == 14000000)
			gObjectTable->Print();
		*/
		
		
		if(bIsBoloEvent || bIsMuonEvent) mOutput.Fill();
		 
	}

	cout << "Done filling events." << endl;
	//gObjectTable->Print();
	mOutput.Write();
	
	
	cout << "Write Tree" << endl;
	//gObjectTable->Print();
	mOutput.Close();
	cout << "Closing File" << endl;
	//gObjectTable->Print();
	//delete mEv;
	//KEventFactory::DeleteEvent(mEv);
		
	//cout << "Clean up KHLAEvent object " << endl;

	//gObjectTable->Print();
}
