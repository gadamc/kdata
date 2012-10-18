/*
 *  skimNoNoiseBolos.cxx
 *  KDataStructure
 *
 *  Created by Adam Cox on 5/18/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


#include "KDataReader.h"
#include "KDataWriter.h"
#include "TFile.h"
#include "TH1I.h"
#include "KHLAEvent.h"
#include "KEvent.h"
#include "KHLAMuonModuleRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLASambaRecord.h"
#include <stdlib.h>
#include <iostream>
#include "TString.h"
#include "TClonesArray.h"
#include "TProcessID.h"

using namespace std;

int main(int argc, char* argv[] )
{
	
	cout << "Skimming Out Bolometer Noise Events (fEventFlag == 0) ";
	
	TString inEDSFile = argv[1];
	TString outFile = argv[2];
	Int_t i = 3;
	Int_t fEventFlagCut = -1;
	
  while ( i < argc ){

		if( strcmp(argv[i],"--eventFlag") == 0 ){
      fEventFlagCut = atoi(argv[++i]);
      if(fEventFlagCut > 0 && fEventFlagCut < 6)
				cout << " only include events with EventFlag set to " << fEventFlagCut;
			else {
				cout << " ignoring invalid Event Flag cut: " << fEventFlagCut << endl;
				fEventFlagCut = -1;
			}
				
    }
    
    else{
      cout << endl;
      cout << argv[i] << " is not a recognized option." << endl << endl;
    }
    i++;
  }
  cout << endl << endl;
 
	cout << "Input file         " << inEDSFile.Data() << endl;
	cout << "Output file        " << outFile.Data() << endl;
	
	KDataWriter fOutEds(outFile.Data(), "KHLAEvent");
	KEvent *eout = fOutEds.GetEvent();
	KHLAEvent *mEout =  dynamic_cast<KHLAEvent*>( eout );
	
	KDataReader fEdsFile(inEDSFile.Data());
	KEvent *ein = fEdsFile.GetEvent();
	KHLAEvent *mEv = dynamic_cast<KHLAEvent*>( ein );
	
	if(mEv == 0) return -1;
	if(mEout == 0) return -1;
	
	Bool_t isGoodEvent = false;
	Int_t counter = 0;
	do
	{
		isGoodEvent =  false;
		mEout->Clear("C");
		Int_t ObjectNumber = TProcessID::GetObjectCount();
		
		//mEout->Set(*mEv,fEdsFile);
		*mEout = *mEv;
		
		mEout->IsSame(*mEv, true);  
		
		if(mEv->GetNumBolos() > 0){
			//manually clear out all of the bolo sub records and
			//samba records. then add them back in accordingly if they
			//are valid. 
			
			mEout->ClearArrays("C");
			if(mEout->AddBoloSubRecords(*mEv, true) > 0) // Add the SubRecords, but only use the skimNoise option.
				mEout->AddMuonModuleSubRecords(*mEv);
        isGoodEvent = true; 
			

		}
		
		if(mEout->GetNumMuonModules() > 0)
			isGoodEvent = true;
		
		if(isGoodEvent)
			fOutEds.Fill();
		
		if(++counter % 50000 == 0)
			cout << "Processing event " << counter << " out of " << fEdsFile.GetEntries() << endl;
		
		//Restore Object count                                                                                                     
		//To save space in the table keeping track of all referenced objects 
		//and computation time,
		//we assume that our events do not address each other. We reset the                                                        
		//object count to what it was at the beginning of the event.                                                               
		TProcessID::SetObjectCount(ObjectNumber);
		
	} while(fEdsFile.GetNextEntry() > 0);
	
	fOutEds.Write();
	fOutEds.Close();
	fEdsFile.Close();
	return 1;
}