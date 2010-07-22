/*
 *  filldstEvents.cc
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 4/6/10 and Benjamin Schmidt 14.04.10 .
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "TFile.h"
#include "EdwHLAEvent.h"
#include "EdwTestEventClass.h"
#include "EdwHLAMuonModuleSubRecord.h"
#include "EdwHLASingleBoloSubRecord.h"
#include "EdwHLASambaSubRecord.h"
#include "TTree.h"
#include <iostream>
#include <fstream>
#include "TRandom.h"
#include <string>
#include "EdwDSWriter.h"
#include "TProcessID.h"

const Int_t kNumberMyBolos = 13;
const Int_t kEdwRunNumber = 12;
const Double_t kEdwRun12StartTime = 1235646944.0; //Thu, 26 Feb 2009 11:15:44 +0000 (UTC) +        0 nsec  First event of Muon Veto Run 54.
const Double_t kEdwRun12EndTime = 1274115600.0; //Mon, 17 May 2010 15:00:00 +0000 (UTC) +        0 nsec.  Approximate end of Run 12. 


using namespace std;


//string manipulation to get detector name from input path
string GetDetectorNameDst( string inPath){
	inPath.replace(0, (inPath.find("dst_")+4),"");
	inPath.replace(inPath.find(".root"), 5,"");
	return inPath;
}

//read the File from runName and save it into an array of Long64_t
bool GetAnasStampCorrection(Long64_t* actualCorrection, string runName, string filePath ){
	runName.append("_tstnew_v1");
	filePath.append(runName);
	Int_t entries=0;
	ifstream inS(filePath.c_str(), ios::in);
	//string specialCase="jl08a008_tstnew_v1";

	string line;
	if(inS.is_open()){
		//Int_t grr=0;
		while(!inS.eof()){
			getline( inS, line );
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
			
			//testing!
			//cout << actualCorrection[i] << " " << actualCorrection[i+1];
			
			
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
		cout<< "Function fillSambaDetectorNumber assumes 13 Bolos (Configuration Run12)";
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
	cout << "Detector Name " << detectorName << " not found in fillSambaDetectorNumber! " << endl;
	
	return -1;
}

int dstToEds(string inputPath, string outputPath, string log, string tstNewPath="/kalinka/home/edelweiss/Bolo/Run12/tstnew/")
{
	
	ofstream fout(log.c_str(), ios::app); //i'd rather not append to the log file. unless we put in some date and time information // the problem with not appending to the logfile is that you don't have the information about the processing of the 13 bolometers, but only of the last one, maybe it would make sense to do sth. like if the logfile was created on another day create a new one else append. or give a flag for creating a new outputFile at the start of processing
	streambuf* cout_rdbuf=cout.rdbuf();
	cout << "Redirecting cout to output file "<< log <<"." << endl;
	streambuf* outBuf=fout.rdbuf();
	cout.rdbuf(outBuf);
	
	cout << "Building Eds file " << outputPath << " out of "<< inputPath << endl;
	TFile inFile(inputPath.c_str(),"read");
	TTree *inTree =(TTree*)inFile.Get("dsttree");
	char runName[10]; //declaring variables for inTree
	Int_t eventNum, eventFlag, tOffset, chi2, ionFlags[6], vFlag, eventCount=1;
	Float_t eHeat, eIon, eRec, q;
	ULong64_t dateSec;
	Long64_t stamp=0;
	Int_t kSizeOfArray=200000;
	Long64_t *correctedStamps= new Long64_t[kSizeOfArray];
	
	inTree->SetBranchAddress("RunName", &runName);
	inTree->SetBranchAddress("EventNum", &eventNum);
	inTree->SetBranchAddress("DateSec", &dateSec);	
	inTree->SetBranchAddress("EventFlag", &eventFlag);
	inTree->SetBranchAddress("Eheat", &eHeat);
	inTree->SetBranchAddress("Eion", &eIon);
	inTree->SetBranchAddress("Toffset", &tOffset);
	inTree->SetBranchAddress("Chi2Flag", &chi2);
	inTree->SetBranchAddress("IonFlags", &ionFlags);
	inTree->SetBranchAddress("Erec", &eRec);
	inTree->SetBranchAddress("Q", &q);
	inTree->SetBranchAddress("Vflag", &vFlag); 
	
	Long_t entries=inTree->GetEntries();
	
	EdwDSWriter fOutFile(outputPath.c_str());
	fOutFile.GetTTree()->BranchRef();
	//cout << "Open Root File" << endl;
	EdwHLAEvent *mEv = dynamic_cast<EdwHLAEvent*>(fOutFile.GetEvent());
	if(mEv == 0) {
		cout << "NULL pointer to event object" << endl;
		return -1;
	}
	
	//Long64_t* correctedStamps=NULL;
	
	string lastRunName="";
	//string specialRun="jl08a008";
	//string myStop="jd01a001";
	string mDetectorName = GetDetectorNameDst(inputPath);
	Int_t detectorNumber= fillSambaDetectorNumber(mDetectorName, kNumberMyBolos);
	cout << "    " << entries  << " entries" << endl;
	
	Long64_t mPreviousStamp = 0;
	Bool_t bFillGoodEvent = true;
	for(int i=0; i<entries; i++){// fill entry specific information
		inTree->GetEntry(i);
		Int_t ObjectNumber = TProcessID::GetObjectCount();
		bFillGoodEvent = true;
		string sRunName = runName;
		/*if(sRunName.compare(0,2,"kb") == 0 || sRunName.compare(0,2,"kc") == 0|| 
			 sRunName.compare(0,2,"kd") == 0 || sRunName.compare(0,2,"ke") == 0||
			 sRunName.compare(0,2,"kf") == 0){ //stop prematurely because there seems to be a problem merging these new data.. a run-away process where the output file size from the merge routine gets extremely large
		  i = entries;
		  break;
		}*/
		if(i%50000 == 0 && i!= 0){
		  cout << "Processing entry " << i << " out of " << entries << endl;
		}
		
		if(sRunName.compare(0,4,"jc27") != 0){  
			
			eventCount++;
			//standard event information
			mEv->SetRunNumber(12);
			mEv->SetRunStartTime(kEdwRun12StartTime);//WHERE TO PUT THIS RUN CONFIGURATION INFORMATION IT HAS TO BE SOMEWHERE WITH EASY ACCESS FOR THE FUTURE probabliy best to put in its own file and include this file StartTime and EndTime can be taken from the first and last Event once everything is filled in
			mEv->SetRunEndTime(kEdwRun12EndTime);
			
			mEv->SetEventTriggerTime((Double_t)dateSec);  //hmm... this doesn't work here! should be in the
			//samba record! There are more than one of these per bolo event.
			
			//getting the stamp:
			if(lastRunName!=runName){
				cout << runName << endl;
				eventCount=1;
				mySetZero( correctedStamps, kSizeOfArray);
				GetAnasStampCorrection(correctedStamps, runName,tstNewPath );
				lastRunName=runName;
			}
			//if(runName==specialRun){
			//	stamp=GetCorrectedStampSpecial(correctedStamps, eventNum, runName);
			//}
			//else{
			stamp=GetCorrectedStamp(correctedStamps, eventNum, eventCount, runName);
			if(fabs(stamp - mPreviousStamp) < 2)
				bFillGoodEvent = false;
			
			if((stamp+1) % 100 == 0){
				cout << "Old Stamp " << stamp++ << " New Stamp " << stamp << endl;
			}
			//}
			mEv->SetStamp(stamp);
			//mEv->SetTriggerType(Int_t);
			//mEv->SetDataType(Int_t);
			//mEv->SetDetectorStatusWord(Int_t);
			//mEv->SetBlindnessWord(Int_t);
			//mEv->SetGSEventNumber(UInt_t);
			//mEv->SetDataCleaningWord(Int_t);
			EdwHLASingleBoloSubRecord* mBolo2 = mEv->AddBolo();
			
			mBolo2->SetQvalue((double)q);
			mBolo2->SetEnergyRecoil((double)eRec);
			mBolo2->SetEnergyIon((double)eIon);
			mBolo2->SetEnergyHeat((double)eHeat);
			mBolo2->SetEventFlag(eventFlag);
			mBolo2->SetChi2Flag(chi2);
			mBolo2->SetVoltageFlag(vFlag);
			mBolo2->SetIonFlags(ionFlags,6);
			mBolo2->SetDetectorName(mDetectorName.c_str(),mDetectorName.length());
			mBolo2->SetIonPulseTimeOffset(tOffset);
			mBolo2->SetDetectorNumber(detectorNumber);
			//mBolo2->SetTriggerBit1(Int_t);
			//mBolo2->SetTriggerBit2(Int_t);
			//mBolo2->SetFiducialVolume(Double_t);	
			
			EdwHLASambaSubRecord* sam=mEv->AddSamba();
			sam-> SetSambaEventNumber(eventNum);
			sam-> SetRunName(runName, 10);
			sam-> SetNtpDateSec(dateSec);
			//sam-> SetNtpDateMicroSec(dateSec%1000);
			sam-> SetSambaDAQNumber(sRunName.compare(4,1,"a") + 1); 
			
			//sam-> SetPosition(Int_t aWord);
			//sam-> SetDelai(Double_t aWord);
			//sam-> SetSambaMajorVersionNum(Int_t aWord);
			//sam-> SetSambaMinorVersionNum(Int_t aWord);
			
			mBolo2->SetSambaRecord(sam);
			
			//EdwHLAMuonModuleSubRecord* muon=mEv->AddMuonModule();
			//muon-> SetModuleNumber(3);
			
			// For boloSysRecors:
			mEv->GetBoloSystemRecord()->SetRunName(sRunName);
			//mEv->GetHLABoloSysRecord()->SetRunStartTime(Double_t)
			//mEv->GetHLABoloSysRecord()->SetRunEndTime(Double_t) //currently doesn't make sense
			//for the bolo system because not all machines are started at the same time. 
			//maybe this will somehow change in the next version of Samba.
			//cout << "Before fill "<<mBolo2->GetDetectorName()<<endl;
			if(bFillGoodEvent) fOutFile.Fill();
			mPreviousStamp = stamp;
		}
		//cout << "After fill "<<mBolo2->GetDetectorName()<<endl;
		mEv->Clear("C");  
		//cout << "After clear "<<moBolo2->GetDetectorName()<<endl;
		
		//Restore Object count 
		//To save space in the table keeping track of all referenced objects
		//we assume that our events do not address each other. We reset the 
		//object count to what it was at the beginning of the event.
		TProcessID::SetObjectCount(ObjectNumber);
	}
	cout << "Done filling events." << endl;
	fOutFile.Write();
	cout << "Write Tree" << endl;
	fOutFile.Close();
	cout << "Closing File" << endl;
	
	delete [] correctedStamps;
	inFile.Close();
	cout.rdbuf(cout_rdbuf);
	return 0;
}
