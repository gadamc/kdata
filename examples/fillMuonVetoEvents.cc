/*
 *  fillMuonVetoEvents.cc
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/16/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include <string>
#include <iostream>
//#include <stdio>

//ROOT includes
#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TBits.h"

//dMEAT includes
#include "KHLAEvent.h"
#include "KHLAMuonModuleSubRecord.h"
#include "KHLAMuonVetoSysRecord.h"
#include "KHLASingleBoloSubRecord.h"
#include "KHLASambaSubRecord.h"
#include "KDataWriter.h"

TFile *mMuonVetoRootFile = 0;
KDataWriter *mKEDSOutFile = 0;
TTree *mMuonTree = 0;  
KHLAEvent *mEvent = 0;

//warning! These have to be the same everywhere! Might need to 
//consider some sort of Singleton class. Or... better yet, 
//the KTree Class which does all of this tree buiding
//and manipulating. anyways....

const Int_t kKRunNumber = 12;
const Double_t kKRun12StartTime = 1235646944.0; //Thu, 26 Feb 2009 11:15:44 +0000 (GMT) +        0 nsec  First event of Muon Veto Run 54.
const Double_t kKRun12EndTime = 1274115600.0; //Mon, 17 May 2010 15:00:00 +0000 (UTC) +        0 nsec.  Approximate end of Run 12.
const Int_t kNumMuonModules = 51; //48 modules, plus two spots for the NC counter Muon Module and PMT plus 1 for no Module Number == 0.
const Int_t kNumPmtsPerMod = 2;

const Int_t fSizeOfArray = 97;

struct MuonVetoData{
	ULong64_t f10muSecStamp; //should go in the Event
	Short_t fADC[fSizeOfArray]; //array of ADC values - attached to each module
	Short_t fADCPound[fSizeOfArray];  //if this is stored, it would go in the system record
	UInt_t fMultiADC;  //counts number of ADC channels with non-zero value
	Double_t fDistEst; //system record
	Double_t fDistNemo;  //system record
	UInt_t fEventNo; //system record
	UInt_t fFile;  //system record
	UChar_t fGeometry; //module specific
	UChar_t fMultiTDC; //counts number TDC channels with non-zero (should always be at least 1)
	UInt_t fRun; //system record
	TBits *fEventQualityBits; //system record
	Short_t fTDC[fSizeOfArray]; //array of TDC values - attached to each module
	Short_t fTDCPound[fSizeOfArray]; //if this is stored, it would go in the system record
	Double_t fDeltaT; //system record
	Int_t fPcTimeSec; //system record
	Int_t fPcTimeMicroSec; //system record
};

MuonVetoData mMuonVetoData;

const Int_t sizeOfIndex = 52;

struct MuonChannelMap {	
	Int_t adc_index[sizeOfIndex][kNumPmtsPerMod];
	Int_t tdc_index[sizeOfIndex][kNumPmtsPerMod];
};

void initializeMuonChannelMap(MuonChannelMap &map, UInt_t runNum)
{
	map.adc_index[1][0]  = 0;     map.adc_index[1][1] = 8;
	map.tdc_index[1][0]  = 0;     map.tdc_index[1][1] = 8;
	
	map.adc_index[2][0]  = 1;     map.adc_index[2][1] = 9;
	map.tdc_index[2][0]  = 1;     map.tdc_index[2][1] = 9; 
	
	map.adc_index[3][0]  =  2;    map.adc_index[3][1] = 10; 
	map.tdc_index[3][0]  =  2;    map.tdc_index[3][1] = 10;  
	
	map.adc_index[4][0]  = 16;    map.adc_index[4][1] = 24;
	map.tdc_index[4][0]  = 16;    map.tdc_index[4][1] = 24;  
	
	map.adc_index[5][0]  = 17;    map.adc_index[5][1] = 25;  
	map.tdc_index[5][0]  = 17;    map.tdc_index[5][1] = 25;  
	
	map.adc_index[6][0]  = 18;    map.adc_index[6][1] = 26;  
	map.tdc_index[6][0]  = 18;    map.tdc_index[6][1] = 26;  
	
	map.adc_index[7][0]  = -99;     map.adc_index[7][1]  = -99;
	map.tdc_index[7][0]  = -99;     map.tdc_index[7][1]  = -99;
	
	map.adc_index[8][0]  = -99;     map.adc_index[8][1]  = -99;
	map.tdc_index[8][0]  = -99;     map.tdc_index[8][1]  = -99;
	
	map.adc_index[9][0]  =  4;    map.adc_index[9][1] = 12;  
	map.tdc_index[9][0]  =  4;    map.tdc_index[9][1] = 12;  
	
	map.adc_index[10][0] =  5;    map.adc_index[10][1] = 13;  
	map.tdc_index[10][0] =  5;    map.tdc_index[10][1] = 13;  
	
	map.adc_index[11][0] =  6;    map.adc_index[11][1] = 14;  
	map.tdc_index[11][0] =  6;    map.tdc_index[11][1] = 14;  
	
	map.adc_index[12][0] = 20;    map.adc_index[12][1] = 28; 
	map.tdc_index[12][0] = 20;    map.tdc_index[12][1] = 28;  
	
	map.adc_index[13][0] = 21;    map.adc_index[13][1] = 29;  
	map.tdc_index[13][0] = 21;    map.tdc_index[13][1] = 29; 
	
	map.adc_index[14][0] = 22;    map.adc_index[14][1] = 30;   
	map.tdc_index[14][0] = 22;    map.tdc_index[14][1] = 30;   
	
	map.adc_index[15][0] = -99;  map.adc_index[15][1] = -99; 
	map.tdc_index[15][0] = -99;  map.tdc_index[15][1] = -99; 
	
	map.adc_index[16][0] = -99;  map.adc_index[16][1] = -99;
	map.tdc_index[16][0] = -99;  map.tdc_index[16][1] = -99;
	
	map.adc_index[17][0] = 32;    map.adc_index[17][1] = 40;   
	map.tdc_index[17][0] = 32;    map.tdc_index[17][1] = 40;   
	
	map.adc_index[18][0] = 33;    map.adc_index[18][1] = 41; 
	map.tdc_index[18][0] = 33;    map.tdc_index[18][1] = 41;  
	
	map.adc_index[19][0] = 34;    map.adc_index[19][1] = 42; 
	map.tdc_index[19][0] = 34;    map.tdc_index[19][1] = 42; 
	
	map.adc_index[20][0] = 48;    map.adc_index[20][1] = 56;  
	map.tdc_index[20][0] = 35;    map.tdc_index[20][1] = 43;  
	
	map.adc_index[21][0] = 49;    map.adc_index[21][1] = 57; 
	map.tdc_index[21][0] = 51;    map.tdc_index[21][1] = 59;  
	
	map.adc_index[22][0] = 19;    map.adc_index[22][1] = 27;  
	map.tdc_index[22][0] = 19;    map.tdc_index[22][1] = 27;  
	
	map.adc_index[23][0] = -99;   map.adc_index[23][1] = -99;
	map.tdc_index[23][0] = -99;  map.tdc_index[23][1] = -99;
	
	map.adc_index[24][0] = -99; map.adc_index[24][1] = -99;	
	map.tdc_index[24][0] = -99;  map.tdc_index[24][1] = -99;
	
	map.adc_index[25][0] = 36;    map.adc_index[25][1] = 44;  
	map.tdc_index[25][0] = 36;    map.tdc_index[25][1] = 44;  
	
	map.adc_index[26][0] = 37;    map.adc_index[26][1] = 45;  
	map.tdc_index[26][0] = 37;    map.tdc_index[26][1] = 45;  
	
	map.adc_index[27][0] = 38;    map.adc_index[27][1] = 46;   
	map.tdc_index[27][0] = 38;    map.tdc_index[27][1] = 46;   
	
	map.adc_index[28][0] = 39;    map.adc_index[28][1] = 47;   
	map.tdc_index[28][0] = 39;    map.tdc_index[28][1] = 47;   
	
	map.adc_index[29][0] = 52;    map.adc_index[29][1] = 60; 
	map.tdc_index[29][0] = 52;    map.tdc_index[29][1] = 60; 
	
	map.adc_index[30][0] = 53;    map.adc_index[30][1] = 61;  
	map.tdc_index[30][0] = 53;    map.tdc_index[30][1] = 61;  
	
	map.adc_index[31][0] = 54;    map.adc_index[31][1] = 62;  
	map.tdc_index[31][0] = 54;    map.tdc_index[31][1] = 62;  
	
	map.adc_index[32][0] = 55;    map.adc_index[32][1] = 63;  
	map.tdc_index[32][0] = 55;    map.tdc_index[32][1] = 63;   
	
	map.adc_index[33][0] = 64;    map.adc_index[33][1] = 72; 
	map.tdc_index[33][0] = 64;    map.tdc_index[33][1] = 72;  
	
	map.adc_index[34][0] = 65;    map.adc_index[34][1] = 73;   
	map.tdc_index[34][0] = 65;    map.tdc_index[34][1] = 73;   
	
	map.adc_index[35][0] = 66;    map.adc_index[35][1] = 74;  
	map.tdc_index[35][0] = 66;    map.tdc_index[35][1] = 74;  
	
	map.adc_index[36][0] = 67;    map.adc_index[36][1] = 75;  
	map.tdc_index[36][0] = 67;    map.tdc_index[36][1] = 75;  
	
	map.adc_index[37][0] = 80;    map.adc_index[37][1] = 88;  
	map.tdc_index[37][0] = 80;    map.tdc_index[37][1] = 88; 
	
	map.adc_index[38][0] = 81;    map.adc_index[38][1] = 89;  
	map.tdc_index[38][0] = 81;    map.tdc_index[38][1] = 89; 	
	
	/** This should solve the problem of non-working TDC[89]! Is this a good solution? */
	//tdc_index[38][1] = 81 was a way around the non-working TDC channel 89.
	// now, tdc_index[38][1] = 89 set back to original -- adam cox
	map.adc_index[39][0] = 82;    map.adc_index[39][1] = 90;  
	map.tdc_index[39][0] = 82;    map.tdc_index[39][1] = 90;  
	
	map.adc_index[40][0] = 83;    map.adc_index[40][1] = 91; 
	map.tdc_index[40][0] = 83;    map.tdc_index[40][1] = 91;  
	
	map.adc_index[41][0] = 68;    map.adc_index[41][1] = 76;  
	map.tdc_index[41][0] = 68;    map.tdc_index[41][1] = 76;  
	
	map.adc_index[42][0] = 69;    map.adc_index[42][1] = 77;   
	map.tdc_index[42][0] = 69;    map.tdc_index[42][1] = 77;   
	
	map.adc_index[43][0] = 70;    map.adc_index[43][1] = 78;  
	map.tdc_index[43][0] = 70;    map.tdc_index[43][1] = 78;  
	
	map.adc_index[44][0] = 71;    map.adc_index[44][1] = 79;  
	map.tdc_index[44][0] = 71;    map.tdc_index[44][1] = 79;  
	
	map.adc_index[45][0] = 84;    map.adc_index[45][1] = 92;  
	map.tdc_index[45][0] = 84;    map.tdc_index[45][1] = 92;  
	
	map.adc_index[46][0] = 85;    map.adc_index[46][1] = 93;  
	map.tdc_index[46][0] = 85;    map.tdc_index[46][1] = 93;  
	
	map.adc_index[47][0] = 86;    map.adc_index[47][1] = 94;  
	map.tdc_index[47][0] = 86;    map.tdc_index[47][1] = 94;  
	
	map.adc_index[48][0] = 87;    map.adc_index[48][1] = 95;  
	map.tdc_index[48][0] = 87;    map.tdc_index[48][1] = 95;  

	map.adc_index[49][0] = -99;    map.adc_index[49][1] = -99;  
	map.tdc_index[49][0] = -99;    map.tdc_index[49][1] = -99;  
	
	//50 and 51 are the neutron counter PMTs
	map.adc_index[50][0] = 3;     map.adc_index[50][1] = 11;  
	map.tdc_index[50][0] = 3;     map.tdc_index[50][1] = 11;  
	
	map.adc_index[51][0] = 23;    map.adc_index[51][1] = 31;  
	map.tdc_index[51][0] = 23;    map.tdc_index[51][1] = 31;  
	

	if(runNum >= 69){
		map.adc_index[7][0]  = 7;     map.adc_index[7][1]  = 15;
		map.tdc_index[7][0]  = 7;     map.tdc_index[7][1]  = 15;
		
		map.adc_index[8][0]  = 35;     map.adc_index[8][1]  = 43;
		map.tdc_index[8][0]  = 48;     map.tdc_index[8][1]  = 56;
		
		map.adc_index[15][0] = 51;  map.adc_index[15][1] = 59; 
		map.tdc_index[15][0] = 49;  map.tdc_index[15][1] = 57; 
		
		map.adc_index[16][0] = 50;  map.adc_index[16][1] = 58;
		map.tdc_index[16][0] = 50; map.tdc_index[16][1] = 58;
		
	}

}

