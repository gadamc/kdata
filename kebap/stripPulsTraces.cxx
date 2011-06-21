/*
*  stripPulsTraces.cxx
*  KDataStructure
*
*  Created by Benjamin Schmidt, based on all_pass one GS event number as of 6/1/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/

//Usage: .../stripPulsTrace /path/to/inputFile /path/to/outputFile 1
//3rd argument (0 or 1) to determine override files totally useless at the moment!


#include "KDataReader.h"
#include "KDataWriter.h"
#include "TFile.h"
#include "TH1I.h"
#include "KRawEvent.h"
#include "KEvent.h"
#include "KHLAMuonModuleRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawSambaRecord.h"
#include <stdlib.h>
#include <iostream>
#include "TString.h"
#include "TClonesArray.h"
#include "TProcessID.h"

using namespace std;

int main(int argc, char* argv[] )
{
//0   argument programm file;
//1st argument input file; 
//2nd argument output file;
//3rd argument override;
	cout << "Stripping pulse traces to remove file size for testing" << endl;
	cout << "The number of arguments should be 3. You passed in " << argc << endl;
	for(Int_t i = 0; i < argc; i++){
		cout << argv[i] << " ";
	}
	cout << endl << endl;
	
	TString inEDSFile = argv[1];
	TString outFile = argv[2];
	cout << "Input file         " << inEDSFile.Data() << endl;
	cout << "Output file        " << outFile.Data() << endl;
	
	
	KDataReader fInEds(inEDSFile.Data());
	KEvent *ein = fInEds.GetEvent();
	KRawEvent *mEvIn = dynamic_cast<KRawEvent *>( ein );
	
	KDataWriter fOutEds(outFile.Data(), mEvIn->GetClassName(), "recreate");
	KEvent *eout = fOutEds.GetEvent();
	KRawEvent *mEvOut =  dynamic_cast<KRawEvent*>( eout );
	
	
	cout << mEvOut<< "<------------" << mEvIn << endl;
	if(mEvOut == 0) return -1;
	if(mEvIn == 0) return -1;

	cout << "Processing "<< fInEds.GetEntries()<<" entries" << endl;
	for(Int_t i = 0; i < fInEds.GetEntries(); i++){
		if(i%100==0)cout << "forLoop " << i << endl;
		//cout << i << endl;
		mEvOut->Clear("C");
		//mEvIn->Clear("C");
		
		fInEds.GetEntry(i);

		
//		if(!mEvOut->IsSame(*mEvIn, true)){
//			cout << "Break Here" << endl;
//
//			//for(Int_t s = 0; s < mEvOut->GetNumSambas(); s++){
//				//	KHLASambaRecord *sam = mEvOut->GetSamba(s);
//				//}
//		} //is there a problem with the copy?
		
		
		mEvOut->SetEventTriggerStamp(mEvIn->GetStamp()); //assign the ClonesArrayPointer to a 0 value?
		for(Int_t j=0; j< mEvIn->GetNumBolos(); j++ ){
			//mEvOut->AddBolo(((KRawBolometerRecord*)mEvIn->GetBolo(i)));
			KRawBolometerRecord boloRec=*((KRawBolometerRecord*)mEvIn->GetBolo(j)); //assignment copies local members, but sets the pointers to 0
			KRawBolometerRecord *boloRecPointer=(KRawBolometerRecord*)mEvIn->GetBolo(j);
			KRawSambaRecord* sambaRec=mEvOut->AddSamba();
			*sambaRec=*(boloRecPointer->GetSambaRecord());
			boloRec.SetSambaRecord(sambaRec);
			mEvOut->AddBoloSubRecord(boloRec); //This method should take care of adding the samba record only once per mac
		}
		//cout << mEvOut->GetStamp()<< endl;
	fOutEds.Fill();
	}
	cout << "\n \n Done processing " << fInEds.GetEntries() << " entries" << endl;
	fInEds.Close();
	fOutEds.Write();
	fOutEds.Close();
	
	return 1;
}
