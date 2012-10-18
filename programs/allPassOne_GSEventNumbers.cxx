/*
 *  allPassOne_GSEventNumbers.cxx
 *  KDataStructure
 *
 *  Created by Adam Cox on 6/1/10.
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
	cout << "Filling Global Software Event Number" << endl;
	cout << "The number of arguments should be 3. You passed in " << argc << endl;
	for(Int_t i = 0; i < argc; i++){
		cout << argv[i] << " ";
	}
	cout << endl << endl;
	
	TString inEDSFile = argv[1];
	TString outFile = argv[2];
	cout << "Input file         " << inEDSFile.Data() << endl;
	cout << "Output file        " << outFile.Data() << endl;
	
	KDataWriter fOutEds(outFile.Data());
	KEvent *eout = fOutEds.GetEvent();
	KHLAEvent *mEvOut =  dynamic_cast<KHLAEvent*>( eout );
	
	KDataReader fInEds(inEDSFile.Data());
	KEvent *ein = fInEds.GetEvent();
	KHLAEvent *mEvIn = dynamic_cast<KHLAEvent *>( ein );
	
	if(mEvOut == 0) return -1;
	if(mEvIn == 0) return -1;
	
	Int_t fGSEventNumber = 0;	
	
	for(Int_t i = 0; i < fInEds.GetEntries(); i++){
		
		//cout << i << endl;
		mEvOut->Clear("C");
		//mEvIn->Clear("C");
		
		fInEds.GetEntry(i);
		
		//mEvIn->Dump();
		
		//mEvOut->Set(*mEvIn, fInEds);
		*mEvOut = *mEvIn;
		
		//mEvOut->Dump();
		//mEvIn->Dump();
		
		if(!mEvOut->IsSame(*mEvIn, true)){
			cout << "Break Here" << endl;
			
			//for(Int_t s = 0; s < mEvOut->GetNumSambas(); s++){
			//	KHLASambaRecord *sam = mEvOut->GetSamba(s);
			//}
		} //is there a problem with the copy? 
		
		//cout << "After Is Same" << endl << endl;
		//mEvOut->Dump();
		//mEvIn->Dump();
		
		mEvOut->SetGSEventNumber(fGSEventNumber++);
		
		//cout << "After Setting GS Event Number" << endl;
		
		//mEvOut->Dump();
		//mEvIn->Dump();
		
		fOutEds.Fill();
		
		if(fGSEventNumber % 50000 == 0)
			cout << "Processing event " << fGSEventNumber << " out of " << fInEds.GetEntries() << endl;
		
		
		
	} 
	
	
	fInEds.Close();
		
	fOutEds.Write();
	fOutEds.Close();
	
	return 1;
}