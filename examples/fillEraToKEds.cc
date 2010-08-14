/*
 *  fillEraToEds.cc
 *  KDataStructure
 *
 *  Created by Adam Cox on 6/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */



#include "TFile.h"
#include "KHLAEvent.h"
#include "KHLAMuonModuleSubRecord.h"
#include "KHLASingleBoloSubRecord.h"
#include "KHLASambaSubRecord.h"
#include "TTree.h"
#include <iostream>
#include <fstream>
#include "TRandom.h"
#include <string>
#include "KDataWriter.h"
#include "TProcessID.h"
#include <math.h>

const Int_t kNumberMyBolos = 13;
const Int_t kKRunNumber = 12;
const Double_t kKRun12StartTime = 1235646944.0; //Thu, 26 Feb 2009 11:15:44 +0000 (UTC) +        0 nsec  First event of Muon Veto Run 54.
const Double_t kKRun12EndTime = 1274115600.0; //Mon, 17 May 2010 15:00:00 +0000 (UTC) +        0 nsec.  Approximate end of Run 12. 


using namespace std;


//string manipulation to get detector name from input path
/*string GetDetectorNameDst( string inPath){
	inPath.replace(0, (inPath.find("dst_")+4),"");
	inPath.replace(inPath.find(".root"), 5,"");
	return inPath;
}*/

//read the File from runName and save it into an array of Long64_t
bool GetAnasStampCorrection(Long64_t* actualCorrection, string runName, string filePath ){
	runName.append("_tstnew_v1");
	filePath.append(runName);
	Int_t entries=0;
	ifstream inS(filePath.c_str(), ios::in);
	//string specialCase="jl08a008_tstnew_v1";
	//string sepcialCase="jl14c001_tstnew_v1"; //Erics dataset includes events of jl14c001_tstnew_v1 and jl14c002_tstnew_v1 but tells that they all come out of jl14c001_tstnew_v1. the change to compensate for this will be included in the fillEraToEds method
	
	string line;
	if(inS.is_open()){
		//Int_t grr=0;
		while(getline( inS, line )){
			//cout <<  line;
			entries++;
		}
		inS.close();
		ifstream inSt(filePath.c_str(), ios::in);
		getline(inSt,line);
		getline(inSt,line);
		getline(inSt,line);
		for(int i=1; i<2*(entries-3);i+=2){
			inSt >> actualCorrection[i] >> actualCorrection[i+1];
			
			
			
			/*if(runName==specialCase){
			 char c;
			 grr++;
			 if (grr%3000==0)cin >> c;
			 cout << actualCorrection[i] <<" : "<< actualCorrection[i+1] << " : "<<i<<endl;
			 if(grr==entries)cin>> c;
			 }*/
			/*if(((i-1)%2000)==0){working only the first time
			 int j;
			 cin>> j;
			 }*/
			// saving samba event number + stamp first real event at positions 2 and 3
		}
		inSt.close();
		return 1;
		
	}
	else {
		cout << "Can't open " <<filePath << endl;
		return 0;
	} 
}


//if new RunName outside 
Long64_t GetCorrectedStamp(Long64_t *stampArray, Int_t sambaNum,Int_t eventCount, string runName){
	Long64_t stamp;
	if((sambaNum==stampArray[eventCount*2-1])){
		stamp=stampArray[eventCount*2];
		return stamp;
	}
	else {
		if(sambaNum>stampArray[eventCount*2-1]){
			if((sambaNum==stampArray[sambaNum*2-1])){
				stamp=stampArray[sambaNum*2];
				return stamp;
			}
			else {
				cout<< "In File: "<< runName <<" SambaNumber not found at the right position "<< eventCount <<" in stampArray. Found: "<< stampArray[eventCount*2-1] << " : "<< sambaNum << endl;
				Int_t i;
				cin >> i;
				return -1;
			}
		}
		else{
			cout<< "In File: "<< runName <<" SambaNumber not found at the right position "<< eventCount <<" in stampArray. Found: "<< stampArray[eventCount*2-1] << " : "<< sambaNum << endl;
			Int_t i;
			cin >> i;
			return -1;
		}
	}
}