Bool_t testMuonChannelMapForUniqueness(MuonChannelMap &map)
{
	Bool_t mapIsUnique = true;
	
	for(Int_t index = 1; index < sizeOfIndex; index++){
		for(Int_t pmt = 0; pmt < kNumPmtsPerMod; pmt++){
			Int_t adcVal = map.adc_index[index][pmt];
			Int_t tdcVal = map.tdc_index[index][pmt];
		
			if(adcVal ==-99){
				if(tdcVal != -99){
					cout << "map.adc_index["<<index<<"]["<<pmt<<"] == -99, but map.tdc_index["<<index<<"]["<<pmt<<"] == " << tdcVal << endl;
					mapIsUnique = false;
				}
			}
			
			if(tdcVal ==-99){
				if(adcVal != -99){
					cout << "map.tdc_index["<<index<<"]["<<pmt<<"] == -99, but map.adc_index["<<index<<"]["<<pmt<<"] == " << adcVal << endl;
					mapIsUnique = false;
				}
			}
			
			if(adcVal != -99 && tdcVal != -99) {
				
				//check the remaining pmts in this index.
				for(Int_t p = pmt+1; p < kNumPmtsPerMod; p++){
					
					if( (adcVal == map.adc_index[index][p]) || (tdcVal == map.tdc_index[index][p]) ){
						
						cout << "Check Muon Channel Map. Found Same Value for a/tdc_index[" << index << "][" 
						<< pmt << "] and a/tdc_index[" << index << "][" << p << "]" << endl;
						
						mapIsUnique = false;
					}					
				}
				
				//check the rest of the map
				for(Int_t i = index + 1; i < sizeOfIndex; i++){
					for(Int_t p = 0; p < kNumPmtsPerMod; p++){
						if( (adcVal == map.adc_index[i][p]) || (tdcVal == map.tdc_index[i][p]) ){
							
							cout << "Check Muon Channel Map. Found Same Value for a/tdc_index[" << index << "][" 
							<< pmt << "] and a/tdc_index[" << i << "][" << p << "]" << endl;
							
							mapIsUnique = false;
						}	
					}
				}
			
				
			}
			
		}
	}
	
	return mapIsUnique; 
}

