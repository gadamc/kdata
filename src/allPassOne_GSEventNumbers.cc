/*
 *  allPassOne_GSEventNumbers.cc
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 6/1/10.
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
	cout << "Filling Global Software Event Number" << endl;
	cout << "The number of arguments should be 3. You passed in " << argc << endl;
	for(Int_t i = 0; i < argc; i++){
		cout << argv[i];
	}
	cout << endl << endl;
	
	TString inEDSFile = argv[1];
	TString outFile = argv[2];
	cout << "Input file         " << inEDSFile.Data() << endl;
	cout << "Output file        " << outFile.Data() << endl;
	
	EdwDSWriter fOutEds(outFile.Data());
	EdwEventBase *eout = fOutEds.GetEvent();
	EdwHLAEvent *mEvOut =  dynamic_cast<EdwHLAEvent*>( eout );
	
	EdwDSReader fInEds(inEDSFile.Data());
	EdwEventBase *ein = fInEds.GetEvent();
	EdwHLAEvent *mEvIn = dynamic_cast<EdwHLAEvent *>( ein );
	
	if(mEvOut == 0) return -1;
	if(mEvIn == 0) return -1;
	
	Int_t fGSEventNumber = 0;	
	
	do
	{
		mEvOut->Clear("C");
		
		*mEvOut = *mEvIn;
		
		mEvOut->IsSame(*mEvIn, true); //is there a problem with the copy? 
		
		mEvOut->SetGSEventNumber(fGSEventNumber++);
		
		fOutEds.Fill();
		
		if(fGSEventNumber % 50000 == 0)
			cout << "Processing event " << fGSEventNumber << " out of " << fInEds.GetEntries() << endl;
		
		
		
	} while(fInEds.GetNextEntry() > 0);
	
	
	fInEds.Close();
		
	fOutEds.Write();
	fOutEds.Close();
	
	return 1;
}