//There is a special case for file jl08a008, where samba numbering restarts during file at
// ev number 3149=1
//It seems that Eric left out the first 3148 events, but corrected it in a newer version, thus this lovely method is now obsolete
Long64_t GetCorrectedStampSpecial(Long64_t *stampArray, Int_t sambaNum, string runName){
	Long64_t stamp;
	Int_t help;
	help=sambaNum+3148;
	//cout << "sambaNum : help : "<< sambaNum << " : " << help << endl;
	if(((sambaNum)==stampArray[(help*2-1)])){
		stamp=stampArray[(help*2)];
		return stamp;
	}
	else {
		cout<< "In File: "<< runName <<" SambaNumber not found at the right position "<< (help*2-1) <<" in stampArray. Found: "<< stampArray[(help*2-1)] << " : "<< sambaNum << endl;
		Int_t i;
		cin >> i;
		return -1;
	}
}

void mySetZero(Long64_t *myArray, Int_t aNum){
	for(Int_t i=0; i<aNum; i++){
		myArray[i]=0;
	}
}

Int_t fillSambaDetectorNumber(string detectorName, Int_t boloNumber){
	if(boloNumber!=13){
		cout<< "Function fillSambaDetectorNumber assumes 12 Bolos (Configuration Run12)";
		return -1;
	}
	string allBolos[13]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	Int_t boloIndexToDetectorNumber[13][2] =
	{{0,0}, {1,9}, {2,32}, {3,11},
		{4,23}, {5,5}, {6,17}, {7,14},
		{8,26}, {9,2}, {10,20}, {11,29}, {12,8}};
	for(Int_t i=0; i< boloNumber;i++){
		if(detectorName==allBolos[i]) return boloIndexToDetectorNumber[i][1];
	}
	cout << "Detector Name " << detectorName << " not found in fillSamaDetectorNumber! " << endl;
	
	return -1;
}