Bool_t openMuonVetoFile(const Char_t* file)
{
	mMuonVetoRootFile = new TFile(file, "read");
	return mMuonVetoRootFile->IsOpen();
}

Bool_t openKEDSOutFile(const Char_t* file)
{
	mKEDSOutFile = new KDataWriter(file);
	if(mKEDSOutFile != 0){
		return mKEDSOutFile->IsReady();
	}
	else return false;
}

Bool_t setMuonVetoFileBranches(void)
{
	mMuonVetoData.fEventQualityBits = NULL;
	mMuonVetoData.fEventQualityBits = new TBits;
	
	if(mMuonVetoData.fEventQualityBits == NULL) return false;
	
	mMuonTree = (TTree *)mMuonVetoRootFile->Get("tree");
	if(mMuonTree == NULL)
		return false;
	
	mMuonTree->SetBranchAddress("10usStamp",&mMuonVetoData.f10muSecStamp);
	mMuonTree->SetBranchAddress("ADC",mMuonVetoData.fADC);
	mMuonTree->SetBranchAddress("ADC#",mMuonVetoData.fADCPound);
	mMuonTree->SetBranchAddress("Multi_ADC",&mMuonVetoData.fMultiADC);
	mMuonTree->SetBranchAddress("Dist_Est",&mMuonVetoData.fDistEst);
	mMuonTree->SetBranchAddress("Dist_Nemo",&mMuonVetoData.fDistNemo);
	mMuonTree->SetBranchAddress("EventNo.",&mMuonVetoData.fEventNo);
	mMuonTree->SetBranchAddress("File",&mMuonVetoData.fFile);
	mMuonTree->SetBranchAddress("Geometry",&mMuonVetoData.fGeometry);
	mMuonTree->SetBranchAddress("Multi",&mMuonVetoData.fMultiTDC);
	mMuonTree->SetBranchAddress("Run",&mMuonVetoData.fRun);
	mMuonTree->SetBranchAddress("Event_Binary_Quality",&mMuonVetoData.fEventQualityBits);
	mMuonTree->SetBranchAddress("TDC",mMuonVetoData.fTDC);
	mMuonTree->SetBranchAddress("TDC#",mMuonVetoData.fTDCPound);
	mMuonTree->SetBranchAddress("deltaT",&mMuonVetoData.fDeltaT);
	mMuonTree->SetBranchAddress("pcTimeSec",&mMuonVetoData.fPcTimeSec);
	mMuonTree->SetBranchAddress("pcTimeUSec",&mMuonVetoData.fPcTimeMicroSec);
	
	return true;
}

