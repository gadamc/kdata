/*
 *  testAllEventsForUniqueIDs.cxx
 *  kData
 *
 *  Created by Adam Cox on 9/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include "KHLAEvent.h"
#include "KDataReader.h"
#include "KHLASambaRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLAMuonModuleRecord.h"
#include "KHLABoloPulseRecord.h"
#include <vector> 
#include <iostream>

using namespace std;

int main(int argc, char* argv[] ) 
{
	cout << "testAllEventsForUniqueIDs" << endl;
	
	if(argc != 2){
		cout << "Incorrect number of args" << endl;
		return -1;
	}
	
	KDataReader f(argv[1]);
	KHLAEvent *e = (KHLAEvent *)f.GetEvent();
	
	vector<UInt_t> listOfIDs;
	
	for(Int_t i = 0; i < f.GetEntries(); i++){
		f.GetEntry(i);
		listOfIDs.clear();
		
		for(Int_t s = 0; s < e->GetNumSambas(); s++)
			listOfIDs.push_back(e->GetSamba(s)->GetUniqueID());
		
		for(Int_t b = 0; b < e->GetNumBolos(); b++)
			listOfIDs.push_back(e->GetBolo(b)->GetUniqueID());
		
		for(Int_t m = 0; m < e->GetNumMuonModules(); m++)
			listOfIDs.push_back(e->GetMuonModule(m)->GetUniqueID());
		
		for(Int_t p = 0; p < e->GetNumBoloPulses(); p++)
			listOfIDs.push_back(e->GetBoloPulse(p)->GetUniqueID());
		
		for(UInt_t k = 0; k < listOfIDs.size(); k++){
			UInt_t thisID = listOfIDs.at(k);
			for(UInt_t j = k+1; j < listOfIDs.size(); j++){
				if(thisID == listOfIDs.at(j)){
					cout << "Sound the Alarm Bitches!" << endl;
					cout << "Found two ID's that are the same" << endl;
					cout << "Entry " << i << endl;
				}
			}
		}
	}
	
	cout << "If nothing was printed to screen, then congratulations, all UniqueIDs are unique within each Event" << endl;
	cout << "Otherwise... Fail." << endl;
	return 0;
}