int eraToKEds(string inputPath, string kDetectorName, string outputFile, string log, 
						 string tstNewPath="/kalinka/home/edelweiss/Bolo/Run12/tstnew/")
{
	
	ofstream fout(log.c_str()); //i'd rather not append to the log file. unless we put in some date and time information
	streambuf* cout_rdbuf=cout.rdbuf();
	cout << "Redirecting cout to output file "<< log <<"." << endl;
	streambuf* outBuf=fout.rdbuf();
	cout.rdbuf(outBuf);
	
	cout << "Building Eds file " << outputFile << " out of " << endl;
	
	string inputDstFile = inputPath; inputDstFile.append("dst_"); 
	inputDstFile.append(kDetectorName);
	inputDstFile.append(".root");
	
	string inputEIonFile = inputPath; inputEIonFile.append("eion_"); 
	inputEIonFile.append(kDetectorName);
	inputEIonFile.append(".root");
	
	string inputEHeatFile = inputPath; inputEHeatFile.append("eheat_"); 
	inputEHeatFile.append(kDetectorName);
	inputEHeatFile.append(".root");
	
	string inputCutsFile = inputPath; inputCutsFile.append("cuts_"); 
	inputCutsFile.append(kDetectorName);
	inputCutsFile.append(".root");
	
	cout << "Opening files: " << endl;
	cout << "               " << inputDstFile << endl;
	cout << "               " << inputEIonFile << endl;
	cout << "               " << inputEHeatFile << endl;
	cout << "               " << inputCutsFile << endl;
	
	//read in the DST tree file
	TFile inDstFile(inputDstFile.c_str(),"read");
	TTree *inDstTree =(TTree*)inDstFile.Get("dsttree");
	char runName[10]; //declaring variables for inTree
	Int_t eventNum, eventFlag, tOffset, chi2, ionFlags[6], vFlag, eventCount=1;
	Float_t eHeat, eIon, eRec, q;
	ULong64_t dateSec;
		
	inDstTree->SetBranchAddress("RunName", &runName);
	inDstTree->SetBranchAddress("EventNum", &eventNum);
	inDstTree->SetBranchAddress("DateSec", &dateSec);	
	inDstTree->SetBranchAddress("EventFlag", &eventFlag);
	inDstTree->SetBranchAddress("Eheat", &eHeat);
	inDstTree->SetBranchAddress("Eion", &eIon);
	inDstTree->SetBranchAddress("Toffset", &tOffset);
	inDstTree->SetBranchAddress("Chi2Flag", &chi2);
	inDstTree->SetBranchAddress("IonFlags", &ionFlags);
	inDstTree->SetBranchAddress("Erec", &eRec);
	inDstTree->SetBranchAddress("Q", &q);
	inDstTree->SetBranchAddress("Vflag", &vFlag); 
	
	cout << "DST file branches read" << endl;
	
	//read in the eion file
	TFile inEionFile(inputEIonFile.c_str(),"read");
	TTree *inEionTree =(TTree*)inEionFile.Get("energytree");
	Float_t e0col1, e0col2, e0gar1, e0gar2, e0vet1, e0vet2, ecol1, ecol2;
	Float_t efid, egar1, egar2, etot, evet1, evet2, ldbcol1, ldbcol2;
	Float_t ldbfid, ldbgar1, ldbgar2, ldbvet1, ldbvet2;
	//Int_t polarflag;
	//Bool_t vetocut;
	inEionTree->SetBranchAddress("E0col1", &e0col1);
	inEionTree->SetBranchAddress("E0col2", &e0col2);
	if(!kDetectorName.compare(0, 3, "FID") == 0) {
		inEionTree->SetBranchAddress("E0gar1", &e0gar1);
		inEionTree->SetBranchAddress("E0gar2", &e0gar2);
		inEionTree->SetBranchAddress("Egar1", &egar1);
		inEionTree->SetBranchAddress("Egar2", &egar2);
		inEionTree->SetBranchAddress("LdbGar1", &ldbgar1);
		inEionTree->SetBranchAddress("LdbGar2", &ldbgar2);
	}
	inEionTree->SetBranchAddress("E0vet1", &e0vet1);
	inEionTree->SetBranchAddress("E0vet2", &e0vet2);
	inEionTree->SetBranchAddress("Ecol1", &ecol1);
	inEionTree->SetBranchAddress("Ecol2", &ecol2);
	inEionTree->SetBranchAddress("Efid", &efid);
	
	inEionTree->SetBranchAddress("Etot", &etot);
	inEionTree->SetBranchAddress("Evet1", &evet1);
	inEionTree->SetBranchAddress("Evet2", &evet2);
	inEionTree->SetBranchAddress("LdbCol1", &ldbcol1);
	inEionTree->SetBranchAddress("LdbCol2", &ldbcol2);
	inEionTree->SetBranchAddress("LdbFid", &ldbfid);

	inEionTree->SetBranchAddress("LdbVet1", &ldbvet1);
	inEionTree->SetBranchAddress("LdbVet2", &ldbvet2);
	
	cout << "EIon file branches read" << endl;
	
	//read in the eheat file
	TFile inEHeatFile(inputEHeatFile.c_str(),"read");
	TTree *inEHeatTree =(TTree*)inEHeatFile.Get("energytree");
	Float_t  E0heat, LdbHeat;
	inEHeatTree->SetBranchAddress("E0heat", &E0heat);
	inEHeatTree->SetBranchAddress("LdbHeat", &LdbHeat);
	
	cout << "EHeat file branches read" << endl;
	//read in the cuts file
	TFile inCutsFile(inputCutsFile.c_str(),"read");
	TTree *inCutsTree =(TTree*)inCutsFile.Get("cuttree");
	Bool_t CutRun, CutHeatBase, CutCol1Base, CutCol2Base, CutVet1Base;
	Bool_t CutVet2Base, CutGar1Base, CutGar2Base, CutBases, CutChi2Heat;
	Bool_t CutChi2Col1, CutChi2Col2, CutChi2Vet1, CutChi2Vet2, CutChi2, CutChi2Gar1, CutChi2Gar2;
	inCutsTree->SetBranchAddress("CutRun", &CutRun);
	inCutsTree->SetBranchAddress("CutHeatBase", &CutHeatBase);
	inCutsTree->SetBranchAddress("CutCol1Base", &CutCol1Base);	
	inCutsTree->SetBranchAddress("CutCol2Base", &CutCol2Base);
	inCutsTree->SetBranchAddress("CutVet1Base", &CutVet1Base);
	inCutsTree->SetBranchAddress("CutVet2Base", &CutVet2Base);

	inCutsTree->SetBranchAddress("CutBases", &CutBases);
	inCutsTree->SetBranchAddress("CutChi2Heat", &CutChi2Heat);
	inCutsTree->SetBranchAddress("CutChi2Col1", &CutChi2Col1);
	inCutsTree->SetBranchAddress("CutChi2Col2", &CutChi2Col2);
	inCutsTree->SetBranchAddress("CutChi2Vet1", &CutChi2Vet1);
	inCutsTree->SetBranchAddress("CutChi2Vet2", &CutChi2Vet2);
	inCutsTree->SetBranchAddress("CutChi2", &CutChi2);
	if(!kDetectorName.compare(0, 3, "FID") == 0) {
		inCutsTree->SetBranchAddress("CutChi2Gar1", &CutChi2Gar1);
		inCutsTree->SetBranchAddress("CutChi2Gar2", &CutChi2Gar2);
		inCutsTree->SetBranchAddress("CutGar1Base", &CutGar1Base);
		inCutsTree->SetBranchAddress("CutGar2Base", &CutGar2Base);
	}
	

	cout << "Cuts file branches read" << endl;
	if(inDstTree->GetEntries() != inCutsTree->GetEntries() ||
		 inDstTree->GetEntries() != inEHeatTree->GetEntries() ||
		 inDstTree->GetEntries() != inEionTree->GetEntries() ||
		 inCutsTree->GetEntries() != inEHeatTree->GetEntries()  ||
		 inCutsTree->GetEntries() != inEionTree->GetEntries() ||
		 inEHeatTree->GetEntries() != inEionTree->GetEntries())
	{
		cout << "Didn't find the same number of entries in all of the ERA files. Quitting." << endl;
		return -1;
	}
	
	Long_t entries=inDstTree->GetEntries();
	cout << "Opening output file: " << outputFile << endl;
	
	KDataWriter fOutFile(outputFile.c_str());
	fOutFile.GetTTree()->BranchRef();
	//cout << "Open Root File" << endl;
	KHLAEvent *mEv = dynamic_cast<KHLAEvent*>(fOutFile.GetEvent());
	if(mEv == 0) {
		cout << "NULL pointer to event object" << endl;
		return -1;
	}
	
	//Long64_t* correctedStamps=NULL;
	
	string lastRunName="";
	//string specialRun="jl08a008";
	//string myStop="jd01a001";
	//string kDetectorName = GetDetectorNameDst(inputPath);
	
	Int_t detectorNumber= fillSambaDetectorNumber(kDetectorName, kNumberMyBolos);
	
	cout << "    " << entries  << " entries" << endl;
	
	Long64_t stamp=0;
	Int_t kSizeOfArray=200000;
	Long64_t *correctedStamps= new Long64_t[kSizeOfArray];
	Long64_t mPreviousStamp = 0;
	Bool_t bFillGoodEvent = true;
	Int_t fNumDuplicateStamps = 0;
	Bool_t firstElementFilejl14=true;
	string ericsSpecialRun="jl14c001";

	cout << "Beginning Loop" << endl;
	for(int i=0; i<entries; i++){// fill entry specific information
		start:
		inDstTree->GetEntry(i);
		inCutsTree->GetEntry(i);
		inEHeatTree->GetEntry(i);
		inEionTree->GetEntry(i);
		bFillGoodEvent = true;
		
		Int_t ObjectNumber = TProcessID::GetObjectCount();
		string sRunName = runName;
		/*if(sRunName.compare(0,2,"kb") == 0 || sRunName.compare(0,2,"kc") == 0|| 
		 sRunName.compare(0,2,"kd") == 0 || sRunName.compare(0,2,"ke") == 0||
		 sRunName.compare(0,2,"kf") == 0){ //stop prematurely because there seems to be a problem merging these new data.. a run-away process where the output file size from the merge routine gets extremely large
		 i = entries;
		 break;
		 }*/
		if(i%5000 == 0 && i!= 0){
		  cout << "Processing entry " << i << " out of " << entries << endl;
		}
		if(sRunName.compare(0,4,"jc27") != 0){  
			
			eventCount++;
			//standard event information
			mEv->SetRunNumber(12);
			mEv->SetRunStartTime(kKRun12StartTime);//WHERE TO PUT THIS RUN CONFIGURATION INFORMATION IT HAS TO BE SOMEWHERE WITH EASY ACCESS FOR THE FUTURE probabliy best to put in its own file and include this file StartTime and EndTime can be taken from the first and last Event once everything is filled in
			mEv->SetRunEndTime(kKRun12EndTime);
			
			mEv->SetEventTriggerTime((Double_t)dateSec);  //hmm... this doesn't work here! should be in the
			//samba record! There are more than one of these per bolo event.
			
			//getting the stamp:
			if(runName==ericsSpecialRun && eventNum==1){
				if(firstElementFilejl14){
				cout << runName << " and eventNumber: " << eventNum << endl;
				eventCount=1;
				mySetZero( correctedStamps, kSizeOfArray);
				GetAnasStampCorrection(correctedStamps, runName,tstNewPath );
				lastRunName=runName;
				firstElementFilejl14=false;
				}
				else{
				string temp="jl14c002";
				i=3093459;
				goto start;
				}
			}
			else{
				if(lastRunName!=runName){
					cout << runName << endl;
					eventCount=1;
					mySetZero( correctedStamps, kSizeOfArray);
					GetAnasStampCorrection(correctedStamps, runName,tstNewPath );
					lastRunName=runName;
				}
			}
			//if(runName==specialRun){
			//	stamp=GetCorrectedStampSpecial(correctedStamps, eventNum, runName);
			//}
			//else{
			stamp=GetCorrectedStamp(correctedStamps, eventNum, eventCount, runName);
			//}
			if(fabs(stamp - mPreviousStamp) < 2){
				//cout << "Found Duplicate Entry. Current STamp: " << stamp << " previous stamp " << mPreviousStamp << endl;
				fNumDuplicateStamps++;
				bFillGoodEvent = false;
			}
				
			
			if((stamp+1) % 100 == 0){
				//cout << "Old Stamp " << stamp; 
				stamp++; 
				//cout << " New Stamp " << stamp << endl;
				//stamp++;
			}
			
			mEv->SetStamp(stamp);
			//mEv->SetTriggerType(Int_t);
			//mEv->SetDataType(Int_t);
			//mEv->SetDetectorStatusWord(Int_t);
			//mEv->SetBlindnessWord(Int_t);
			//mEv->SetGSEventNumber(UInt_t);
			//mEv->SetDataCleaningWord(Int_t);
			KHLASingleBoloSubRecord* bolo = mEv->AddBolo();
			
			bolo->SetQvalue((double)q);
			bolo->SetEnergyRecoil((double)eRec);
			bolo->SetEnergyIon((double)eIon);
			bolo->SetEnergyHeat((double)eHeat);
			bolo->SetEventFlag(eventFlag);
			bolo->SetChi2Flag(chi2);
			bolo->SetVoltageFlag(vFlag);
			bolo->SetIonFlags(ionFlags,6);
			bolo->SetDetectorName(kDetectorName.c_str(),kDetectorName.length());
			bolo->SetIonPulseTimeOffset(tOffset);
			bolo->SetDetectorNumber(detectorNumber);
			
			bolo->SetEnergyCollectrode(0, ecol1);
			bolo->SetEnergyCollectrode(1, ecol2);
			bolo->SetEnergyVeto(0, evet1);
			bolo->SetEnergyVeto(1, evet2);
			
			bolo->SetEnergyBaselineCollectrode(0, e0col1);
			bolo->SetEnergyBaselineCollectrode(1, e0col2);
			bolo->SetEnergyBaselineVeto(0, e0vet1);
			bolo->SetEnergyBaselineVeto(1, e0vet2);
	
			bolo->SetBaselineNoiseCollectrode(0, ldbcol1);
			bolo->SetBaselineNoiseCollectrode(1, ldbcol2);
			bolo->SetBaselineNoiseVeto(0, ldbvet1);
			bolo->SetBaselineNoiseVeto(1, ldbvet2);

			bolo->SetEnergyIonFiducial(efid);
			bolo->SetEnergySumIonChannels(etot);
			bolo->SetBaselineIonFiducial(ldbfid);
			bolo->SetEnergyBaselineHeat(E0heat);
			bolo->SetBaselineNoiseHeat(LdbHeat);
			if(!kDetectorName.compare(0, 3, "FID") == 0) {
				bolo->SetEnergyGuard(0, egar1);
				bolo->SetEnergyGuard(1, egar2);
				bolo->SetEnergyBaselineGuard(0, e0gar1);
				bolo->SetEnergyBaselineGuard(1, e0gar2);
				bolo->SetBaselineNoiseGuard(0, ldbgar1);
				bolo->SetBaselineNoiseGuard(1, ldbgar2);
			}
			
			bolo->SetCutsBitNumber(0, CutChi2Col1);
			bolo->SetCutsBitNumber(1, CutChi2Col2);
			bolo->SetCutsBitNumber(2, CutChi2Vet1);
			bolo->SetCutsBitNumber(3, CutChi2Vet2);

			bolo->SetCutsBitNumber(6, CutChi2Heat);
			bolo->SetCutsBitNumber(7, CutChi2);
			bolo->SetCutsBitNumber(8, CutCol1Base);
			bolo->SetCutsBitNumber(9, CutCol2Base);
			bolo->SetCutsBitNumber(10, CutVet1Base);
			bolo->SetCutsBitNumber(11, CutVet2Base);

			bolo->SetCutsBitNumber(14, CutHeatBase);
			bolo->SetCutsBitNumber(15, CutBases);
			bolo->SetCutsBitNumber(16, CutRun);
			if(!kDetectorName.compare(0, 3, "FID") == 0){
				bolo->SetCutsBitNumber(4, CutChi2Gar1);
				bolo->SetCutsBitNumber(5, CutChi2Gar2);
				bolo->SetCutsBitNumber(12, CutGar1Base);
				bolo->SetCutsBitNumber(13, CutGar2Base);
			}
			
			
			//bolo->SetTriggerBit1(Int_t);
			//bolo->SetTriggerBit2(Int_t);
			//bolo->SetFiducialVolume(Double_t);	
			
			KHLASambaSubRecord* sam=mEv->AddSamba();
			sam-> SetSambaEventNumber(eventNum);
			sam-> SetRunName(runName, 10);
			sam-> SetNtpDateSec(dateSec);
			//sam-> SetNtpDateMicroSec(dateSec%1000);
			sam-> SetSambaDAQNumber(sRunName.compare(4,1,"a") + 1); 
			
			//sam-> SetPosition(Int_t aWord);
			//sam-> SetDelai(Double_t aWord);
			//sam-> SetSambaMajorVersionNum(Int_t aWord);
			//sam-> SetSambaMinorVersionNum(Int_t aWord);
			
			bolo->SetSambaRecord(sam);
			
			//KHLAMuonModuleSubRecord* muon=mEv->AddMuonModule();
			//muon-> SetModuleNumber(3);
			
			// For boloSysRecors:
			mEv->GetBoloSystemRecord()->SetRunName(sRunName);
			//mEv->GetHLABoloSysRecord()->SetRunStartTime(Double_t)
			//mEv->GetHLABoloSysRecord()->SetRunEndTime(Double_t) //currently doesn't make sense
			//for the bolo system because not all machines are started at the same time. 
			//maybe this will somehow change in the next version of Samba.
			//cout << "Before fill "<<bolo->GetDetectorName()<<endl;
			if(bFillGoodEvent) fOutFile.Fill();
			mPreviousStamp = stamp;
		}
		//cout << "After fill "<<bolo->GetDetectorName()<<endl;
		mEv->Clear("C");  
		//cout << "After clear "<<moBolo2->GetDetectorName()<<endl;
		
		//Restore Object count 
		//To save space in the table keeping track of all referenced objects
		//we assume that our events do not address each other. We reset the 
		//object count to what it was at the beginning of the event.
		TProcessID::SetObjectCount(ObjectNumber);
	}
	cout << "Number of Duplicate stamps: " << fNumDuplicateStamps << endl;
	
	
	cout << "Done filling events." << endl;
	fOutFile.Write();
	cout << "Write Tree" << endl;
	fOutFile.Close();
	cout << "Closing File" << endl;
	
	delete [] correctedStamps;
	inDstFile.Close();
	cout.rdbuf(cout_rdbuf);
	return fNumDuplicateStamps;
}