Bool_t setOutputKEDSBranches(void)
{
	if(mKEDSOutFile != 0){
		mEvent = dynamic_cast<KHLAEvent *>(mKEDSOutFile->GetEvent());
	}
	if(mEvent != 0) return true;
	else return false;
}

Int_t getMuonModulesData(Bool_t mMods[], Int_t mADCVals[][kNumPmtsPerMod], 
												 Int_t mTDCVals[][kNumPmtsPerMod], Int_t sizeOfMods, Int_t nPmts,
												Short_t adcArray[], Short_t tdcArray[], Int_t arraySize, 
												 MuonChannelMap &mMap, Bool_t &bIsNCCounter)
{
	//this function searches through the adcArray and the tdcArray
	//looking for non-zero values. When a non-zero value is found, it 
	//then deterimines which module number was hit by searching through
	// the _index arrays (defined below). the values in the _index arrays
	//are such that adc/tdc_index[i][k] = j means that module number i, pmt number
	// k corresponds to the jth element of the adcArray or tdcArray, respectively.
	
	//sizeOfMods is the size of the mMods array and should be equal to 49
	//which is the number of modules(48) plus one, since the first module number
	//is 1 and not module number 0. also, 
	//_index[50] and _index[51] below correspond to the channels
	//used by the neutron counter and _index[49] is not used
	
	const Int_t kEmpty = -1; //if the value in the adcArray or tdcArray == kEmpty, then there is no event.

	if(sizeOfMods != sizeOfIndex) return -1;
	if(nPmts != kNumPmtsPerMod) return -1;
	if(arraySize != fSizeOfArray) return -1;
	//a/tdc_index tells me the value in the list that corresponds to 
	//a particular muon module number. 
	
	//initialize the arrays to zero.
	memset(mMods, false, sizeOfMods*sizeof(Bool_t));
	//for(Int_t i = 0; i < sizeOfMods; i++) 
	//	mMods[i] = false;
	
	memset(mADCVals, kEmpty, sizeOfMods*nPmts*sizeof(Int_t));
	memset(mTDCVals, kEmpty, sizeOfMods*nPmts*sizeof(Int_t));
	
	Int_t nModulesHit = 0;
	for(Int_t i = 0; i < arraySize; i++){
		
		if(adcArray[i] != kEmpty){
			if(i==3 || i == 11 || i == 23 || i == 31) {
				bIsNCCounter = true;
			}

			for(Int_t moduleNum = 1; moduleNum < sizeOfIndex; moduleNum++){
				for(Int_t pmtNum = 0; pmtNum < kNumPmtsPerMod; pmtNum++){
					if(mMap.adc_index[moduleNum][pmtNum] == i){
						if(!mMods[moduleNum]) nModulesHit++; //count the number of Modules Hit, but prevent double counting
						mMods[moduleNum] = true;
						mADCVals[moduleNum][pmtNum] = adcArray[i];
						if(moduleNum == 23 || moduleNum == 24)
							cout << "ADC Hey - you can't be here!  moduleNum = " << moduleNum << endl;
						moduleNum = sizeOfIndex;
						pmtNum = kNumPmtsPerMod; //stop looping once you've found the correct module
					}
				}
			}
		}
		
		if(tdcArray[i] != kEmpty){
			if(i==3 || i == 11 || i == 23 || i == 31){
				bIsNCCounter = true;
			} 
			
			for(Int_t moduleNum = 1; moduleNum < sizeOfIndex; moduleNum++){
				for(Int_t pmtNum = 0; pmtNum < kNumPmtsPerMod; pmtNum++){
					if(mMap.tdc_index[moduleNum][pmtNum] == i){
						if(!mMods[moduleNum]) nModulesHit++; //count the number of Modules Hit, but prevent double counting
						mMods[moduleNum] = true;
						mTDCVals[moduleNum][pmtNum] = tdcArray[i];
						if(moduleNum == 23 || moduleNum == 24)
							cout << "TDC Hey - you can't be here!  moduleNum = " << moduleNum << endl;
						moduleNum = sizeOfIndex;
						pmtNum = kNumPmtsPerMod; //stop looping once you've found the correct module
					}
				}
			}
		}
	
	}
	
	return nModulesHit;
}

