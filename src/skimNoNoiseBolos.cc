/*
 *  skimNoNoiseBolos.cpp
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 5/18/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


#include "EdwDSReader.h"
#include "EdwDSWriter.h"
#include "TFile.h"
#include "TH1I.h"
#include "EdwHLAEvent.h"
#include "EdwEventBase.h"
#include "EdwHLAMuonModuleSubRecord.h"
#include "EdwHLASingleBoloSubRecord.h"
#include "EdwHLASambaSubRecord.h"
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
	
	EdwDSWriter fOutEds(outFile.Data(), "HLA", "recreate");
	EdwEventBase *eout = fOutEds.GetEvent();
	EdwHLAEvent *mEout =  dynamic_cast<EdwHLAEvent*>( eout );
	
	EdwDSReader fEdsFile(inEDSFile.Data());
	EdwEventBase *ein = fEdsFile.GetEvent();
	EdwHLAEvent *mEv = dynamic_cast<EdwHLAEvent*>( ein );
	
	if(mEv == 0) return -1;
	if(mEout == 0) return -1;
	
	Bool_t isGoodEvent = false;
	Int_t counter = 0;
	do
	{
		isGoodEvent =  false;
		mEout->Clear("C");
		Int_t ObjectNumber = TProcessID::GetObjectCount();
		
		*mEout = *mEv;
		
		mEout->IsSame(*mEv, true); //is there a problem with the Stamp? 
		
		if(mEv->GetNumBolos() > 0){
			//manually clear out all of the bolo sub records and
			//samba records. then add them back in accordingly if they
			//are valid. 
			mEout->GetBoloSubRecords()->Clear("C");
			mEout->GetSambaSubRecords()->Clear("C");
			mEout->SetNumBolo(0);
			mEout->SetNumSamba(0);
			for(Int_t i = 0; i < mEv->GetNumBolos(); i++){
				if(mEv->GetBolo(i)->GetEventFlag() > 0 &&
					 (fEventFlagCut > 0 ? mEv->GetBolo(i)->GetEventFlag()==fEventFlagCut : true)
					 ){  //0 are 'noise' events.
					isGoodEvent = true;
					//this should be the same as cutting events where the EventFlag == 0
					EdwHLASingleBoloSubRecord *inBolo = mEv->GetBolo(i);
					EdwHLASingleBoloSubRecord *outBolo = mEout->AddBolo();
					EdwHLASambaSubRecord *outSamba = mEout->AddSamba();
					*outBolo = *inBolo;  //copy the bolo sub record
					EdwHLASambaSubRecord *inSamba = inBolo->GetSambaRecord();
					//cout << "Copying Samba Record" << endl;
					*outSamba = *inSamba; //copy the samba sub record information 
					//cout << "Setting Samba Record" << endl;
					
					//cout << "GetSambaRecord in: " << inBolo->GetSambaRecord() << endl;
					//cout << "GetSambaRecord out: " << outBolo->GetSambaRecord() << endl;
					outBolo->SetSambaRecord(outSamba);
					//cout << "GetSambaRecord in: " << inBolo->GetSambaRecord() << endl;
					//cout << "GetSambaRecord out: " << outBolo->GetSambaRecord() << endl;
					
				}
			}
			
		}
		
		if(mEv->GetNumMuonModules() > 0)
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