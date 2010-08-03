/*
 *  fillEvents.cc
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 4/6/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "TFile.h"
#include "EdwHLaMCEvent.h"
//#include "EdwTestEventClass.h"
#include "EdwHLAMuonModuleSubRecord.h"
#include "EdwMCSingleBoloSubRecord.h"
#include "TTree.h"
#include <iostream>
#include "TRandom.h"
#include "TTimeStamp.h"
#include <unistd.h>
#include "TCut.h"
#include "TClass.h"
#include "TBits.h"
#include "EdwEventFactory.h"
#include "EdwDSWriter.h"
#include "TString.h"
#include <fstream>

//#include "TObjectTable.h"

using namespace std;

int main(int argc, char* argv[]) 
{
	
	cout << "Welcome to Fill Event" << endl;
	TString inFile = argv[1];
	TString outFile = argv[2];
	
	EdwDSWriter mOutput(outFile.Data(),"HLaMC","recreate");
	EdwHLaMCEvent *mEv = dynamic_cast<EdwHLaMCEvent*>(mOutput.GetEvent());
	
	ifstream input;
	input.open(inFile,ios::in);
	if (!input.is_open()) {cout << "Could not open" << inFile << endl;}
	
	Int_t mEventID;
	Int_t mNumHits;
	Int_t mTrackID;
	Int_t mParentID;
	Double_t mTime;
	string mParticleName;
	string mProcess;
	string mCreationProcess;
	Double_t mEnergy;
	string mVolume;
	Double_t mXpos;
	Double_t mYpos;
	Double_t mZpos;
	
	while(!input.eof())
{
		
		mEv->Clear("C"); //clear the event at the beginning, just to be sure. 
		/*
		if((i % 2000) == 0) 
			cout << i << " events filled" << endl;
		mEv->SetRunNumber(12);
		//mEv->SetDataCleaningWord(4);
		*/
		//read input stream and get mNumHits
		input >> mEventID >> mNumHits >> mTrackID >> mParentID >> mTime >> mParticleName >> mProcess >> mCreationProcess >> mEnergy >> mVolume >> mXpos >> mYpos >> mZpos;
		
		//fill the bolo with all hits of one event
		for(Int_t i = 0; i < mNumHits; i++) {
			EdwMCSingleBoloSubRecord* mBolo = mEv->AddBolo();
			mBolo->SetEventID(mEventID);
			mBolo->SetNumHits(mNumHits);
			mBolo->SetTrackID(mTrackID);
			mBolo->SetParentID(mParentID);
			mBolo->SetTime(mTime);
			mBolo->SetParticleName(mParticleName.c_str(), mParticleName.length());
			mBolo->SetProcess(mProcess.c_str(), mProcess.length());
			mBolo->SetCreationProcess(mCreationProcess.c_str(), mCreationProcess.length());
			mBolo->SetEnergyRecoil(mEnergy);
			mBolo->SetDetectorName(mVolume.c_str(), mVolume.length());
			mBolo->SetXpos(mXpos);
			mBolo->SetYpos(mYpos);
			mBolo->SetZpos(mZpos);
			
			if (i < mNumHits) {
				input >> mEventID >> mNumHits >> mTrackID >> mParentID >> mTime >> mParticleName >> mProcess >> mCreationProcess >> mEnergy >> mVolume >> mXpos >> mYpos >> mZpos;
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
		
		
		 mOutput.Fill();
		 
	}

	cout << "Done filling events." << endl;
	//gObjectTable->Print();
	mOutput.Write();
	
	input.close();
	
	cout << "Write Tree" << endl;
	//gObjectTable->Print();
	mOutput.Close();
	cout << "Closing File" << endl;
	//gObjectTable->Print();
	//delete mEv;
	//EdwEventFactory::DeleteEvent(mEv);
		
	//cout << "Clean up EdwHLAEvent object " << endl;

	//gObjectTable->Print();
}