Bool_t fillEvents(void)
{
	
	MuonChannelMap mMuonChanMap;
	
	mMuonTree->GetEntry(0);
	initializeMuonChannelMap(mMuonChanMap, mMuonVetoData.fRun);
	if(!testMuonChannelMapForUniqueness(mMuonChanMap)) {
		cout << "The Muon Module ADC/TDC map is not unique!" << endl;
		cout << "  ***** This program will now terminate without filling the KDS file!" << endl;
		return -1;
	}
		
	
	Int_t numPureNCEvents = 0;
	Int_t numEventsWithNC = 0;
	Int_t numEmptyEvents = 0;
	Int_t nEvents = mMuonTree->GetEntries();
		
	for(Int_t entry = 0; entry < nEvents; entry++){
		
		if(entry%20000==0 && entry!=0 )
			cout << "Processing Entry " << entry << endl;
		
		mEvent->Clear("C");
		
		mMuonTree->GetEntry(entry);
		mMuonTree->SetScanField(100);
		mEvent->SetRunNumber(kKRunNumber);
		mEvent->SetRunStartTime(kKRun12StartTime);
		mEvent->SetRunEndTime(kKRun12EndTime);
		
		mEvent->SetStamp(mMuonVetoData.f10muSecStamp);
		
		//for now, just do this to set the event time. 
		Double_t microTime = 1.0e-6*mMuonVetoData.fPcTimeMicroSec;
		if(microTime > 1.0)
			microTime = 0; //this indicates a problem in the muon veto data stream. 
		
		mEvent->SetEventTriggerTime((Double_t)mMuonVetoData.fPcTimeSec + microTime);
		
		KHLAMuonVetoSysRecord *mMvSysRec = mEvent->GetMuonVetoSystemRecord();
		
		mMvSysRec->SetRunNumber(mMuonVetoData.fRun);
		mMvSysRec->SetIsSystemOn(true);
		mMvSysRec->SetDistanceEst(mMuonVetoData.fDistEst);
		mMvSysRec->SetDistanceNemo(mMuonVetoData.fDistNemo);
		mMvSysRec->SetEventNumber(mMuonVetoData.fEventNo);
		mMvSysRec->SetFileNumber(mMuonVetoData.fFile);
		mMvSysRec->SetPcTimeSec(mMuonVetoData.fPcTimeSec);
		mMvSysRec->SetPcTimeMuSec(mMuonVetoData.fPcTimeMicroSec);
		mMvSysRec->SetMultiADC(mMuonVetoData.fMultiADC);
		mMvSysRec->SetMultiTDC(mMuonVetoData.fMultiTDC);
		mMvSysRec->SetCommonStopTime(mMuonVetoData.fTDC[96]);
		mMvSysRec->SetEventQualityBits(mMuonVetoData.fEventQualityBits);
		
		Bool_t mModHits[kNumMuonModules+1];
		Int_t mADCVals[kNumMuonModules+1][kNumPmtsPerMod];
		Int_t mTDCVals[kNumMuonModules+1][kNumPmtsPerMod];
		Bool_t bIsNCHit = false;
		
		Int_t numberOfModsHit = getMuonModulesData(mModHits, mADCVals, mTDCVals, kNumMuonModules+1, 
																							 kNumPmtsPerMod, mMuonVetoData.fADC, mMuonVetoData.fTDC, 
																							 fSizeOfArray, mMuonChanMap, bIsNCHit);
		
		if(numberOfModsHit >= 0){
			//cout << "Found " << numberOfModsHit << " modules Hit" << endl;
			for(Int_t module = 1; module < kNumMuonModules+1; module++){
				if(mModHits[module]){
					KHLAMuonModuleSubRecord *mMod = mEvent->AddMuonModule();
					mMod->SetModuleNumber(module);
					for(Int_t pmt = 0; pmt < kNumPmtsPerMod; pmt++){
						mMod->SetAdc(pmt, mADCVals[module][pmt]);
						mMod->SetTdc(pmt, mTDCVals[module][pmt]);
					}
					mMod->SetAdcGroup(mMuonVetoData.fGeometry);
				}
			}
		}
		
		if (numberOfModsHit == 0 && !bIsNCHit){
			cout << "Found Event with No Muon Module ADC/TDC values and no event on the Neutron Counter ADC/TDC. " << endl;
			cout << "Event Number " << mMuonVetoData.fEventNo << " Entry Number " << entry << endl;
			//mMuonTree->Scan("10usStamp:EventNo.:Multi:ADC:ADC#:TDC:TDC#","","",1,entry);
			numEmptyEvents++;
		}
		
		if (numberOfModsHit == -1){
			cout << "Warning, getMuonModulesData failed because of wrong array size." << endl;
			cout << "Event Number " << mMuonVetoData.fEventNo << " Entry Number " << entry << endl;
		}
		
		if(numberOfModsHit > 0 && bIsNCHit){
			numEventsWithNC++;
			//this happens a lot ( > 1000 times per run)... so don't print every time. 
			//cout << "Found an event with both a Neutron Counter hit and Muon Module hit" << endl;
			//mMuonTree->Scan("10usStamp:EventNo.:Multi:ADC:ADC#:TDC:TDC#","","",1,entry);
		}
		
		if(numberOfModsHit == 0 && bIsNCHit){
			numEventsWithNC++;
			numPureNCEvents++;
		}
		
		/*if(numberOfModsHit > 0 || (numberOfModsHit == 0 && !bIsNCHit)) {
			mKEDSOutFile->Fill();
		}*/
		//always fill the tree!
		mKEDSOutFile->Fill();

	}
	
	cout << "End of Filling Events. Found " << numPureNCEvents << " events with ONLY a Neutron Counter hit" << endl;
	cout << "                       Found " << numEmptyEvents << " completely empty events" << endl;
	cout << "                       Found " << numEventsWithNC << " events with a Neutron Counter hit" << endl;
	cout << "There are " << mKEDSOutFile->GetEntries() << " events in the KEDS Tree" << endl;
	//cout << "There should be " << mKEdsTree->GetEntries() + numPureNCEvents  << " events in the Muon Veto Root File" << endl;
	cout << "There are " << nEvents << " events in the Muon Veto Root File" << endl;
	return true;
}

Bool_t writeCloseAndCleanup(void)
{
	cout << "Writing File and Closing" << endl;
	cout << endl;
	
	mKEDSOutFile->Write();
	mKEDSOutFile->Close();
	mMuonVetoRootFile->Close();
	
	delete mMuonVetoData.fEventQualityBits;
	delete mKEDSOutFile;
	/*
	if(mEvent!=NULL) delete mEvent;
	if(mKEdsTree!=NULL) delete mKEdsTree;
	if(mMuonTree!=NULL) delete mMuonTree;
	if(mKEDSOutFile!=NULL) delete mKEDSOutFile;
	if(mMuonVetoRootFile!=NULL) delete mMuonVetoRootFile;
	*/

	cout << "Have a nice day. Please come again." << endl;
	
	return true;
}

Int_t MuonVetoToDS(const Char_t* muonVetoFile, const Char_t* outKEDSFile)
{
	if(!openMuonVetoFile(muonVetoFile)){
		cout << "Can't open Muon Veto File" << endl;
		return -1;
	}
	cout << "Opened Muon Veto File " << muonVetoFile << endl;
	
	if(!openKEDSOutFile(outKEDSFile)){
		cout << "Can't open output KEDS File" << endl;
		return -1;
	}
	cout << "Opened output KEDS File " << outKEDSFile << endl;
	
	if(!setMuonVetoFileBranches()){
		cout << "Failed Setting Muon Veto Branches" << endl;
		return -1;
	}
	cout << "Setup Reading Muon Veto File Branches " << endl;
	
	if(!setOutputKEDSBranches()){
		cout << "Failed Setting Output KEDS Branches" << endl;
		return -1;
	}	
	cout << "Setup Output KEDS Branch Address" << endl;
	
	fillEvents();
	
	return writeCloseAndCleanup();
	
}



#ifndef __CINT__
//we only compile with the main if we're not in the CINT.
//you can still compile this file with the ACLiC compiler when you start a ROOT session, but then you 
//should call MuonVetoToDS directly since a function titled 'main' cannot be used in this case. 
int main(int argc, char* argv[])
{
	if(argc != 3) return -1;
	return MuonVetoToDS(argv[1], argv[2]);
}
#endif

